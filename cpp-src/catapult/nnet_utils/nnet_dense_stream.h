#ifndef NNET_DENSE_STREAM_H_
#define NNET_DENSE_STREAM_H_

#include "nnet_common.h"
#include "nnet_types.h"
#include <ac_channel.h>
#include <ac_float.h>
#include <math.h>
#include <assert.h>

namespace nnet {

template<class data_T, class res_T, typename CONFIG_T>
void dense_wrapper(
    data_T data[CONFIG_T::n_in],
    res_T  res[CONFIG_T::n_out],
    typename CONFIG_T::weight_t weights[CONFIG_T::n_in*CONFIG_T::n_out],
    typename CONFIG_T::bias_t   biases[CONFIG_T::n_out]
) {
    if (CONFIG_T::strategy == nnet::latency) {
        dense_latency<data_T, res_T, CONFIG_T>(data, res, weights, biases);
    } else {
        dense_resource<data_T, res_T, CONFIG_T>(data, res, weights, biases);
    }
}

template<class data_T, class res_T, typename CONFIG_T>
void dense(
    ac_channel<data_T> &data_stream,
    ac_channel<res_T>  &res_stream,
    typename CONFIG_T::weight_t weights[CONFIG_T::n_in*CONFIG_T::n_out],
    typename CONFIG_T::bias_t   biases[CONFIG_T::n_out])
{
    typename data_T::value_type data[CONFIG_T::n_in];

    typename res_T::value_type res[CONFIG_T::n_out];

    DataPrepare: for(int i_in = 0; i_in < CONFIG_T::n_in / data_T::size; i_in++) {
        if (CONFIG_T::n_in / data_T::size > 1) {
        }
        data_T data_pack = data_stream.read();
        DataPack: for (int i_pack = 0; i_pack < data_T::size; i_pack++) {
            data[i_in * data_T::size + i_pack] = data_pack[i_pack];
        }
    }

    dense_wrapper<typename data_T::value_type, typename res_T::value_type, CONFIG_T>(data, res, weights, biases);

    ResWrite: for(unsigned i_out = 0; i_out < CONFIG_T::n_out / res_T::size; i_out++) {
        if (CONFIG_T::n_out / res_T::size > 1) {
        }
        res_T res_pack;
        ResPack: for (int i_pack = 0; i_pack < res_T::size; i_pack++) {
            res_pack[i_pack] = res[i_out * res_T::size + i_pack];
        }
        res_stream.write(res_pack);
    }
}

}

#endif
