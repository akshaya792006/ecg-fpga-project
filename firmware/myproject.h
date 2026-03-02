#ifndef MYPROJECT_H_
#define MYPROJECT_H_

#include <ap_axi_sdata.h>
#include <hls_math.h>
#include <hls_stream.h>

#include "parameters.h"

typedef ap_axiu<16, 1, 1, 1> axis_pkt_t;
typedef hls::stream<axis_pkt_t> axis_stream_t;

void myproject(axis_stream_t &in_stream, axis_stream_t &out_stream, int num_samples);

#endif
