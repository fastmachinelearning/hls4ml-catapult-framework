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
#include <iostream>
#include <mc_scverify.h>

#include "myproject.h"
#include "parameters.h"

// Weights start

#ifndef __SYNTHESIS__
fc1_weight_t w2[320];
fc1_bias_t b2[20];
fc2_weight_t w4[100];
fc2_bias_t b4[5];
#else
fc1_weight_t w2[320] = {
#include "weights/w2.txt"
};
fc1_bias_t b2[20] = {
#include "weights/b2.txt"
};
fc2_weight_t w4[100] = {
#include "weights/w4.txt"
};
fc2_bias_t b4[5] = {
#include "weights/b4.txt"
};
#endif

// Weights end

// Layers modelled as blocks start

template<class T, int N>
struct channel_array {
   T arr[N]; 
};

void CCS_BLOCK(layer1)(
    input_t in[N_INPUT_1_1],
    ac_channel<channel_array<layer2_t, N_LAYER_2> > &out
) {
    channel_array<layer2_t, N_LAYER_2> layer2_out;
    nnet::dense<input_t, layer2_t, config2>(in, layer2_out.arr, w2, b2);
    
    out.write(layer2_out);
}

void CCS_BLOCK(layer2)(
    ac_channel<channel_array<layer2_t, N_LAYER_2> > &in,
    ac_channel<channel_array<layer3_t, N_LAYER_2> > &out
) {
    channel_array<layer2_t, N_LAYER_2> layer_in = in.read();
    channel_array<layer3_t, N_LAYER_2> layer_out;
    
    nnet::relu<layer2_t, layer3_t, relu_config3>(layer_in.arr, layer_out.arr);
    
    out.write(layer_out);
}

void CCS_BLOCK(layer3)(
    ac_channel<channel_array<layer3_t, N_LAYER_2> > &in,
    ac_channel<channel_array<result_t, N_LAYER_4> > &out
) {
    channel_array<layer3_t, N_LAYER_2> layer_in = in.read();
  
    channel_array<layer4_t, N_LAYER_4> layer4_out;
    nnet::dense<layer3_t, layer4_t, config4>(layer_in.arr, layer4_out.arr, w4, b4);
    
    out.write(layer4_out);
}

void CCS_BLOCK(layer4)(
    ac_channel<channel_array<layer4_t, N_LAYER_4> > &in,
    result_t out[N_LAYER_4],
    unsigned short &const_size_in_1,
    unsigned short &const_size_out_1
) {
    channel_array<layer4_t, N_LAYER_4> layer_in = in.read();
    
    nnet::softmax<layer4_t, result_t, softmax_config5>(layer_in.arr, out);

    const_size_in_1 = N_INPUT_1_1;
    const_size_out_1 = N_LAYER_4;
}

// Layers modelled as blocks end

void myproject(
    input_t fc1_input[N_INPUT_1_1],
    result_t layer5_out[N_LAYER_4],
    unsigned short &const_size_in_1,
    unsigned short &const_size_out_1
) {

#ifndef __SYNTHESIS__
    static bool loaded_weights = false;
    if (!loaded_weights) {
        nnet::load_weights_from_txt<fc1_weight_t, 320>(w2, "w2.txt");
        nnet::load_weights_from_txt<fc1_bias_t, 20>(b2, "b2.txt");
        nnet::load_weights_from_txt<fc2_weight_t, 100>(w4, "w4.txt");
        nnet::load_weights_from_txt<fc2_bias_t, 5>(b4, "b4.txt");
        loaded_weights = true;
    }
#endif

    // Uncomment the region below (*) and comment the region afterwards (**) if
    // you want to synthesise Dense019 as an entire model (i.e. without using any
    // blocks).

    /*
    // Region (*)
    const_size_in_1 = N_INPUT_1_1;
    const_size_out_1 = N_LAYER_4;

    layer2_t layer2_out[N_LAYER_2];
    nnet::dense<input_t, layer2_t, config2>(fc1_input, layer2_out, w2, b2); // fc1

    layer3_t layer3_out[N_LAYER_2];
    nnet::relu<layer2_t, layer3_t, relu_config3>(layer2_out, layer3_out); // fc1_relu

    layer4_t layer4_out[N_LAYER_4];
    nnet::dense<layer3_t, layer4_t, config4>(layer3_out, layer4_out, w4, b4); // fc2

    nnet::softmax<layer4_t, result_t, softmax_config5>(layer4_out, layer5_out); // fc2_softmax
    */

    // Uncomment the region below (**) and comment the region beforehand (*) if
    // you want to synthesise Dense019 as a connection of blocks.

    /*
    // Region (**)
    static ac_channel<channel_array<layer2_t, N_LAYER_2> > layer2_out;
    static ac_channel<channel_array<layer3_t, N_LAYER_2> > layer3_out;
    static ac_channel<channel_array<layer4_t, N_LAYER_4> > layer4_out;
    
    layer1(fc1_input, layer2_out);
    layer2(layer2_out, layer3_out);
    layer3(layer3_out, layer4_out);
    layer4(layer4_out, layer5_out, const_size_in_1, const_size_out_1);
    */

    // DO NOT LEAVE BOTH REGIONS UNCOMMENTED!
}
