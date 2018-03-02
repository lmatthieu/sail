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

#ifndef SAIL_VOWPALSERVICE_IMPL_H
#define SAIL_VOWPALSERVICE_IMPL_H

#include "sail/api/vowpal_service.h"
#include "sail/api/vowpal.pb.h"

namespace sail {
namespace vw {

class VowpalServiceImpl : public VowpalService {
 public:
  virtual ~VowpalServiceImpl();
  void Fit(sail::RedisContext *context,
                   const ::sail::vw::VowpalModelRequest *request,
                   ::sail::vw::VowpalPrediction *response) override;
  void Predict(sail::RedisContext *context,
               const ::sail::vw::VowpalModelRequest *request,
               ::sail::vw::VowpalPrediction *response) override;
};

}
}

#endif //SAIL_VOWPALSERVICE_IMPL_H
