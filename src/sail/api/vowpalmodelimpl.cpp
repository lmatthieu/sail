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

#include "vowpalmodelimpl.h"

namespace sail {
namespace vw {

VwMemoryBuffer::VwMemoryBuffer() {
  read_offset_ = 0;
  files.push_back(-1);
}

ssize_t VwMemoryBuffer::write_file(int fp, const void *data, size_t size) {
  auto buf = reinterpret_cast<const char *>(data);
  data_.insert(data_.end(), &buf[0], &buf[size]);
  return size;
}

ssize_t VwMemoryBuffer::read_file(int fp, void *data, size_t size) {
  size = min(size, data_.size() - read_offset_);
  std::copy(data_.data() + read_offset_,
            data_.data() + read_offset_ + size,
            reinterpret_cast<char *>(data));
  read_offset_ += size;
  return size;
}

}
}


