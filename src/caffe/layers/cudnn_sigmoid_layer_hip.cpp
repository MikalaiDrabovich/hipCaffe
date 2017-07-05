#ifdef USE_ACCMI
#include <vector>

#include "caffe/layers/cudnn_sigmoid_layer.hpp"

namespace caffe {

template <typename Dtype>
void CuDNNSigmoidLayer<Dtype>::Forward_gpu(const vector<Blob<Dtype>*>& bottom,
    const vector<Blob<Dtype>*>& top) {
  const Dtype* bottom_data = bottom[0]->gpu_data();
  Dtype* top_data = top[0]->mutable_gpu_data();
#ifdef USE_MIOPEN
  MIOPEN_CHECK(miopenActivationForward(
      this->handle_,                // handle
      activ_desc_,                  // activDesc
      miopen::dataType<Dtype>::one, // *alpha
      this->bottom_desc_,           // xDesc
      bottom_data,                  // *x
      miopen::dataType<Dtype>::zero,// *beta
      this->top_desc_,              // yDesc
      top_data                      // *y
  ));
#endif

#ifdef USE_CUDNN
#if CUDNN_VERSION_MIN(5, 0, 0)
  CUDNN_CHECK(cudnnActivationForward(this->handle_,
        activ_desc_,
        cudnn::dataType<Dtype>::one,
        this->bottom_desc_, bottom_data,
        cudnn::dataType<Dtype>::zero,
        this->top_desc_, top_data));
#else
  CUDNN_CHECK(cudnnActivationForward_v4(this->handle_,
        activ_desc_,
        cudnn::dataType<Dtype>::one,
        this->bottom_desc_, bottom_data,
        cudnn::dataType<Dtype>::zero,
        this->top_desc_, top_data));
#endif
#endif // #ifdef USE_CUDNN
}

template <typename Dtype>
void CuDNNSigmoidLayer<Dtype>::Backward_gpu(const vector<Blob<Dtype>*>& top,
    const vector<bool>& propagate_down,
    const vector<Blob<Dtype>*>& bottom) {
  if (!propagate_down[0]) {
    return;
  }

  const Dtype* top_data = top[0]->gpu_data();
  const Dtype* top_diff = top[0]->gpu_diff();
  const Dtype* bottom_data = bottom[0]->gpu_data();
  Dtype* bottom_diff = bottom[0]->mutable_gpu_diff();
#ifdef USE_MIOPEN
  MIOPEN_CHECK(miopenActivationBackward(
      this->handle_,                 // handle
      activ_desc_,                   // activDesc
      miopen::dataType<Dtype>::one,  // *alpha
      this->top_desc_,               // yDesc
      top_data,                      // *y
      this->top_desc_,               // dyDesc
      top_diff,                      // *dy
      this->bottom_desc_,            // xDesc
      bottom_data,                   // *x
      miopen::dataType<Dtype>::zero, // *beta
      this->bottom_desc_,            // dxDesc
      bottom_diff                    // *dx
  ));
#endif

#ifdef USE_CUDNN
#if CUDNN_VERSION_MIN(5, 0, 0)
  CUDNN_CHECK(cudnnActivationBackward(this->handle_,
        activ_desc_,
        cudnn::dataType<Dtype>::one,
        this->top_desc_, top_data, this->top_desc_, top_diff,
        this->bottom_desc_, bottom_data,
        cudnn::dataType<Dtype>::zero,
        this->bottom_desc_, bottom_diff));
#else
  CUDNN_CHECK(cudnnActivationBackward_v4(this->handle_,
        activ_desc_,
        cudnn::dataType<Dtype>::one,
        this->top_desc_, top_data, this->top_desc_, top_diff,
        this->bottom_desc_, bottom_data,
        cudnn::dataType<Dtype>::zero,
        this->bottom_desc_, bottom_diff));
#endif
#endif // #ifdef USE_CUDNN
}

INSTANTIATE_LAYER_GPU_FUNCS(CuDNNSigmoidLayer);

}  // namespace caffe
#endif
