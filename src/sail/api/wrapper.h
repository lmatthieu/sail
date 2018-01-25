/**
 * SAIL - A shallow/simple AI learning environment
 * Copyright (C) 2017 - 2018 Matthieu Lagacherie
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

#ifndef SAIL_WRAPPER_H
#define SAIL_WRAPPER_H

namespace sail {

/**
 * This class handle custom pre-serialization and post-serialization
 * operations.
 * Usage example: a raw binary object stored in an Any field in protocol
 * buffers
 */
class Wrapper {
 public:

  /**
   * This virtual method implements pre-serialization operations.
   * @return 0 if success
   */
  virtual int wrapperSerialize() {
    return 0;
  }
  /**
   * This virtual method implements post-deserialization operations.
   * @return 0 if success
   */
  virtual int wrapperDeserialize() {
    return 0;
  }
};

}

#endif //SAIL_WRAPPER_H
