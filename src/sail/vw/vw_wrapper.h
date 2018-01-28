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

#ifndef SAIL_VW_WRAPPER_H
#define SAIL_VW_WRAPPER_H

#include <algorithm>
#include <string>
#include <vector>

#include "vowpalwabbit/vw.h"

#include "sail/redis_command.h"

#define VWTYPE_ENCODING_VERSION 0
#define VW_PARAMS_SIZE 1024

namespace sail {

extern RedisModuleType *VwType;

struct VwTypeObject {
  vw *vw_;
  char parameters_[VW_PARAMS_SIZE];
};

/**
 * Create a new Vw type
 *
 * @param parameters vw parameters
 * @return VwTypeObject wrapping vw main object
 */
struct VwTypeObject *createVwTypeObject(const char *parameters = 0);

/**
 * rdb_load is called when loading data from the RDB file. It loads data in
 * the same format as rdb_save produces
 *
 * @param rdb
 * @param encver
 * @return
 */
void *vwTypeRdbLoad(RedisModuleIO *rdb, int encver);

/**
 * rdb_save is called when saving data to the RDB file
 * @param rdb
 * @param value
 */
void vwTypeRdbSave(RedisModuleIO *rdb, void *value);

/**
 * aof_rewrite is called when the AOF is being rewritten, and the module
 * needs to tell Redis what
 * is the sequence of commands to recreate the content of a given key
 * @param aof
 * @param key
 * @param value
 */
void vwTypeAofRewrite(RedisModuleIO *aof, RedisModuleString *key, void *value);

size_t vwTypeMemUsage(const void *value);

/**
 * free is called when a key with the module native type is deleted via DEL or in any other mean,
 * in order to let the module reclaim the memory associated with such a value.
 * @param value
 */
void vwTypeFree(void *value);

int vwNewCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);

class VwAccessor : public RedisCommand {
 public:
  VwAccessor(int nbargs) : RedisCommand(nbargs) {}
  VwTypeObject *getVwType(RedisModuleString *key);
};

class VwPredictCommand : public VwAccessor {
 public:
  VwPredictCommand() : VwAccessor(3) {};
  int run() override;
};

class VwFitCommand : public VwAccessor {
 public:
  VwFitCommand() : VwAccessor(3) {};

  int run() override;
};

class VwInitCommand : public RedisCommand {
 public:
  VwInitCommand();

  int run() override;
};

class VwGetCommand : public RedisCommand {
 public:
  VwGetCommand() : RedisCommand(2) {}

  int run() override;
};

class VwNewCommand : public RedisCommand {
 public:
  VwNewCommand() : RedisCommand(3) {}

  int run() override;
};

}

#endif //SAIL_VW_WRAPPER_H
