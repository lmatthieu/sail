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

#include <vowpalwabbit/example.h>
#include <vowpalwabbit/vw.h>
#include "sail/api/vowpalserviceimpl.h"
#include "vowpalmodelimpl.h"

namespace sail {
namespace vw {

VowpalServiceImpl::~VowpalServiceImpl() {

}

int VowpalServiceImpl::Fit(sail::RedisContext *context,
                            const ::sail::vw::VowpalModelRequest *request,
                            ::sail::vw::VowpalPrediction *response) {
  auto model = context->getValue<VowpalModelImpl>(request->model_id());
  auto ex = request->example();
  auto vw = model->getVw();

  example *vwex = VW::read_example(*vw, ex);
  vw->learn(vwex);

  response->set_prediction(vwex->pred.scalar);
  VW::finish_example(*vw, vwex);

  return 0;
}

int VowpalServiceImpl::Predict(sail::RedisContext *context,
                                const ::sail::vw::VowpalModelRequest *request,
                                ::sail::vw::VowpalPrediction *response) {
  auto model = context->getValue<VowpalModelImpl>(request->model_id());
  auto ex = request->example();
  auto vw = model->getVw();

  example *vwex = VW::read_example(*vw, ex);
  vw->l->predict(*vwex);

  response->set_prediction(vwex->partial_prediction);
  VW::finish_example(*vw, vwex);

  return 0;
}

}
}


