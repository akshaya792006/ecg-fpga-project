#include "myproject.h"

#include "weights/conv1_weights.h"
#include "weights/conv2_weights.h"
#include "weights/dense1_weights.h"
#include "weights/dense2_weights.h"

static data_t relu(data_t x) { return (x > 0) ? x : (data_t)0; }

static data_t sigmoid_lut(data_t x) {
#pragma HLS INLINE
    float xf = (float)x;
    float y = 1.0f / (1.0f + hls::expf(-xf));
    return (data_t)y;
}

void myproject(axis_stream_t &in_stream, axis_stream_t &out_stream, int num_samples) {
#pragma HLS INTERFACE axis port = in_stream
#pragma HLS INTERFACE axis port = out_stream
#pragma HLS INTERFACE s_axilite port = num_samples bundle = CTRL
#pragma HLS INTERFACE s_axilite port = return bundle = CTRL

    data_t input[INPUT_LEN];
    data_t conv1_out[CONV1_OUT_LEN][CONV1_OUT_CH];
    data_t pool1_out[POOL1_OUT_LEN][CONV1_OUT_CH];
    data_t conv2_out[CONV2_OUT_LEN][CONV2_OUT_CH];
    data_t pool2_out[POOL2_OUT_LEN][CONV2_OUT_CH];
    data_t flatten[FLATTEN_LEN];
    data_t dense1_out[DENSE1_UNITS];

#pragma HLS ARRAY_PARTITION variable = conv1_out complete dim = 2
#pragma HLS ARRAY_PARTITION variable = pool1_out complete dim = 2
#pragma HLS ARRAY_PARTITION variable = conv2_out complete dim = 2
#pragma HLS ARRAY_PARTITION variable = pool2_out complete dim = 2
#pragma HLS ARRAY_PARTITION variable = dense1_out complete dim = 1

sample_loop:
    for (int sample = 0; sample < num_samples; sample++) {
        // AXI-Stream input: 187 samples per ECG beat.
    read_input:
        for (int i = 0; i < INPUT_LEN; i++) {
#pragma HLS PIPELINE II = 1
            axis_pkt_t pkt = in_stream.read();
            input[i] = (data_t)pkt.data;
        }

        // Conv1D #1
    conv1_t:
        for (int t = 0; t < CONV1_OUT_LEN; t++) {
        conv1_f:
            for (int f = 0; f < CONV1_OUT_CH; f++) {
#pragma HLS PIPELINE II = 1
                acc_t acc = conv1_b[f];
            conv1_k:
                for (int k = 0; k < CONV1_KERNEL; k++) {
                    acc += input[t + k] * conv1_w[k][0][f];
                }
                conv1_out[t][f] = relu((data_t)acc);
            }
        }

        // MaxPool1D #1
    pool1_t:
        for (int t = 0; t < POOL1_OUT_LEN; t++) {
        pool1_f:
            for (int f = 0; f < CONV1_OUT_CH; f++) {
#pragma HLS PIPELINE II = 1
                data_t a = conv1_out[t * 2][f];
                data_t b = conv1_out[t * 2 + 1][f];
                pool1_out[t][f] = (a > b) ? a : b;
            }
        }

        // Conv1D #2
    conv2_t:
        for (int t = 0; t < CONV2_OUT_LEN; t++) {
        conv2_f:
            for (int f = 0; f < CONV2_OUT_CH; f++) {
#pragma HLS PIPELINE II = 1
                acc_t acc = conv2_b[f];
            conv2_k:
                for (int k = 0; k < CONV2_KERNEL; k++) {
                conv2_c:
                    for (int c = 0; c < CONV2_IN_CH; c++) {
                        acc += pool1_out[t + k][c] * conv2_w[k][c][f];
                    }
                }
                conv2_out[t][f] = relu((data_t)acc);
            }
        }

        // MaxPool1D #2
    pool2_t:
        for (int t = 0; t < POOL2_OUT_LEN; t++) {
        pool2_f:
            for (int f = 0; f < CONV2_OUT_CH; f++) {
#pragma HLS PIPELINE II = 1
                data_t a = conv2_out[t * 2][f];
                data_t b = conv2_out[t * 2 + 1][f];
                pool2_out[t][f] = (a > b) ? a : b;
            }
        }

        // Flatten
        int idx = 0;
    flat_t:
        for (int t = 0; t < POOL2_OUT_LEN; t++) {
        flat_c:
            for (int c = 0; c < CONV2_OUT_CH; c++) {
#pragma HLS PIPELINE II = 1
                flatten[idx++] = pool2_out[t][c];
            }
        }

        // Dense #1 (ReLU)
    dense1_u:
        for (int u = 0; u < DENSE1_UNITS; u++) {
#pragma HLS PIPELINE II = 1
            acc_t acc = dense1_b[u];
        dense1_i:
            for (int i = 0; i < FLATTEN_LEN; i++) {
                acc += flatten[i] * dense1_w[i][u];
            }
            dense1_out[u] = relu((data_t)acc);
        }

        // Dense #2 (sigmoid)
        acc_t out_acc = dense2_b[0];
    dense2_i:
        for (int i = 0; i < DENSE1_UNITS; i++) {
#pragma HLS PIPELINE II = 1
            out_acc += dense1_out[i] * dense2_w[i][0];
        }
        data_t prob = sigmoid_lut((data_t)out_acc);

        axis_pkt_t out_pkt;
        out_pkt.data = prob.range(15, 0);
        out_pkt.keep = -1;
        out_pkt.strb = -1;
        out_pkt.user = 0;
        out_pkt.id = 0;
        out_pkt.dest = 0;
        out_pkt.last = 1;
        out_stream.write(out_pkt);
    }
}
