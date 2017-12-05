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

class RedisCommand {
 public:
  explicit RedisCommand(int nbargs) : nbargs_(nbargs) {}

  void init(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    ctx_ = ctx;
    for (size_t i = 0; i < argc; ++i)
      args_.push_back(argv[i]);
  }
  inline RedisModuleCtx *context() { return ctx_; }
  inline RedisModuleString *args(size_t i) { return args_[i]; }
  inline int getNbArgs() { return nbargs_; }

  virtual int run() = 0;

  RedisModuleKey *openKey(RedisModuleString *key, int mode = REDISMODULE_READ
      | REDISMODULE_WRITE);

 private:
  int nbargs_;
  RedisModuleCtx *ctx_;
  std::vector<RedisModuleString *> args_;
};

template<class Command>
static int run_command(RedisModuleCtx *ctx, RedisModuleString **argv, int
argc) {
  Command cmd;

  if (argc != cmd.getNbArgs()) {
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
