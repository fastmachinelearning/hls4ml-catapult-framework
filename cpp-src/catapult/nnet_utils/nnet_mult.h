#ifndef NNET_MULT_H_
#define NNET_MULT_H_

#include "nnet_common.h"
#include "nnet_helpers.h"
#include <ac_channel.h>	
#include <ac_float.h>
#include <math.h>

namespace nnet {

namespace product{

/* ---
 * 5 different methods to perform the product of input and weight, depending on the
 * types of each. 
 * --- */

template<class x_T, class w_T, class y_T>
class Product{
    public:
    static y_T product(x_T a, w_T w){
        // 'Normal' product
        
        return a * w;
    }
    static void limit(unsigned multiplier_limit) {} // Nothing to do here
};

template<class x_T, class w_T, class y_T>
class both_binary : public Product<x_T, w_T, y_T>{
    public:
    static y_T product(x_T a, w_T w){
        // specialisation for 1-bit weights and incoming data
        
        return a == w;
    }
};

template<class x_T, class w_T, class y_T>
class weight_binary : public Product<x_T, w_T, y_T>{
    public:
    static y_T product(x_T a, w_T w){
        // Specialisation for 1-bit weights, arbitrary data
        
        return w == 0 ? (x_T) -a : a;
    }
};

template<class x_T, class w_T, class y_T>
class weight_ternary : public Product<x_T, w_T, y_T>{
    public:
    static y_T product(x_T a, w_T w){
        // Specialisation for 2-bit weights, arbitrary data
        
        if (w == 0) return (x_T) 0;
        else if(w == -1) return (x_T) -a;
        else return (x_T) a; // if(w == 1)
    }
};

template<class x_T, class w_T, class y_T>
class mult : public Product<x_T, w_T, y_T>{
    public:
    static y_T product(x_T a, w_T w){
        // 'Normal' product
        
        return a * w;
    }
    static void limit(unsigned multiplier_limit){
        
        
    }
};

template<class x_T, class w_T, class y_T>
class weight_exponential : public Product<x_T, w_T, y_T>{
    public:
    static y_T product(x_T a, w_T w){
        // Shift product for exponential weights
        
        // shift by the exponent. Negative weights shift right
        y_T ay = a;
        y_T y = ay << w.weight;
        // negate or not depending on weight sign
        return w.sign == 1 ? (y_T) y : (y_T) -y;
    }
};

} // namespace product_type

template<class data_T, class res_T, typename CONFIG_T>
inline typename std::enable_if<std::is_same<data_T, ac_int<1, false>>::value
        and std::is_same<typename CONFIG_T::weight_t, ac_int<1, false>>::value, ac_int<nnet::ceillog2(CONFIG_T::n_in) + 2, true>>::type
cast(typename CONFIG_T::accum_t x){
  return (ac_int<nnet::ceillog2(CONFIG_T::n_in) + 2, true>) (x - CONFIG_T::n_in / 2) * 2;
}

template<class data_T, class res_T, typename CONFIG_T>
inline typename std::enable_if<(not std::is_same<data_T, ac_int<1, false>>::value), res_T>::type
cast(typename CONFIG_T::accum_t x){
  return (res_T) x;
}

}

#endif
