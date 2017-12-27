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

#include "sail/sail_rei.h"
#include "sail/vw/vw_wrapper.h"

namespace sail {

int ReiNew::run() {
  size_t l1;
  RedisModuleString *model_repo_rs = args(1);
  RedisModuleString *model_params_rs = args(2);
  const char *model_repo = RedisModule_StringPtrLen(model_repo_rs, &l1);
  char model_name[255];
  char model_memory[255];

  sprintf(model_name, "%s:model", model_repo);
  sprintf(model_memory, "%s:memory", model_repo);

  RedisModuleString *model_name_rs = createString(model_name);
  RedisModuleString *model_memory_rs = createString(model_memory);

  // checking the non-existence of keys
  if (exists(model_repo_rs) != 0 || exists(model_name_rs)
      || exists(model_memory_rs)) {
    return RedisModule_ReplyWithError(context(),
                                      "Conflict while creating keys.");
  }

  // creates a new vw model with specified parameters
  RedisModuleCallReply *rep = RedisModule_Call(context(), "SAIL.VW.NEW",
                                               "ss", model_name_rs,
                                               model_params_rs);

  if (RedisModule_CallReplyType(rep) == REDISMODULE_REPLY_ERROR) {
    return RedisModule_ReplyWithCallReply(context(), rep);
  }

  // saving the model in the repository
  rep = RedisModule_Call(context(), "HMSET",
                         "sssssss", model_repo_rs,
                         createString("model"), model_name_rs,
                         createString("memory"), model_memory_rs,
                         createString("parameters"), model_params_rs);

  if (RedisModule_CallReplyType(rep) == REDISMODULE_REPLY_ERROR) {
    return RedisModule_ReplyWithCallReply(context(), rep);
  }

  RedisModule_ReplyWithNull(context());

  return REDISMODULE_OK;
}

int ReiPredict::run() {
  RedisModuleString *model_repo_rs = args(1);
  RedisModuleString *model_example_rs = args(2);

  // get model metadatas
  RedisModuleCallReply *rep = RedisModule_Call(context(), "HMGET",
                                               "sss", model_repo_rs,
                                               createString("model"),
                                               createString("memory"));

  if (RedisModule_CallReplyType(rep) != REDISMODULE_REPLY_ARRAY
      || RedisModule_CallReplyLength(rep) != 2) {
    return RedisModule_ReplyWithError(context(),
                                      "Cannot get model metadata.");
  }
  auto model_reply = RedisModule_CallReplyArrayElement(rep, 0);
  auto model_rs = RedisModule_CreateStringFromCallReply(model_reply);
  auto example_reply = RedisModule_CallReplyArrayElement(rep, 1);
  auto example_rs = RedisModule_CreateStringFromCallReply(example_reply);

  /////////////////////
  // get the prediction
  VwTypeObject *vwto = getVwType(model_rs);

  // we read example data
  size_t l1;
  const char *ex_str = RedisModule_StringPtrLen(example_rs, &l1);

  example *ex = VW::read_example(*vwto->vw_, ex_str);
  vwto->vw_->l->predict(*ex);

  RedisModule_ReplyWithDouble(context(), ex->partial_prediction);
  VW::finish_example(*vwto->vw_, ex);

  // add example to the memory

  return 0;
}
}


