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
#include "sail/sail_rei.h"

int loadSail(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
  REDISMODULE_NOT_USED(argv);
  REDISMODULE_NOT_USED(argc);

  if (RedisModule_Init(ctx, "sail", 1, REDISMODULE_APIVER_1) ==
      REDISMODULE_ERR) {
    return REDISMODULE_ERR;
  }
  RedisModuleTypeMethods tm;

  tm.version = REDISMODULE_TYPE_METHOD_VERSION;
  tm.rdb_load = sail::vwTypeRdbLoad;
  tm.rdb_save = sail::vwTypeRdbSave;
  tm.aof_rewrite = sail::vwTypeAofRewrite;
  tm.mem_usage = sail::vwTypeMemUsage;
  tm.free = sail::vwTypeFree;

  sail::VwType = RedisModule_CreateDataType(ctx, "Vowpal_Tp", 0, &tm);

  if (sail::VwType == NULL)
    return REDISMODULE_ERR;

  sail::register_command<sail::VwPredictCommand>(ctx, "sail.vw.predict");
  sail::register_command<sail::VwFitCommand>(ctx, "sail.vw.fit");
  sail::register_command<sail::VwInitCommand>(ctx, "sail.vw.init");
  sail::register_command<sail::VwGetCommand>(ctx, "sail.vw.get");
  sail::register_command<sail::VwNewCommand>(ctx, "sail.vw.new");
  sail::register_command<sail::ReiNew>(ctx, "sail.rei.new");

  return REDISMODULE_OK;
}
