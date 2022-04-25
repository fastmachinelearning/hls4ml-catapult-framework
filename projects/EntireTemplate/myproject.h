#ifndef __ENTIRETEMPLATE__
#define __ENTIRETEMPLATE__

#include "all_headers.h"
#include "ac_int.h"
#include "ac_fixed.h"
#include "ac_channel.h"
#include <vector>

struct config : nnet::dense_config {
  static const unsigned n_in = 10;
  static const unsigned n_out = 2;
  static const unsigned io_type = nnet::io_parallel;
  static const unsigned strategy = nnet::latency;
  static const unsigned reuse_factor = 1;
  static const unsigned n_zeros = 0;
  static const unsigned n_nonzeros = 80;
  static const bool store_weights_in_bram = false;
  typedef ac_fixed<16,6, true> accum_t;
  typedef ac_fixed<11,3,true> bias_t;
  typedef ac_fixed<8,2,true> weight_t;
  typedef ac_int<1, false> index_t;
  template<class x_T, class y_T>
  using product = nnet::product::mult<x_T, y_T>;
};

struct config_cast {
  static const unsigned n_in = 12;
  typedef ac_int<1, false> weight_t;
  typedef ac_int<5, true> accum_t;
};

struct config_activ : nnet::activ_config {
  static const unsigned n_in = 10;
  static const unsigned table_size = 1024;
  static const unsigned io_type = nnet::io_parallel;
  static const unsigned reuse_factor = 1;
  static const nnet::softmax_implementation implementation = nnet::softmax_implementation::latency;
  typedef ac_fixed<18,8, true> exp_table_t;
  typedef ac_fixed<18,8, true> inv_table_t;
  typedef ac_fixed<10,4,true> table_t;
};

struct config_batchnorm : nnet::batchnorm_config {
  static const unsigned n_in = 15;
  static const unsigned n_filt = 3;
  static const unsigned io_type = nnet::io_serial;
  static const unsigned reuse_factor = 15;
  static const bool store_weights_in_bram = false;
  typedef ac_fixed<5,2,true> bias_t;
  typedef ac_fixed<10,7,true> scale_t;
  template<class x_T, class y_T>
  using product = nnet::product::mult<x_T, y_T>;
};

struct config_merge : nnet::merge_config {
  static const unsigned n_elem = 10;
};

struct config_dot : nnet::dot_config {
  static const unsigned n_in = 10;
  static const unsigned n_out = 10;
  static const unsigned reuse_factor = 3;
  typedef ac_fixed<25,12,true> accum_t;
  template<class x_T, class y_T>
  using product = nnet::product::mult<x_T, y_T>;
};

struct config_concat : nnet::concat_config {
  static const unsigned n_elem1_0 = 15;
  static const unsigned n_elem1_1 = 5;
  static const unsigned n_elem1_2 = 1;
  static const unsigned n_elem2_0 = 20;
  static const unsigned n_elem2_1 = 19;
  static const unsigned n_elem2_2 = 2;

  static const unsigned axis = 0;
};

struct config_transpose {
  static const unsigned height = 10;
  static const unsigned width = 10;
  static const unsigned depth = 10;
  static const unsigned perm[3];
};

typedef ac_fixed<10,5,true> ac_t;
typedef ac_fixed<9,3,true> ac_t_2;
typedef ac_int<5, true> ac_t_i;
typedef nnet::array<ac_t, 2> arr;

// CNNs and GNNs skipped for now
namespace nnet {
  // nnet_activation.h
  template void linear<ac_t, ac_t, config_activ>(ac_t[], ac_t[]);
  template void relu<ac_t, ac_t, config_activ>(ac_t[], ac_t[]);
  template void relu_max<ac_t, ac_t, 3, config_activ>(ac_t[], ac_t[]);
  template void relu6<ac_t, ac_t, config_activ>(ac_t[], ac_t[]);
  template void relu1<ac_t, ac_t, config_activ>(ac_t[], ac_t[]);
  template void sigmoid<ac_t, ac_t, config_activ>(ac_t[], ac_t[]);
  template unsigned softmax_idx_from_real_val<ac_t, config_activ>(ac_t);
  template void softmax_latency<ac_t, ac_t, config_activ>(ac_t[], ac_t[]);
  template void softmax_stable<ac_t, ac_t, config_activ>(ac_t[], ac_t[]);
  template void softmax_legacy<ac_t, ac_t, config_activ>(ac_t[], ac_t[]);
  template void softmax<ac_t, ac_t, config_activ>(ac_t[], ac_t[]);
  template void tanh<ac_t, ac_t, config_activ>(ac_t[], ac_t[]);
  template void hard_sigmoid<ac_t, ac_t, config_activ>(ac_t[], ac_t[]);
  template void leaky_relu<ac_t, ac_t, config_activ>(ac_t[], ac_t, ac_t[]);
  template void thresholded_relu<ac_t, ac_t, config_activ>(ac_t[], ac_t, ac_t[]);
  template void softplus<ac_t, ac_t, config_activ>(ac_t[], ac_t[]);
  template void softsign<ac_t, ac_t, config_activ>(ac_t[], ac_t[]);
  template void elu<ac_t, ac_t, config_activ>(ac_t[], const ac_t, ac_t[]);
  template void elu<ac_t, ac_t, config_activ>(ac_t[], ac_t[]);
  template void selu<ac_t, ac_t, config_activ>(ac_t[], ac_t[]);
  template void prelu<ac_t, ac_t, config_activ>(ac_t[], ac_t[], ac_t[]);
  template void binary_tanh<ac_t, ac_t, config_activ>(ac_t[], ac_t[]);
  template void ternary_tanh<ac_t, ac_t, config_activ>(ac_t[], ac_t[]);

  // nnet_activation_stream.h
  template void linear<arr, arr, config_activ>(ac_channel<arr> &, ac_channel<arr> &);
  template void relu<arr, arr, config_activ>(ac_channel<arr> &, ac_channel<arr> &);
  template void sigmoid<arr, arr, config_activ>(ac_channel<arr> &, ac_channel<arr> &);
  template void softmax_latency<arr, arr, config_activ>(ac_channel<arr> &, ac_channel<arr> &);
  template void softmax_stable<arr, arr, config_activ>(ac_channel<arr> &, ac_channel<arr> &);
  template void softmax_legacy<arr, arr, config_activ>(ac_channel<arr> &, ac_channel<arr> &);
  template void softmax<arr, arr, config_activ>(ac_channel<arr> &, ac_channel<arr> &);
  template void tanh<arr, arr, config_activ>(ac_channel<arr> &, ac_channel<arr> &);
  template void hard_sigmoid<arr, arr, config_activ>(ac_channel<arr> &, ac_channel<arr> &);
  template void leaky_relu<arr, arr, config_activ>(ac_channel<arr> &, ac_t, ac_channel<arr> &);
  template void thresholded_relu<arr, arr, config_activ>(ac_channel<arr> &, ac_t, ac_channel<arr> &);
  template void softplus<arr, arr, config_activ>(ac_channel<arr> &, ac_channel<arr> &);
  template void softsign<arr, arr, config_activ>(ac_channel<arr> &, ac_channel<arr> &);
  template void elu<arr, arr, config_activ>(ac_channel<arr> &, const ac_t, ac_channel<arr> &);
  template void elu<arr, arr, config_activ>(ac_channel<arr> &, ac_channel<arr> &);
  template void selu<arr, arr, config_activ>(ac_channel<arr> &, ac_channel<arr> &);
  template void prelu<arr, arr, config_activ>(ac_channel<arr> &, ac_t[], ac_channel<arr> &);

  // nnet_array.h
  template void transpose_3d<ac_t, config_transpose>(ac_t[], ac_t[]);

  // nnet_batchnorm.h
  template void normalize<ac_t, ac_t, config_batchnorm>(ac_t[], ac_t[],
                                                        config_batchnorm::scale_t[],
                                                        config_batchnorm::bias_t[]);
  template void normalize_binary_tanh<ac_t, config_batchnorm>(ac_t[], ac_int<1, false>[], ac_t[]);
  template void normalize_ternary_tanh<ac_t, config_batchnorm>(ac_t[], ac_int<2, true>[], ac_t[], ac_t[]);

  // nnet_batchnorm_stream.h
  template void normalize<arr, arr, config_batchnorm>(ac_channel<arr> &, ac_channel<arr> &,
                                                      config_batchnorm::scale_t[],
                                                      config_batchnorm::bias_t[]);
  template void normalize_binary_tanh<arr, config_batchnorm>(ac_channel<arr> &,
                                                             ac_channel<array<ac_int<1, false>, 15> > &,
                                                             ac_t[]);
  template void normalize_ternary_tanh<arr, config_batchnorm>(ac_channel<arr> &,
                                                              ac_channel<array<ac_int<2, true>, 15> > &,
                                                              ac_t[], ac_t[]);

  // nnet_common.h
  template ac_t_i reduce<ac_t_i, 5, Op_add<ac_t_i> >(const ac_t_i*, Op_add<ac_t_i>);
  template ac_t_i reduce<ac_t_i, 2, Op_add<ac_t_i> >(const ac_t_i*, Op_add<ac_t_i>);
  template ac_t_i reduce<ac_t_i, 1, Op_add<ac_t_i> >(const ac_t_i*, Op_add<ac_t_i>);
  template class Op_add<ac_t>;
  template class Op_and<bool>;
  template class Op_or<bool>;
  template class Op_max<ac_t>;
  template class Op_min<ac_t>;

  // nnet_dense.h
  template void dense<ac_t, ac_t, config>(ac_t[], ac_t[], config::weight_t[], config::bias_t[]);

  // nnet_dense_latency.h
  template void dense_latency<ac_t, ac_t, config>(ac_t[], ac_t[], config::weight_t[], config::bias_t[]);

  // nnet_dense_compressed.h
  template void fill_mult<config>(config::index_t, config::accum_t[], config::accum_t);
  //template void dense_compressed<ac_t, ac_t, config>(ac_t[], ac_t[], config::weight_t[], config::bias_t[]);

  // nnet_dense_resource.h
  template void dense_resource_rf_leq_nin<ac_t, ac_t, config>(ac_t[], ac_t[], config::weight_t[],
                                                              config::bias_t[]);
  template void dense_resource_rf_gt_nin_rem0<ac_t, ac_t, config>(ac_t[], ac_t[], config::weight_t[],
                                                                  config::bias_t[]);
  template void dense_resource_rf_gt_nin<ac_t, ac_t, config>(ac_t[], ac_t[], config::weight_t[],
                                                             config::bias_t[]);
  template void dense_resource<ac_t, ac_t, config>(ac_t[], ac_t[], config::weight_t[], config::bias_t[]);

  // nnet_dense_stream.h
  template void dense_wrapper<ac_t, ac_t, config>(ac_t[], ac_t[], config::weight_t[], config::bias_t[]);
  template void dense<arr, arr, config>(ac_channel<arr> &, ac_channel<arr> &, config::weight_t[],
                                        config::bias_t[]);

  // nnet_helpers.h
  template void copy_data<ac_t, ac_t, 5, 5>(std::vector<ac_t>, ac_t[]);
  template void copy_data<ac_t, arr, 5, 5>(std::vector<ac_t>, ac_channel<arr> &);
  template void fill_zero<ac_t, 5>(ac_t[]);
  template void fill_zero<arr, 5>(ac_channel<arr> &);
  template void change_type<ac_t, ac_t, 5>(ac_channel<ac_t> &, ac_channel<ac_t> &);
  template void hls_stream_debug<ac_t, 5>(ac_channel<ac_t> &, ac_channel<ac_t> &);

  // nnet_merge.h
  template void add<ac_t, ac_t, ac_t, config_merge>(ac_t[], ac_t[], ac_t[]);
  template void subtract<ac_t, ac_t, ac_t, config_merge>(ac_t[], ac_t[], ac_t[]);
  template void multiply<ac_t, ac_t, ac_t, config_merge>(ac_t[], ac_t[], ac_t[]);
  template void average<ac_t, ac_t, ac_t, config_merge>(ac_t[], ac_t[], ac_t[]);
  template void maximum<ac_t, ac_t, ac_t, config_merge>(ac_t[], ac_t[], ac_t[]);
  template void minimum<ac_t, ac_t, ac_t, config_merge>(ac_t[], ac_t[], ac_t[]);
  template void dot1d<ac_t, ac_t, ac_t, config_dot>(ac_t[], ac_t[], ac_t[]);
  template void concatenate1d<ac_t, ac_t, ac_t, config_concat>(ac_t[], ac_t[], ac_t[]);
  template void concatenate2d_0<ac_t, ac_t, ac_t, config_concat>(ac_t[], ac_t[], ac_t[]);
  template void concatenate2d_1<ac_t, ac_t, ac_t, config_concat>(ac_t[], ac_t[], ac_t[]);
  template void concatenate2d<ac_t, ac_t, ac_t, config_concat>(ac_t[], ac_t[], ac_t[]);
  template void concatenate3d_0<ac_t, ac_t, ac_t, config_concat>(ac_t[], ac_t[], ac_t[]);
  template void concatenate3d_1<ac_t, ac_t, ac_t, config_concat>(ac_t[], ac_t[], ac_t[]);
  template void concatenate3d_2<ac_t, ac_t, ac_t, config_concat>(ac_t[], ac_t[], ac_t[]);
  template void concatenate3d<ac_t, ac_t, ac_t, config_concat>(ac_t[], ac_t[], ac_t[]);

  // nnet_merge_stream.h
  template void add<arr, arr, arr, config_merge>(ac_channel<arr> &, ac_channel<arr> &, ac_channel<arr> &);
  template void subtract<arr, arr, arr, config_merge>(ac_channel<arr> &, ac_channel<arr> &, ac_channel<arr> &);
  template void multiply<arr, arr, arr, config_merge>(ac_channel<arr> &, ac_channel<arr> &, ac_channel<arr> &);
  template void average<arr, arr, arr, config_merge>(ac_channel<arr> &, ac_channel<arr> &, ac_channel<arr> &);
  template void maximum<arr, arr, arr, config_merge>(ac_channel<arr> &, ac_channel<arr> &, ac_channel<arr> &);
  template void minimum<arr, arr, arr, config_merge>(ac_channel<arr> &, ac_channel<arr> &, ac_channel<arr> &);

  // nnet_mult.h
  namespace product {
    template class both_binary<ac_t, ac_t>;
    template class weight_binary<ac_t, ac_t>;
    template class weight_ternary<ac_t, ac_t>;
    template class mult<ac_t, ac_t>;
    //template class weight_exponential<ac_t, ac_t, ac_t>;
  };

  template ac_int<ceillog2(config_cast::n_in) + 2, true> cast<ac_int<1, false>, ac_t, config_cast>(config_cast::accum_t);
  template ac_t_2 cast<ac_t, ac_t_2, config_cast>(config_cast::accum_t);

  // nnet_stream.h
  template void clone_stream<arr, arr, 5>(ac_channel<arr> &, ac_channel<arr> &, ac_channel<arr> &);
  template void repack_stream<arr, arr, 5>(ac_channel<arr> &, ac_channel<arr> &);

  // nnet_types.h
  template struct array<ac_t, 10>;
  template struct array<ac_t_i, 10>;
};

#endif
