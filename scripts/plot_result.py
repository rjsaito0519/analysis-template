#!/usr/bin/env python3
"""Plot the example ROOT output with uproot and matplotlib."""

from __future__ import annotations

import argparse
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import uproot


def plot_histogram(input_file: Path, output_file: Path) -> None:
    with uproot.open(input_file) as root_file:
        values, edges = root_file["h_value"].to_numpy()

    centers = (edges[:-1] + edges[1:]) / 2
    widths = edges[1:] - edges[:-1]
    figure, axis = plt.subplots(figsize=(7, 5), constrained_layout=True)
    axis.bar(centers, values, width=widths, align="center")
    axis.set(xlabel="value", ylabel="events", title="Example analysis")
    output_file.parent.mkdir(parents=True, exist_ok=True)
    figure.savefig(output_file, dpi=160)
    plt.close(figure)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("input", type=Path)
    parser.add_argument("--output", type=Path, default=Path("plot.png"))
    args = parser.parse_args()
    plot_histogram(args.input, args.output)


if __name__ == "__main__":
    main()
