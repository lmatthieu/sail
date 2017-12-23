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

namespace sail {

int ReiNew::run() {
  RedisModuleString *model_repo = args(1);
  RedisModuleString *model_name = args(2);
  RedisModuleString *model_params = args(3);
  RedisModuleString *model_memory = args(4);

  // creates a new vw model with specified parameters
  RedisModuleCallReply *rep = RedisModule_Call(context(), "SAIL.VW.NEW",
                                               "ss", model_name, model_params);

  if (RedisModule_CallReplyType(rep) == REDISMODULE_REPLY_ERROR) {
    return RedisModule_ReplyWithCallReply(context(), rep);
  }

  // saving the model in the repository
  rep = RedisModule_Call(context(), "HSET",
                         "sss", model_repo, model_name, model_memory);

  if (RedisModule_CallReplyType(rep) == REDISMODULE_REPLY_ERROR) {
    return RedisModule_ReplyWithCallReply(context(), rep);
  }

  RedisModule_ReplyWithNull(context());

  return REDISMODULE_OK;
}

}


