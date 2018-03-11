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

#ifndef SAIL_SAILCONTEXT_H
#define SAIL_SAILCONTEXT_H

#include <string>
#include "redismodule.h"

namespace sail {

class RedisContextPolicy {
 public:
  explicit RedisContextPolicy(RedisModuleCtx *ctx) : ctx_(ctx) {
  }

  RedisModuleKey *openKey(RedisModuleString *key, int mode = REDISMODULE_READ
      | REDISMODULE_WRITE) {
    return reinterpret_cast<RedisModuleKey *>(RedisModule_OpenKey(ctx_,
                                                                  key, mode));
  }

  template<class T>
  T *getValue(const std::string &key) {
    RedisModuleString
        *rstr = RedisModule_CreateString(ctx_, key.data(), key.length());
    RedisModuleKey *rkey = openKey(rstr);
    int type = RedisModule_KeyType(rkey);

    if (type == REDISMODULE_KEYTYPE_EMPTY) {
      return nullptr;
    }

    T *obj = reinterpret_cast<T *>(
        RedisModule_ModuleTypeGetValue(rkey));

    return obj;
  }

 private:
  RedisModuleCtx *ctx_;
};

template<class ContextPolicy>
class Context : public ContextPolicy {
 public:
  using ContextPolicy::ContextPolicy;

  template<class T>
  T *getValue(const std::string &key) {
    return ContextPolicy::template getValue<T>(key);
  }
};

typedef Context<RedisContextPolicy> RedisContext;

}

#endif //SAIL_SAILCONTEXT_H
