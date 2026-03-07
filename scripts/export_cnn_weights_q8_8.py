#!/usr/bin/env python3
"""Export trained Keras CNN weights to Q8.8 hex memory files for Verilog ROM loading."""

from __future__ import annotations

import argparse
from pathlib import Path


FRAC_BITS = 8
TOTAL_BITS = 16
MAX_VAL = (1 << (TOTAL_BITS - 1)) - 1
MIN_VAL = -(1 << (TOTAL_BITS - 1))


def q88_to_hex(v: float) -> str:
    scaled = int(round(v * (1 << FRAC_BITS)))
    if scaled > MAX_VAL:
        scaled = MAX_VAL
    if scaled < MIN_VAL:
        scaled = MIN_VAL
    if scaled < 0:
        scaled = (1 << TOTAL_BITS) + scaled
    return f"{scaled:04x}"


def write_hex(values, path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8") as f:
        for v in values:
            f.write(f"{v}\n")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--model", default="ecg_model.h5", help="Trained Keras model file")
    parser.add_argument("--sample", default="sample.npy", help="Optional sample ECG file")
    parser.add_argument("--outdir", default="verilog/weights", help="Output directory for .mem files")
    args = parser.parse_args()

    try:
        import numpy as np
        import tensorflow as tf
    except ImportError as exc:
        raise SystemExit(
            "Missing dependency. Install tensorflow and numpy to export trained weights."
        ) from exc

    model_path = Path(args.model)
    if not model_path.exists():
        raise SystemExit(f"Missing trained model file: {model_path}")

    model = tf.keras.models.load_model(model_path)
    layer_names = [l.name for l in model.layers]

    required = ["conv1d", "conv1d_1", "dense", "dense_1"]
    missing = [name for name in required if name not in layer_names]
    if missing:
        raise SystemExit(f"Model missing required layers: {missing}")

    outdir = Path(args.outdir)

    conv1_w, conv1_b = model.get_layer("conv1d").get_weights()  # [k, in, out], [out]
    conv2_w, conv2_b = model.get_layer("conv1d_1").get_weights()
    dense1_w, dense1_b = model.get_layer("dense").get_weights()  # [in, out], [out]
    dense2_w, dense2_b = model.get_layer("dense_1").get_weights()

    write_hex((q88_to_hex(x) for x in conv1_w.flatten(order="C")), outdir / "conv1_w.mem")
    write_hex((q88_to_hex(x) for x in conv1_b.flatten(order="C")), outdir / "conv1_b.mem")

    write_hex((q88_to_hex(x) for x in conv2_w.flatten(order="C")), outdir / "conv2_w.mem")
    write_hex((q88_to_hex(x) for x in conv2_b.flatten(order="C")), outdir / "conv2_b.mem")

    write_hex((q88_to_hex(x) for x in dense1_w.flatten(order="C")), outdir / "dense1_w.mem")
    write_hex((q88_to_hex(x) for x in dense1_b.flatten(order="C")), outdir / "dense1_b.mem")

    write_hex((q88_to_hex(x) for x in dense2_w.flatten(order="C")), outdir / "dense2_w.mem")
    write_hex((q88_to_hex(x) for x in dense2_b.flatten(order="C")), outdir / "dense2_b.mem")

    sample_path = Path(args.sample)
    if sample_path.exists():
        sample = np.load(sample_path)
        sample = sample.reshape(-1)
        write_hex((q88_to_hex(float(x)) for x in sample), outdir / "sample_input.mem")

    print("Export complete:", outdir)
    print("Layer names:", layer_names)
    print("conv1_w shape", conv1_w.shape, "conv2_w shape", conv2_w.shape)
    print("dense1_w shape", dense1_w.shape, "dense2_w shape", dense2_w.shape)


if __name__ == "__main__":
    main()
