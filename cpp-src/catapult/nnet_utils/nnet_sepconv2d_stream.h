#ifndef NNET_SEPARABLE_CONV2D_STREAM_H_
#define NNET_SEPARABLE_CONV2D_STREAM_H_

#include "nnet_common.h"
#include <ac_channel.h>
#include <ac_float.h>
#include "nnet_sepconv_stream.h"
#include "nnet_conv2d_stream.h"

namespace nnet {

template<class data_T, class res_T, typename CONFIG_T>
void depthwise_conv_2d_cl(
    ac_channel<data_T> &data,
    ac_channel<res_T>  &res,
    typename CONFIG_T::weight_t weights[CONFIG_T::filt_height * CONFIG_T::filt_width * CONFIG_T::n_chan],
    typename CONFIG_T::bias_t   biases[CONFIG_T::n_chan])
{
    assert(CONFIG_T::pad_top == 0 && CONFIG_T::pad_bottom == 0 && CONFIG_T::pad_left == 0 && CONFIG_T::pad_right == 0);
    assert(CONFIG_T::filt_height == CONFIG_T::filt_width);
    assert(CONFIG_T::stride_height <= CONFIG_T::filt_height && CONFIG_T::stride_width <= CONFIG_T::filt_width);

    ac_channel<typename data_T::value_type> data_window[CONFIG_T::filt_height * CONFIG_T::filt_width * CONFIG_T::n_chan];
    const int win_depth = CONFIG_T::filt_height * CONFIG_T::out_width;
    for (unsigned i_out = 0; i_out < CONFIG_T::filt_height * CONFIG_T::filt_width * CONFIG_T::n_chan; i_out++) {
    }


    res_T res_pack;
    unsigned outputs_ready = 0;

    ac_int<CONFIG_T::filt_height * CONFIG_T::filt_width, false> pixel_idx[data_T::size / CONFIG_T::n_chan];

    ReadInputHeight: for (unsigned i_ih = 0; i_ih < CONFIG_T::in_height; i_ih++) {
        ReadInputWidth: for (unsigned i_iw = 0; i_iw < CONFIG_T::in_width / (data_T::size / CONFIG_T::n_chan); i_iw++) {
            if (CONFIG_T::strategy == nnet::latency && data_T::size / CONFIG_T::n_chan == 1) {
            }
            compute_scaled_indices_2d<data_T, CONFIG_T>(i_ih, i_iw, pixel_idx);
            compute_depthwise_output<data_T, res_T, CONFIG_T>(data.read(), data_window, res, res_pack, outputs_ready, weights, biases, pixel_idx);
        }
    }
}

template<class data_T, class res_T, typename CONFIG_T>
void pointwise_conv_2d_cl(
    ac_channel<data_T> &data,
    ac_channel<res_T>  &res,
    typename CONFIG_T::weight_t weights[CONFIG_T::n_chan * CONFIG_T::n_filt],
    typename CONFIG_T::bias_t   biases[CONFIG_T::n_filt])
{
    assert(CONFIG_T::pad_top == 0 && CONFIG_T::pad_bottom == 0 && CONFIG_T::pad_left == 0 && CONFIG_T::pad_right == 0);
    assert(CONFIG_T::filt_height == 1 && CONFIG_T::filt_width == 1);


    ReadInputHeight: for (unsigned i_ih = 0; i_ih < CONFIG_T::in_height; i_ih++) {
        ReadInputWidth: for (unsigned i_iw = 0; i_iw < CONFIG_T::in_width / (data_T::size / CONFIG_T::n_chan); i_iw++) {
            if (CONFIG_T::strategy == nnet::latency && data_T::size / CONFIG_T::n_chan == 1) {
            }
            if (i_ih % CONFIG_T::stride_height == 0 && i_iw % CONFIG_T::stride_width == 0) {
                pointwise_mult_buffer<data_T, res_T, CONFIG_T>(data.read(), res, weights, biases);
            } else {
                data.read();
            }
        }
    }
}


template<class data_T, class res_T, typename CONFIG_T>
void separable_conv_2d_cl(
    ac_channel<data_T> &data,
    ac_channel<res_T>  &res,
    typename CONFIG_T::depthwise_config::weight_t depthwise_weights[CONFIG_T::depthwise_config::filt_height * CONFIG_T::depthwise_config::filt_width * CONFIG_T::depthwise_config::n_chan],
    typename CONFIG_T::pointwise_config::weight_t pointwise_weights[CONFIG_T::pointwise_config::n_chan * CONFIG_T::pointwise_config::n_filt],
    typename CONFIG_T::depthwise_config::bias_t   depthwise_biases[CONFIG_T::depthwise_config::n_chan],
    typename CONFIG_T::pointwise_config::bias_t   pointwise_biases[CONFIG_T::pointwise_config::n_filt]
) {

    ac_channel<data_T> depthwise_res;
    unsigned res_depth = CONFIG_T::depthwise_config::out_height * CONFIG_T::depthwise_config::out_width;

    depthwise_conv_2d_cl<data_T, data_T, typename CONFIG_T::depthwise_config>(data, depthwise_res, depthwise_weights, depthwise_biases);
    pointwise_conv_2d_cl<data_T, res_T, typename CONFIG_T::pointwise_config>(depthwise_res, res, pointwise_weights, pointwise_biases);
}

}
#endif
