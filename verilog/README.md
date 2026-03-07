# CNN Verilog Inference (Q8.8)

This folder contains synthesizable Verilog modules matching the trained Keras model architecture found in `model_architecture.json`:

- Input: `(187, 1)`
- Conv1D: 16 filters, kernel 3, stride 1 + ReLU + MaxPool(2)
- Conv1D: 32 filters, kernel 3, stride 1 + ReLU + MaxPool(2)
- Flatten (45*32=1440)
- Dense 64 + ReLU
- Dense 1 + Sigmoid

## 1) Export real trained weights

Run:

```bash
python scripts/export_cnn_weights_q8_8.py \
  --model ecg_model.h5 \
  --sample sample.npy \
  --outdir verilog/weights
```

Generated memory files:

- `conv1_w.mem`, `conv1_b.mem`
- `conv2_w.mem`, `conv2_b.mem`
- `dense1_w.mem`, `dense1_b.mem`
- `dense2_w.mem`, `dense2_b.mem`
- `sample_input.mem` (if sample exists)

All values are two's-complement Q8.8 (`16-bit`).

## 2) Vivado simulation

In Vivado Tcl console:

```tcl
read_verilog verilog/relu.v
read_verilog verilog/maxpool.v
read_verilog verilog/conv1d_layer1.v
read_verilog verilog/conv1d_layer2.v
read_verilog verilog/flatten.v
read_verilog verilog/dense_layer.v
read_verilog verilog/sigmoid.v
read_verilog verilog/cnn_top.v
read_verilog verilog/cnn_tb.v

set_property top cnn_tb [current_fileset]
launch_simulation
run 1 us
```

`cnn_tb.v` drives clock/reset/start, loads `sample_input.mem`, and monitors output probability.
