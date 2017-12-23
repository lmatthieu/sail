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

#ifndef SAIL_REI_H
#define SAIL_REI_H

#include "sail/redis_command.h"
#include "redis_command.h"

namespace sail {

/**
 * SAIL.REI.NEW creates a new model based on a vowpal wabbit model.
 *
 * This command takes in input:
 * - model repository
 * - model name, must be unique (registered globaly as key)
 * - model parameters compatible with vowpal wabbit parameters
 * - memory namespace, must be unique. Stored as a sorted set
 */
class ReiNew : public RedisCommand {
 public:
  ReiNew() : RedisCommand(5) {}

  int run() override;
};

}

#endif //SAIL_REI_H
