/*******************************************************************************
* Copyright 2016-2017 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#include "mkldnn.h"

#include "c_types_map.hpp"
#include "nstl.hpp"
#include "primitive_desc.hpp"
#include "memory_pd.hpp"
#include "string.h"

using namespace mkldnn::impl;
using namespace mkldnn::impl::status;

status_t primitive_desc_t::query(query_t what, int idx, void *result) const {
    auto safe_ret_pd = [&](const memory_pd_t *_) {
        if (_ == nullptr) return not_required;
        *(const primitive_desc_t **)result = _;
        return success;
    };

    switch (what) {
        case query::engine: *(engine_t**)result = engine(); break;
        case query::primitive_kind: *(primitive_kind_t*)result = kind(); break;

        case query::input_pd: return safe_ret_pd(input_pd(idx));
        case query::output_pd: return safe_ret_pd(output_pd(idx));
        case query::src_pd: return safe_ret_pd(src_pd(idx));
        case query::diff_src_pd: return safe_ret_pd(diff_src_pd(idx));
        case query::dst_pd: return safe_ret_pd(dst_pd(idx));
        case query::diff_dst_pd: return safe_ret_pd(diff_dst_pd(idx));
        case query::weights_pd: return safe_ret_pd(weights_pd(idx));
        case query::diff_weights_pd: return safe_ret_pd(diff_weights_pd(idx));
        case query::workspace_pd:
            if (idx != 0) return status::invalid_arguments;
            return safe_ret_pd(workspace_pd(idx));

        case query::num_of_inputs_s32: *(int*)result = n_inputs(); break;
        case query::num_of_outputs_s32: *(int*)result = n_outputs(); break;

        case query::impl_info_str: *(const char **)result = name(); break;

        default: return unimplemented;
    }
    return success;
}

status_t mkldnn_primitive_desc_get_attr(const primitive_desc_t *primitive_desc,
        const primitive_attr_t **attr) {
    if (utils::any_null(primitive_desc, attr))
        return invalid_arguments;

    *attr = primitive_desc->attr();
    return success;
}

// vim: et ts=4 sw=4 cindent cino=^=l0,\:0,N-s
