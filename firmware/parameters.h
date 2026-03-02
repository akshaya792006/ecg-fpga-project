#ifndef PARAMETERS_H_
#define PARAMETERS_H_

#include <ap_fixed.h>

// Fixed-point datatypes tuned for ECG dynamic range.
typedef ap_fixed<16, 6> data_t;
typedef ap_fixed<24, 8> acc_t;

static const int INPUT_LEN = 187;

static const int CONV1_KERNEL = 3;
static const int CONV1_IN_CH = 1;
static const int CONV1_OUT_CH = 16;
static const int CONV1_OUT_LEN = 185;

static const int POOL1_SIZE = 2;
static const int POOL1_STRIDE = 2;
static const int POOL1_OUT_LEN = 92;

static const int CONV2_KERNEL = 3;
static const int CONV2_IN_CH = 16;
static const int CONV2_OUT_CH = 32;
static const int CONV2_OUT_LEN = 90;

static const int POOL2_SIZE = 2;
static const int POOL2_STRIDE = 2;
static const int POOL2_OUT_LEN = 45;

static const int FLATTEN_LEN = POOL2_OUT_LEN * CONV2_OUT_CH; // 1440
static const int DENSE1_UNITS = 64;
static const int DENSE2_UNITS = 1;

#endif
