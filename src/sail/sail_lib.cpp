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

#include "sail/redismodule.h"

int loadSail(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);



extern "C" {

int RedisModule_OnLoad(RedisModuleCtx *ctx,
                       RedisModuleString **argv,
                       int argc) {
  return loadSail(ctx, argv, argc);
}
}
