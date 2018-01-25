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

#ifndef SAIL_VOWPALMODELIMPL_H
#define SAIL_VOWPALMODELIMPL_H

#include "wrapper.h"
#include "vowpal.pb.h"

namespace sail {
namespace vw {

class VowpalModelImpl : public VowpalModel, public Wrapper {
 public:
  int wrapperSerialize() override {
    return Wrapper::wrapperSerialize();
  }

  int wrapperDeserialize() override {
    return Wrapper::wrapperDeserialize();
  }
};

}
}

#endif //SAIL_VOWPALMODELIMPL_H
