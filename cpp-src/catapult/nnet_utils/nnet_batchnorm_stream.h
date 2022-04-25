//
//    rfnoc-hls-neuralnet: Vivado HLS code for neural-net building blocks
//
//    Copyright (C) 2017 EJ Kreinar
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef NNET_BATCHNORM_STREAM_H_
#define NNET_BATCHNORM_STREAM_H_

#include "nnet_common.h"
#include "nnet_mult.h"
#include "nnet_types.h"
#include <ac_channel.h>
#include <ac_float.h>

namespace nnet {

// ****************************************************
//       Streaming Batch Normalization
// ****************************************************

template<class data_T, class res_T, typename CONFIG_T>
void normalize(
    ac_channel<data_T> &data,
    ac_channel<res_T>  &res,
    typename CONFIG_T::scale_t scale[CONFIG_T::n_in],
    typename CONFIG_T::bias_t  bias[CONFIG_T::n_in]
) {

    constexpr unsigned multiplier_limit = DIV_ROUNDUP(CONFIG_T::n_in, CONFIG_T::reuse_factor);
    constexpr unsigned ii = CONFIG_T::n_in / multiplier_limit;
    CONFIG_T::template product<typename data_T::value_type, typename CONFIG_T::scale_t>::limit(multiplier_limit);
    BatchNormLoop: for (int i = 0; i < CONFIG_T::n_in / data_T::size; i++) {

        data_T in_data = data.read();
        res_T out_data;

        BatchNormpack: for (int j = 0; j < data_T::size; j++) {
            int norm_index;
            if (CONFIG_T::n_filt==-1) {
                norm_index = i * data_T::size + j;
            } else {
                norm_index = j % CONFIG_T::n_filt;
            }
            out_data[j] = CONFIG_T::template product<typename data_T::value_type, typename CONFIG_T::scale_t>::product(in_data[j], scale[norm_index]) + bias[norm_index];
        }

        res.write(out_data);
    }
}

// ****************************************************
//       Merged Batch Normalization and Quantized Tanh
// ****************************************************
template<class data_T, typename CONFIG_T>
void normalize_binary_tanh(
    ac_channel<data_T> &data,
    ac_channel<nnet::array<ac_int<1, false>, CONFIG_T::n_in>> &res,
    typename data_T::value_type threshold[CONFIG_T::n_in]
) {

    BinaryNormLoop: for (int i = 0; i < CONFIG_T::n_in / data_T::size; i++) {

        data_T in_data = data.read();
        nnet::array<ac_int<1, false>, CONFIG_T::n_in> out_data;

        BatchNormPack: for (int j = 0; j < data_T::size; j++) {
            out_data[j] = (in_data[j] > threshold[i * data_T::size + j]) ? 1 : 0;
        }

        res.write(out_data);
    }
}

template<class data_T, typename CONFIG_T>
void normalize_ternary_tanh(
    ac_channel<data_T> &data,
    ac_channel<nnet::array<ac_int<2, true>, CONFIG_T::n_in>> &res,
    typename data_T::value_type threshold_hi[CONFIG_T::n_in],
    typename data_T::value_type threshold_lo[CONFIG_T::n_in]
) {

    TernaryNormLoop: for (int i = 0; i < CONFIG_T::n_in / data_T::size; i++) {

        data_T in_data = data.read();
        nnet::array<ac_int<2, true>, CONFIG_T::n_in> out_data;

        BatchNormPack: for (int j = 0; j < data_T::size; j++) {
            
            int norm_index = i * data_T::size + j;

            if (in_data[j] > threshold_hi[norm_index]) {
                out_data[j] = 1;
            } else if (in_data[j] <= threshold_lo[norm_index]) {
                out_data[j] = -1;
            } else {
                out_data[j] = 0;
            }
        }

        res.write(out_data);
    }
}

}

#endif
