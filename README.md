# ecg-fpga-project
ECG Arrhythmia Detection CNN model development on ZYNQ using Vitis hls
# ECG Arrhythmia Detection on FPGA (Zynq-7000)

This repository contains a trained Keras model (ecg_model.h5).

Goal:
Convert this model into a Vitis HLS compatible FPGA accelerator.

Target Board:
Zybo Z7-20 (Zynq-7000)

Requirements:
- Generate firmware folder:
    myproject.cpp
    myproject.h
    parameters.h
    weights/
- Use fixed-point (ap_fixed)
- 1D CNN inference
- Optimized for latency
- Compatible with Vitis HLS 2025.2

Input:
ECG signal shape (1,187,1)
Datatype: float32

Output:
0 = Normal
1 = Abnormal
