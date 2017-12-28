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

#include <sail/redis_command.h>

#include <cstring>

namespace sail {

RedisModuleKey *RedisCommand::openKey(RedisModuleString *key, int mode) {
  return reinterpret_cast<RedisModuleKey *>(RedisModule_OpenKey(context(),
                                                                key, mode));
}

int RedisCommand::exists(RedisModuleString *key) {
  RedisModuleCallReply *rep = RedisModule_Call(context(), "EXISTS",
                                               "s", key);

  if (RedisModule_CallReplyType(rep) == REDISMODULE_REPLY_INTEGER) {
    return (int) RedisModule_CallReplyInteger(rep);
  }
  return -1;
}

RedisModuleString *RedisCommand::createString(const char *ptr, size_t len) {
  if (len == 0)
    len = strlen(ptr);
  return RedisModule_CreateString(context(), ptr, len);
}

sailbigint RedisCommand::hincrby(RedisModuleString *key,
                                 RedisModuleString *field,
                                 sailbigint increment) {
  auto rep = RedisModule_Call(context(), "HSET", "ssl",
                              key, field, increment);

  if (RedisModule_CallReplyType(rep) == REDISMODULE_REPLY_ERROR) {
    return -1;
  }

  return RedisModule_CallReplyInteger(rep);
}

}