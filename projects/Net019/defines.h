#ifndef DEFINES_H_
#define DEFINES_H_

#include "ac_int.h"
#include "ac_fixed.h"
#include "nnet_utils/nnet_types.h"

//hls-fpga-machine-learning insert numbers
#define N_INPUT_1_1 16
#define N_LAYER_2 20
#define N_LAYER_4 5

//hls-fpga-machine-learning insert layer-precision
typedef ac_fixed<16,6,true> model_default_t;
typedef ac_fixed<16,6,true> input_t;
typedef ac_fixed<16,6,true> layer2_t;
typedef ac_fixed<16,6,true> fc1_weight_t;
typedef ac_fixed<16,6,true> fc1_bias_t;
typedef ac_fixed<16,6,true> fc1_relu_default_t;
typedef ac_fixed<16,6,true> layer3_t;
typedef ac_fixed<16,6,true> layer4_t;
typedef ac_fixed<16,6,true> fc2_weight_t;
typedef ac_fixed<16,6,true> fc2_bias_t;
typedef ac_fixed<16,6,true> fc2_softmax_default_t;
typedef ac_fixed<16,6,true> result_t;

#endif
