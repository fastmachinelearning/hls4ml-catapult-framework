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

#ifndef NNET_ACTIVATION_H_
#define NNET_ACTIVATION_H_

#include <cmath>
#include "ac_fixed.h"
#include "nnet_common.h"

namespace nnet {

struct activ_config
{
    // IO size
    static const unsigned n_in = 10;

    // Internal info
    static const unsigned table_size = 1024;

    // Resource reuse info
    static const unsigned io_type = io_parallel;
    static const unsigned reuse_factor = 1;

    // Internal data type definitions
    typedef ac_fixed<18,8, true> table_t;
};

// *************************************************
//       LINEAR Activation -- See Issue 53
// *************************************************
template<class data_T, class res_T, typename CONFIG_T>
void  linear(data_T data[CONFIG_T::n_in], res_T res[CONFIG_T::n_in])
{
    if (CONFIG_T::io_type == io_parallel){
    }

    Linear: for (int ii=0; ii<CONFIG_T::n_in; ii++) {
        if (CONFIG_T::io_type == io_serial){
        }
        res[ii] = data[ii];
    }
}



// *************************************************
//       RELU Activation
// *************************************************
template<class data_T, class res_T, typename CONFIG_T>
void  relu(data_T data[CONFIG_T::n_in], res_T res[CONFIG_T::n_in])
{
    if (CONFIG_T::io_type == io_parallel){
    }

    data_T datareg;
    Relu: for (int ii=0; ii<CONFIG_T::n_in; ii++) {
        if (CONFIG_T::io_type == io_serial){
        }
        datareg = data[ii];
        if (datareg > 0) res[ii] = datareg;
        else res[ii] = 0;
    }
}

template<class data_T, class res_T, int MAX_INT, typename CONFIG_T>
void  relu_max(data_T data[CONFIG_T::n_in], res_T res[CONFIG_T::n_in])
{
    if (CONFIG_T::io_type == io_parallel){
    }

    data_T datareg;
     ReluMax: for (int ii=0; ii<CONFIG_T::n_in; ii++) {
        if (CONFIG_T::io_type == io_serial){
        }
        datareg = data[ii];
        if (datareg < 0) res[ii] = 0;
        else if (datareg > MAX_INT) res[ii] = MAX_INT;
        else res[ii] = datareg;
    }
}

template<class data_T, class res_T, typename CONFIG_T>
void  relu6(data_T data[CONFIG_T::n_in], res_T res[CONFIG_T::n_in])
{
    relu_max<data_T, res_T, 6, CONFIG_T>(data, res);
}

template<class data_T, class res_T, typename CONFIG_T>
void  relu1(data_T data[CONFIG_T::n_in], res_T res[CONFIG_T::n_in])
{
    relu_max<data_T, res_T, 1, CONFIG_T>(data, res);
}

// *************************************************
//       Sigmoid Activation
// *************************************************
template<class data_T, class res_T, typename CONFIG_T>
void  sigmoid(data_T data[CONFIG_T::n_in], res_T res[CONFIG_T::n_in])
{
    // Initialize the lookup table
#ifdef __HLS_SYN__
    typename CONFIG_T::table_t sigmoid_table[CONFIG_T::table_size] = { 
#include <sigmoid_table.h>
    };
#else
    static typename CONFIG_T::table_t sigmoid_table[CONFIG_T::table_size] = {
#include <sigmoid_table.h>
    };
#endif
    
    // TODO: Init tables

    if (CONFIG_T::io_type == io_parallel){
    }

    // Index into the lookup table based on data
    int data_round;
    int index;
    Sigmoid: for (int ii=0; ii<CONFIG_T::n_in; ii++) {
        if (CONFIG_T::io_type == io_serial){
        }
        data_round = (data[ii]*CONFIG_T::table_size/16).to_int();
        index = data_round + 8*CONFIG_T::table_size/16;
        if (index < 0)   index = 0;
        if (index > CONFIG_T::table_size-1) index = CONFIG_T::table_size-1;
        res[ii] = (res_T) sigmoid_table[index];
    }
}

// *************************************************
//       Softmax Activation
// *************************************************

enum class softmax_implementation {latency=0, legacy=1, stable=2};

template<class data_T, typename CONFIG_T>
inline unsigned softmax_idx_from_real_val(data_T x){
    // Slice the top N bits to get an index into the table
    static constexpr int N = ceillog2(CONFIG_T::table_size); // number of address bits for table
    ac_int<N, false> y = x.template slc<N>(x.width-N); // slice the top N bits of input
    return y.template slc<N>(0).to_uint();
}

template <class data_T, class res_T, typename CONFIG_T>
void softmax_latency(data_T data[CONFIG_T::n_in], res_T res[CONFIG_T::n_in]){
    // Initialize the lookup tables
#ifdef __HLS_SYN__
    typename CONFIG_T::exp_table_t exp_table[CONFIG_T::table_size] = {
#include <exp_table.h>
    };
    typename CONFIG_T::inv_table_t invert_table[CONFIG_T::table_size] = {
#include <invert_table.h>
    };
#else
    static typename CONFIG_T::exp_table_t exp_table[CONFIG_T::table_size] = {
#include <exp_table.h>
    };
    static typename CONFIG_T::inv_table_t invert_table[CONFIG_T::table_size] = {
#include <invert_table.h>
    };
#endif

    // TODO: Init tables

    // Calculate all the e^x's
    typename CONFIG_T::exp_table_t exp_res[CONFIG_T::n_in];
    typename CONFIG_T::exp_table_t exp_sum(0);
    SoftmaxLatencyExp: for(unsigned i = 0; i < CONFIG_T::n_in; i++){
    	unsigned x = softmax_idx_from_real_val<data_T, CONFIG_T>(data[i]);
        exp_res[i] = exp_table[x];
    }

    // Explicitly sum the results with an adder tree.
    // Rounding & Saturation mode, which improve accuracy, prevent Vivado from expression balancing
    Op_add<typename CONFIG_T::exp_table_t> op_add;
    exp_sum = reduce<typename CONFIG_T::exp_table_t, CONFIG_T::n_in, Op_add<typename CONFIG_T::exp_table_t>>(exp_res, op_add);

    typename CONFIG_T::inv_table_t inv_exp_sum = invert_table[softmax_idx_from_real_val<typename CONFIG_T::exp_table_t,CONFIG_T>(exp_sum)];
    SoftmaxLatencyCalc: for(unsigned i = 0; i < CONFIG_T::n_in; i++){
        res[i] = exp_res[i] * inv_exp_sum;
    }
}

template <class data_T, class res_T, typename CONFIG_T>
void softmax_stable(data_T data[CONFIG_T::n_in], res_T res[CONFIG_T::n_in]){
    // Initialize the lookup tables
#ifdef __HLS_SYN__
    typename CONFIG_T::exp_table_t exp_table[CONFIG_T::table_size] = {
#include <exp_table.h>
    };
    typename CONFIG_T::inv_table_t invert_table[CONFIG_T::table_size] = {
#include <invert_table.h>
    };
#else
    static typename CONFIG_T::exp_table_t exp_table[CONFIG_T::table_size] = {
#include <exp_table.h>
    };
    static typename CONFIG_T::inv_table_t invert_table[CONFIG_T::table_size] = {
#include <invert_table.h>
    };
#endif

    // TODO: Init tables

    // Find the max and compute all delta(x_i, x_max)
    Op_max<data_T> op_max;
    data_T x_max = reduce<data_T, CONFIG_T::n_in, Op_max<data_T>>(data, op_max);

    // For the diffs, use the same type as the input but force rounding and saturation
    ac_fixed<data_T::width, data_T::i_width,ac_o_mode::AC_SAT,ac_q_mode::AC_RND> d_xi_xmax[CONFIG_T::n_in];
    SoftmaxStableDiff: for(unsigned i = 0; i < CONFIG_T::n_in; i++){
        d_xi_xmax[i] = data[i] - x_max;
    }

    // Calculate all the e^x's
    typename CONFIG_T::exp_table_t exp_res[CONFIG_T::n_in];
    typename CONFIG_T::exp_table_t exp_sum(0);
    SoftmaxStableExp: for(unsigned i = 0; i < CONFIG_T::n_in; i++){
    	unsigned x = softmax_idx_from_real_val<data_T, CONFIG_T>(d_xi_xmax[i]);
        exp_res[i] = exp_table[x];
    }

    // Explicitly sum the results with an adder tree.
    // Rounding & Saturation mode, which improve accuracy, prevent Vivado from expression balancing
    Op_add<typename CONFIG_T::exp_table_t> op_add;
    exp_sum = reduce<typename CONFIG_T::exp_table_t, CONFIG_T::n_in, Op_add<typename CONFIG_T::exp_table_t>>(exp_res, op_add);

    typename CONFIG_T::inv_table_t inv_exp_sum = invert_table[softmax_idx_from_real_val<typename CONFIG_T::exp_table_t,CONFIG_T>(exp_sum)];
    SoftmaxStableCalc: for(unsigned i = 0; i < CONFIG_T::n_in; i++){
        res[i] = exp_res[i] * inv_exp_sum;
    }
}

template<class data_T, class res_T, typename CONFIG_T>
void  softmax_legacy(data_T data[CONFIG_T::n_in], res_T res[CONFIG_T::n_in])
{
    // Initialize the lookup table
#ifdef __HLS_SYN__
    typename CONFIG_T::exp_table_t exp_table[CONFIG_T::table_size] = {
#include <exp_table.h>
    };
    typename CONFIG_T::inv_table_t invert_table[CONFIG_T::table_size] = {
#include <invert_table.h>
    };
#else
    static typename CONFIG_T::exp_table_t exp_table[CONFIG_T::table_size] = {
#include <exp_table.h>
    };
    static typename CONFIG_T::inv_table_t invert_table[CONFIG_T::table_size] = {
#include <invert_table.h>
    };
#endif

    // TODO: Init tables

    if (CONFIG_T::io_type == io_parallel){
        // Note: This is going to be a resource hog to run with pipeline, but hey, whatever
    }

    // Index into the lookup table based on data for exponentials
    typename CONFIG_T::table_t exp_res[CONFIG_T::n_in];// different, independent, fixed point precision
    typename CONFIG_T::table_t exp_diff_res;// different, independent, fixed point precision
    data_T data_cache[CONFIG_T::n_in];
    int data_round;
    int index;
    SoftmaxLegacy1: for (int ii=0; ii<CONFIG_T::n_in; ii++) {
      data_cache[ii] = data[ii];
      exp_res[ii] = 0;
    }
    SoftmaxLegacy2: for (int ii=0; ii<CONFIG_T::n_in; ii++) {
      if (CONFIG_T::io_type == io_serial){
      }
      SoftmaxLegacy2Inner: for (int jj=0; jj<CONFIG_T::n_in; jj++) {
	if (ii==jj) exp_diff_res = 1;
	else {
	  data_round = ((data_cache[jj]-data_cache[ii])*CONFIG_T::table_size/16).to_int();
	  index = data_round + 8*CONFIG_T::table_size/16;
	  if (index < 0)   index = 0;
	  if (index > CONFIG_T::table_size-1) index = CONFIG_T::table_size-1;
	  exp_diff_res = exp_table[index];
	}
	exp_res[ii] += exp_diff_res;
      }
    }

    //Second loop to invert
    SoftmaxLegacy3: for (int ii=0; ii<CONFIG_T::n_in; ii++) {
      int exp_res_index = (exp_res[ii]*CONFIG_T::table_size/64).to_int();
      if (exp_res_index < 0)   exp_res_index = 0;
      if (exp_res_index > CONFIG_T::table_size-1) exp_res_index = CONFIG_T::table_size-1;
      //typename CONFIG_T::table_t exp_res_invert = invert_table[exp_res_index];
      res[ii] = (res_T) invert_table[exp_res_index];
    }

}

template<class data_T, class res_T, typename CONFIG_T>
void softmax(data_T data[CONFIG_T::n_in], res_T res[CONFIG_T::n_in]){
    switch(CONFIG_T::implementation){
    case softmax_implementation::latency:
        softmax_latency<data_T, res_T, CONFIG_T>(data, res);
        break;
    case softmax_implementation::stable:
        softmax_stable<data_T, res_T, CONFIG_T>(data, res);
        break;
    case softmax_implementation::legacy:
        softmax_legacy<data_T, res_T, CONFIG_T>(data, res);
        break;
    }    
}

// *************************************************
//       TanH Activation
// *************************************************
template<class src_T, class res_T>
typename std::enable_if<std::is_same<res_T, ac_int<res_T::width,
                                                   res_T::sign>>::value,
               void>::type tanh_cast(src_T value, res_T &dest)
{
  double tmp = value.to_double();
  int dest_tmp = (int) tmp;
  dest = dest_tmp;
}

template<class src_T, class res_T>
typename std::enable_if<!std::is_same<res_T, ac_int<res_T::width,
                                                    res_T::sign>>::value,
               void>::type tanh_cast(src_T value, res_T &dest)
{
  dest = (res_T) value;
}

template<class data_T, class res_T, typename CONFIG_T>
void  tanh(data_T data[CONFIG_T::n_in], res_T res[CONFIG_T::n_in])
{
    // Initialize the lookup table
#ifdef __HLS_SYN__
    typename CONFIG_T::table_t tanh_table[CONFIG_T::table_size] = {
#include <tanh_table.h>
    };
#else
    static typename CONFIG_T::table_t tanh_table[CONFIG_T::table_size] = {
#include <tanh_table.h>
    };
#endif
    
    // TODO: Init tables

    if (CONFIG_T::io_type == io_parallel){
    }

    // Index into the lookup table based on data
    int data_round;
    int index;
    Tanh: for (int ii=0; ii<CONFIG_T::n_in; ii++) {
        if (CONFIG_T::io_type == io_serial){
        }
        data_round = (data[ii]*CONFIG_T::table_size/8).to_int();
        index = data_round + 4*CONFIG_T::table_size/8;
        //std::cout << "Input: "  << data[ii] << " Round: " << data_round << " Index: " << index << std::endl;
        if (index < 0)   index = 0;
        if (index > CONFIG_T::table_size-1) index = CONFIG_T::table_size-1;

        tanh_cast<typename CONFIG_T::table_t, res_T>(tanh_table[index], res[ii]);
    }
}

// *************************************************
//       Hard sigmoid Activation
// *************************************************
template<class data_T, class res_T, typename CONFIG_T>
void  hard_sigmoid(data_T data[CONFIG_T::n_in], res_T res[CONFIG_T::n_in])
{
    if (CONFIG_T::io_type == io_parallel){
    }

    data_T datareg;
    data_T slope = (data_T) 0.2;
    data_T shift = (data_T) 0.5;
    HardSigmoid: for (int ii=0; ii<CONFIG_T::n_in; ii++) {
        if (CONFIG_T::io_type == io_serial){
        }
        datareg = slope * data[ii] + shift;
        if (datareg > 1) datareg = 1;
        else if (datareg < 0) datareg = 0;
        res[ii] = datareg;
    }
}

// *************************************************
//       Leaky RELU Activation
// *************************************************
template<class data_T, class res_T, typename CONFIG_T>
void  leaky_relu(data_T data[CONFIG_T::n_in], data_T alpha, res_T res[CONFIG_T::n_in])
{
    if (CONFIG_T::io_type == io_parallel){
    }

    data_T datareg;
    LeakyRelu: for (int ii=0; ii<CONFIG_T::n_in; ii++) {
        if (CONFIG_T::io_type == io_serial){
        }
        datareg = data[ii];
        if (datareg > 0) res[ii] = datareg;
        else res[ii] = alpha * datareg;
    }
}

// *************************************************
//       Thresholded RELU Activation
// *************************************************
template<class data_T, class res_T, typename CONFIG_T>
void  thresholded_relu(data_T data[CONFIG_T::n_in], data_T theta, res_T res[CONFIG_T::n_in])
{
    if (CONFIG_T::io_type == io_parallel){
    }

    data_T datareg;
    ThresholdedRelu: for (int ii=0; ii<CONFIG_T::n_in; ii++) {
        if (CONFIG_T::io_type == io_serial){
        }
        datareg = data[ii];
        if (datareg > theta) res[ii] = datareg;
        else res[ii] = 0;
    }
}

// *************************************************
//       Softplus Activation
// *************************************************
template<class data_T, class res_T, typename CONFIG_T>
void  softplus(data_T data[CONFIG_T::n_in], res_T res[CONFIG_T::n_in])
{
    // Initialize the lookup table
#ifdef __HLS_SYN__
    typename CONFIG_T::table_t softplus_table[CONFIG_T::table_size] = {
#include <softplus_table.h>
    };
#else
    static typename CONFIG_T::table_t softplus_table[CONFIG_T::table_size] = {
#include <softplus_table.h>
    };
#endif
    
    // TODO: Init tables

    if (CONFIG_T::io_type == io_parallel){
    }

    // Index into the lookup table based on data
    int data_round;
    int index;
    Softplus: for (int ii=0; ii<CONFIG_T::n_in; ii++) {
        if (CONFIG_T::io_type == io_serial){
        }
        data_round = (data[ii]*CONFIG_T::table_size/16).to_int();
        index = data_round + 8*CONFIG_T::table_size/16;
        if (index < 0)   index = 0;
        if (index > CONFIG_T::table_size-1) index = CONFIG_T::table_size-1;
        res[ii] = (res_T) softplus_table[index];
    }
}

// *************************************************
//       Softsign Activation
// *************************************************
template<class data_T, class res_T, typename CONFIG_T>
void  softsign(data_T data[CONFIG_T::n_in], res_T res[CONFIG_T::n_in])
{
    // Initialize the lookup table
#ifdef __HLS_SYN__
    typename CONFIG_T::table_t softsign_table[CONFIG_T::table_size] = {
#include <softsign_table.h>
    };
#else
    static typename CONFIG_T::table_t softsign_table[CONFIG_T::table_size] = {
#include <softsign_table.h>
    };
#endif
    
    // TODO: Init tables

    if (CONFIG_T::io_type == io_parallel){
    }

    // Index into the lookup table based on data
    int data_round;
    int index;
    Softsign: for (int ii=0; ii<CONFIG_T::n_in; ii++) {
        if (CONFIG_T::io_type == io_serial){
        }
        data_round = (data[ii]*CONFIG_T::table_size/16).to_int();
        index = data_round + 8*CONFIG_T::table_size/16;
        if (index < 0)   index = 0;
        if (index > CONFIG_T::table_size-1) index = CONFIG_T::table_size-1;
        res[ii] = (res_T) softsign_table[index];
    }
}

// *************************************************
//       ELU Activation
// *************************************************
template<class data_T, class res_T, typename CONFIG_T>
void  elu(data_T data[CONFIG_T::n_in], const res_T alpha, res_T res[CONFIG_T::n_in])
{
    // Initialize the lookup table
#ifdef __HLS_SYN__
    typename CONFIG_T::table_t elu_table[CONFIG_T::table_size] = {
#include <elu_table.h>
    };
#else
    static typename CONFIG_T::table_t elu_table[CONFIG_T::table_size] = {
#include <elu_table.h>
    };
#endif
    
    // TODO: Init tables

    if (CONFIG_T::io_type == io_parallel){
    }

    data_T datareg;
    // Index into the lookup table based on data
    int index;
    Elu: for (int ii=0; ii<CONFIG_T::n_in; ii++) {
        if (CONFIG_T::io_type == io_serial){
        }
        datareg = data[ii];
        if (datareg >= 0) {
            res[ii] = datareg;
        } else {
            index = (datareg*CONFIG_T::table_size/-8).to_int();
            if (index > CONFIG_T::table_size-1) index = CONFIG_T::table_size-1;
            res[ii] = alpha * elu_table[index];
        }
    }
}

template<class data_T, class res_T, typename CONFIG_T>
void  elu(data_T data[CONFIG_T::n_in], res_T res[CONFIG_T::n_in])
{
	elu<data_T, res_T, CONFIG_T>(data, 1.0, res);
}

// *************************************************
//       SELU Activation
// *************************************************
template<class data_T, class res_T, typename CONFIG_T>
void  selu(data_T data[CONFIG_T::n_in], res_T res[CONFIG_T::n_in])
{
    // Initialize the lookup table
#ifdef __HLS_SYN__
    typename CONFIG_T::table_t selu_table[CONFIG_T::table_size] = {
#include <selu_table.h>
    };
#else
    static typename CONFIG_T::table_t selu_table[CONFIG_T::table_size] = {
#include <selu_table.h>
    };
#endif
    
    // TODO: Init tables

    if (CONFIG_T::io_type == io_parallel){
    }

    data_T datareg;
    // Index into the lookup table based on data
    int index;
    Selu: for (int ii=0; ii<CONFIG_T::n_in; ii++) {
        if (CONFIG_T::io_type == io_serial){
        }
        datareg = data[ii];
        if (datareg >= 0) {
            res[ii] = res_T(1.0507009873554804934193349852946) * datareg;
        } else {
            index = (datareg*CONFIG_T::table_size/-8).to_int();
            if (index > CONFIG_T::table_size-1) index = CONFIG_T::table_size-1;
            res[ii] = selu_table[index];
        }
    }
}

// *************************************************
//       PReLU Activation
// *************************************************
template<class data_T, class res_T, typename CONFIG_T>
void  prelu(data_T data[CONFIG_T::n_in], data_T alpha[CONFIG_T::n_in], res_T res[CONFIG_T::n_in])
{
    if (CONFIG_T::io_type == io_parallel){
    }

    data_T datareg;
    Prelu: for (int ii=0; ii<CONFIG_T::n_in; ii++) {
        if (CONFIG_T::io_type == io_serial){
        }
        datareg = data[ii];
        if (datareg > 0) res[ii] = datareg;
        else res[ii] = alpha[ii] * datareg;
    }
}

// *************************************************
//       Binary TanH Activation
// *************************************************
template<class data_T, class res_T, typename CONFIG_T>
void  binary_tanh(data_T data[CONFIG_T::n_in], res_T res[CONFIG_T::n_in])
{

 if (CONFIG_T::io_type == io_parallel){
 }
  
 data_T datareg;   
 res_T cache; 
 BinaryTanh: for (int ii=0; ii<CONFIG_T::n_in; ii++) {

  if (CONFIG_T::io_type == io_serial){
  }
  datareg = data[ii];	 
  if( datareg > 0 ) cache = 1;
  else cache = -1;
  
  res[ii] = (res_T) cache;
 
 }
 
}

// *************************************************
//       Ternary TanH Activation
// *************************************************
template<class data_T, class res_T, typename CONFIG_T>
void  ternary_tanh(data_T data[CONFIG_T::n_in], res_T res[CONFIG_T::n_in])
{

 if (CONFIG_T::io_type == io_parallel){
 }
  
 data_T datareg;   
 res_T cache; 
 TernaryTanh: for (int ii=0; ii<CONFIG_T::n_in; ii++) {

  if (CONFIG_T::io_type == io_serial){
  }
  datareg = 2*data[ii];	 
  if( datareg > 1 ) cache = 1;
  else if( datareg > -1 && datareg <= 1) cache=0;
  else cache = -1;
  
  res[ii] = (res_T) cache;
 
 }
 
}

}

#endif
