/**
 * SAIL - A shallow/simple AI learning environment
 * Copyright (C) 2017 Matthieu Lagacherie
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "sail/vw/vw_wrapper.h"

extern "C" {
#include <rmutil/sds.h>
}

namespace sail {

RedisModuleType *VwType = 0;

struct VwTypeObject *createVwTypeObject(const char *parameters) {
  auto vw = reinterpret_cast<VwTypeObject *>(RedisModule_Alloc(
      sizeof (VwTypeObject)));

  if (parameters == 0) {
    vw->vw_ = NULL;
  } else {
    vw->vw_ = VW::initialize(parameters);
    strncpy(vw->parameters_, parameters, VW_PARAMS_SIZE);
  }
  return vw;
}

void *vwTypeRdbLoad(RedisModuleIO *rdb, int encver) {
  if (encver != 0) {
    /* RedisModule_Log("warning","Can't load data with version %d", encver);*/
    return NULL;
  }
  struct VwTypeObject *vwto = createVwTypeObject();
  VwMemoryBuffer buf;

  size_t size = RedisModule_LoadUnsigned(rdb);
  char *cbuf = RedisModule_LoadStringBuffer(rdb, &size);
  size_t psize = RedisModule_LoadUnsigned(rdb);
  char *params = RedisModule_LoadStringBuffer(rdb, &psize);

  buf.write_file(-1, cbuf, size);

  vwto->vw_ = VW::initialize("", &buf);
  strncpy(vwto->parameters_, params, VW_PARAMS_SIZE);

  return vwto;
}

void vwTypeRdbSave(RedisModuleIO *rdb, void *value) {
  struct VwTypeObject *vwto = (struct VwTypeObject *) value;
  VwMemoryBuffer buf;

  VW::save_predictor(*(vwto->vw_), buf);

  RedisModule_SaveUnsigned(rdb, buf.size());
  RedisModule_SaveStringBuffer(rdb, buf.data(), buf.size());
  RedisModule_SaveUnsigned(rdb, VW_PARAMS_SIZE);
  RedisModule_SaveStringBuffer(rdb,
                               vwto->parameters_,
                               VW_PARAMS_SIZE);
}

void vwTypeAofRewrite(RedisModuleIO *aof, RedisModuleString *key, void *value) {
  VwMemoryBuffer buf;
  auto vwto = reinterpret_cast<VwTypeObject *>(value);

  VW::save_predictor(*(vwto->vw_), buf);
  sds obj_s = sdsempty();
  sds obj_p = sdsempty();

  sdscatlen(obj_s, buf.data(), buf.size());
  sdscatlen(obj_p, vwto->parameters_, VW_PARAMS_SIZE);

  RedisModule_EmitAOF(aof, "SAIL.VW.INIT", "sbb", key, obj_p, sdslen(obj_p),
                      obj_s, sdslen(obj_s));
  // sdsfree(obj_s);
  // sdsfree(obj_p);
}

size_t vwTypeMemUsage(const void *value) {
  return 0;
}

void vwTypeFree(void *value) {
  struct VwTypeObject *vo = (struct VwTypeObject *) value;

  if (vo->vw_ != NULL)
    VW::finish(*(vo->vw_));
  RedisModule_Free(vo);
}

VwMemoryBuffer::VwMemoryBuffer() {
  read_offset_ = 0;
  files.push_back(-1);
}

ssize_t VwMemoryBuffer::write_file(int fp, const void *data, size_t size) {
  auto buf = reinterpret_cast<const char *>(data);
  data_.insert(data_.end(), &buf[0], &buf[size]);
  return size;
}

ssize_t VwMemoryBuffer::read_file(int fp, void *data, size_t size) {
  size = min(size, data_.size() - read_offset_);
  std::copy(data_.data() + read_offset_,
            data_.data() + read_offset_ + size,
            reinterpret_cast<char *>(data));
  read_offset_ += size;
  return size;
}

VwPredictCommand::VwPredictCommand() : RedisCommand(3) {}

int VwPredictCommand::run() {
  auto key = openKey(args(1));
  int type = RedisModule_KeyType(key);

  // we test if the key is not empty and that the key have the right type
  if (type == REDISMODULE_KEYTYPE_EMPTY ||
      RedisModule_ModuleTypeGetType(key) != VwType) {
    return RedisModule_ReplyWithError(context(), REDISMODULE_ERRORMSG_WRONGTYPE);
  }

  // If the key is not empty
  struct VwTypeObject
      *vwto = reinterpret_cast<VwTypeObject *>(
      RedisModule_ModuleTypeGetValue(key));

  // we read example data
  size_t l1;
  const char *ex_str = RedisModule_StringPtrLen(args(2), &l1);

  example *ex = VW::read_example(*vwto->vw_, ex_str);
  vwto->vw_->l->predict(*ex);

  RedisModule_ReplyWithDouble(context(), ex->partial_prediction);
  VW::finish_example(*vwto->vw_, ex);

  return REDISMODULE_OK;
}

VwFitCommand::VwFitCommand() : RedisCommand(3) {}
int VwFitCommand::run() {
  auto key = openKey(args(1));
  int type = RedisModule_KeyType(key);

  // we test if the key is not empty and that the key have the right type
  if (type == REDISMODULE_KEYTYPE_EMPTY ||
      RedisModule_ModuleTypeGetType(key) != VwType) {
    return RedisModule_ReplyWithError(context(), REDISMODULE_ERRORMSG_WRONGTYPE);
  }

  // If the key is not empty
  struct VwTypeObject
      *vwto = reinterpret_cast<VwTypeObject *>(
      RedisModule_ModuleTypeGetValue(key));

  // we read example data
  size_t l1;
  const char *ex_str = RedisModule_StringPtrLen(args(2), &l1);

  example *ex = VW::read_example(*vwto->vw_, ex_str);
  vwto->vw_->learn(ex);

  RedisModule_ReplyWithDouble(context(), ex->pred.scalar);
  VW::finish_example(*vwto->vw_, ex);

  return REDISMODULE_OK;
}

VwInitCommand::VwInitCommand() : RedisCommand(4) {}
int VwInitCommand::run() {
  auto key = openKey(args(1));
  int type = RedisModule_KeyType(key);

  // we test if the key is not empty and that the key have the right type
  if (type != REDISMODULE_KEYTYPE_EMPTY &&
      RedisModule_ModuleTypeGetType(key) != VwType) {
    return RedisModule_ReplyWithError(context(), REDISMODULE_ERRORMSG_WRONGTYPE);
  }

  // If the key is empty or does not exists we create an new data structure
  size_t l1;
  const char *params = RedisModule_StringPtrLen(args(2), &l1);
  size_t l2;
  const char *c2 = RedisModule_StringPtrLen(args(3), &l2);

  VwMemoryBuffer buf;
  buf.write_file(-1, c2, l2);

  auto vwto = createVwTypeObject();

  vwto->vw_ = VW::initialize("", &buf);
  strncpy(vwto->parameters_, params, VW_PARAMS_SIZE);

  if (type != REDISMODULE_KEYTYPE_EMPTY) {
    RedisModule_DeleteKey(key);
    key = openKey(args(1));
  }
  RedisModule_ModuleTypeSetValue(key, VwType, vwto);

  RedisModule_ReplyWithNull(context());
  return REDISMODULE_OK;
}

int VwGetCommand::run() {
  auto key = openKey(args(1), REDISMODULE_READ);
  int type = RedisModule_KeyType(key);

  // we test if the key is not empty and that the key have the right type
  if (type == REDISMODULE_KEYTYPE_EMPTY ||
      RedisModule_ModuleTypeGetType(key) != VwType) {
    return RedisModule_ReplyWithError(context(), REDISMODULE_ERRORMSG_WRONGTYPE);
  }

  // If the key is not empty
  struct VwTypeObject
      *data = (struct VwTypeObject *) RedisModule_ModuleTypeGetValue(key);

  RedisModule_ReplyWithSimpleString(context(), data->parameters_);
  return REDISMODULE_OK;
}

int VwNewCommand::run() {
  auto key = openKey(args(1));
  int type = RedisModule_KeyType(key);

  // we test if the key is not empty and that the key have the right type
  if (type != REDISMODULE_KEYTYPE_EMPTY &&
      RedisModule_ModuleTypeGetType(key) != VwType) {
    return RedisModule_ReplyWithError(context(), REDISMODULE_ERRORMSG_WRONGTYPE);
  }

  // If the key is empty or does not exists we create an new data structure
  size_t l1;
  const char *c1 = RedisModule_StringPtrLen(args(2), &l1);
  struct VwTypeObject *data = createVwTypeObject(c1);

  if (type != REDISMODULE_KEYTYPE_EMPTY) {
    RedisModule_DeleteKey(key);
    key = openKey(args(1));
  }
  RedisModule_ModuleTypeSetValue(key, VwType, data);

  RedisModule_ReplyWithNull(context());
  return REDISMODULE_OK;
}
}
