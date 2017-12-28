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

#ifndef SAIL_REDISCONTEXTWRAPPER_H
#define SAIL_REDISCONTEXTWRAPPER_H

#include <vector>

#include "redis_modules_sdk/rmutil/util.h"

namespace sail {

typedef long long sailbigint;

class RedisCommand {
 public:
  explicit RedisCommand(int minnbargs) : nbargs_(minnbargs) {}

  void init(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    ctx_ = ctx;
    for (size_t i = 0; i < argc; ++i)
      args_.push_back(argv[i]);
  }

  inline RedisModuleCtx *context() { return ctx_; }

  inline RedisModuleString *args(size_t i) {
    if (i < args_.size())
      return args_[i];
    return nullptr;
  }

  inline int getMinNbArgs() { return nbargs_; }

  inline size_t getNbArgs() const { return args_.size(); }

  virtual int run() = 0;

  RedisModuleKey *openKey(RedisModuleString *key, int mode = REDISMODULE_READ
      | REDISMODULE_WRITE);

  /**
   * Test the existence of a key in redis
   *
   * @param key the key to test
   * @return -1 if error, 1 if the key exists, 0 if the key does not exist.
   */
  int exists(RedisModuleString *key);

  /**
   * Creates a new RedisModuleString from a C string buffer
   *
   * @param ptr char buffer
   * @param len len of buffer, if 0 then strlen function is called
   * @return a RedisModuleString object
   */
  RedisModuleString *createString(const char *ptr, size_t len = 0);

  /**
   * HINCRBY REDIS command wrapper
   */
  sailbigint hincrby(const RedisModuleString *key,
                     RedisModuleString *field, sailbigint increment = 1);

 private:
  int nbargs_;
  RedisModuleCtx *ctx_;
  std::vector<RedisModuleString *> args_;
};

template<class Command>
static int run_command(RedisModuleCtx *ctx, RedisModuleString **argv, int
argc) {
  Command cmd;

  if (argc < cmd.getMinNbArgs()) {
    return RedisModule_WrongArity(ctx);
  }
  RedisModule_AutoMemory(ctx);

  cmd.init(ctx, argv, argc);

  return cmd.run();
}

template<class Command>
int register_command(RedisModuleCtx *ctx, const char *name) {
  RMUtil_RegisterWriteCmd(ctx, name, &run_command<Command>);
  return 0;
}
}

#endif //SAIL_REDISCONTEXTWRAPPER_H
