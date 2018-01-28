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

#include "sail/api/wrapper.h"
#include "sail/api/vowpal.pb.h"
#include "vowpalwabbit/vw.h"

namespace sail {
namespace vw {

/**
 * This class is used to serialize / deserialize Vowpal wabbit mail object.
 *
 * The code behavior is inspired from the vowpal class memory_io_buf
 * available in the vwdll.cpp file.
 */
class VwMemoryBuffer : public io_buf {
 public:
  VwMemoryBuffer();

  ssize_t write_file(int fp, const void *data, size_t size) override;

  ssize_t read_file(int fp, void *data, size_t size) override;

  inline char *data() {
    return data_.data();
  }

  inline size_t size() const {
    return data_.size();
  }

 private:
  std::vector<char> data_;
  size_t read_offset_;
};


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
