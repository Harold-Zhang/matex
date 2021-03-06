#include "tensorflow/core/framework/op.h"
#include "tensorflow/core/framework/op_kernel.h"
#include "tensorflow/core/framework/shape_inference.h"
#include <stdio.h>
#include <mpi.h>
using namespace tensorflow;
#include <iostream>
//#include <thread>
//#include <mutex>

//std::mutex bmtx;

REGISTER_OP("TfBroadcast")
    .Attr("T: {float, int32}")
    .Input("to_broadcast1: Ref(T)")
    .Input("to_broadcast2: int32")
    .Output("broadcasted: Ref(T)")
    .SetShapeFn([](::tensorflow::shape_inference::InferenceContext* c){
       c->set_output(0, c->input(0));
    return Status::OK();
   });

class TfBroadcastFloatOp : public OpKernel{
   public:
      explicit TfBroadcastFloatOp(OpKernelConstruction *context) : OpKernel(context) {}
      void Compute(OpKernelContext *context) override{
         Tensor input_tensor = context->mutable_input(0, true);

         mutex *bmtx = context->input_ref_mutex(0);
         bmtx->lock();

         Tensor input_indexf = context->input(1);
         auto input_index = input_indexf.flat<int32>();
         auto input = input_tensor.flat<float>();
         //Tensor* output_tensor = NULL;
         context->forward_ref_input_to_ref_output(0, 0);
         //OP_REQUIRES_OK(context, context->allocate_output(0, input_tensor.shape(),
         //                                           &output_tensor));
         
         //auto output = output_tensor->flat<float>();
         const long N = input.size();
         //int rank;
         //MPI_Comm_rank(MPI_COMM_WORLD, &rank);
         //float *tmp_in = new float[N];
         //for(int i = 0; i < N; ++i){
         //    tmp_in[i] = input(i);
         //}
         //MPI_Bcast(tmp_in, N, MPI::FLOAT, 0, MPI_COMM_WORLD);
         MPI_Bcast(&(input(0)), N, MPI::FLOAT, 0, MPI_COMM_WORLD);
         MPI_Barrier(MPI_COMM_WORLD);
         //for(int i = 0; i < N; ++i){
         //   output(i) = tmp_in[i];
         //   input(i) = tmp_in[i];
         //}
         //delete [] tmp_in;
         bmtx->unlock();
      }
};

class TfBroadcastInt32Op : public OpKernel{
   public:
      explicit TfBroadcastInt32Op(OpKernelConstruction *context) : OpKernel(context) {}
      void Compute(OpKernelContext *context) override{
         Tensor input_tensor = context->mutable_input(0, true);
         mutex *bmtx = context->input_ref_mutex(0);
         bmtx->lock();
         Tensor input_indexf = context->input(1);
         auto input_index = input_indexf.flat<int32>();

         auto input = input_tensor.flat<int32>();
         context->forward_ref_input_to_ref_output(0,0);
         //Tensor* output_tensor = NULL;
         //OP_REQUIRES_OK(context, context->allocate_output(0, input_tensor.shape(),
         //                                            &output_tensor));

         //auto output = output_tensor->flat<int32>();
         const int N = input.size();
         //int rank;
         //MPI_Comm_size(MPI_COMM_WORLD, &rank);
         //int *tmp_in = new int32[N];

         //for(int i = 0; i < N; ++i){
         //    tmp_in[i] = input(i);
        // }
         MPI_Bcast(&(input(0)), N, MPI::INT, 0, MPI_COMM_WORLD);
         MPI_Barrier(MPI_COMM_WORLD);
         //for(int i = 0; i < N; ++i){
         //   output(i) = tmp_in[i];
         //   input(i) = tmp_in[i];
        // }
         //delete [] tmp_in;
         bmtx->unlock();
      }
};

REGISTER_KERNEL_BUILDER(Name("TfBroadcast").Device(DEVICE_CPU).TypeConstraint<int32>("T"), TfBroadcastInt32Op);
REGISTER_KERNEL_BUILDER(Name("TfBroadcast").Device(DEVICE_CPU).TypeConstraint<float>("T"), TfBroadcastFloatOp);

