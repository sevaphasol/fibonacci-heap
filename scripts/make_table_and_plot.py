import argparse
import json
import re
from pathlib import Path
from typing import List, Dict

import matplotlib.pyplot as plt
import pandas as pd


def parse_args():
    parser = argparse.ArgumentParser(description="Plot Google Benchmark results.")
    parser.add_argument(
        "--input",
        required=True,
        type=Path,
        help="Path to benchmark JSON file"
        )
    parser.add_argument(
        "--artifacts",
        required=True,
        default=Path("art"),
        type=Path,
        help="Ditectory for table and plot"
        )
    parser.add_argument(
        "--output-plot",
        required=True,
        default=Path("bench.png"),
        type=Path,
        help="Where to save the plot (png, svg, etc.)",
    )
    parser.add_argument(
        "--output-table",
        required=True,
        default=Path("bench.csv"),
        type=Path,
        help="Where to save the table (CSV)",
    )
    return parser.parse_args()


def load_rows(json_path: Path) -> List[Dict]:
    data = json.loads(json_path.read_text())
    pattern = re.compile(r"([^/]+)/([0-9]+)")
    rows = []
    for bench in data.get("benchmarks", []):
        if bench.get("aggregate_name"):
            continue
        name = bench.get("name", "")
        match = pattern.match(name)
        if not match:
            continue
        heap = match.group(1)
        size = int(match.group(2))
        rows.append(
            {
                "heap": heap,
                "size": size,
                "cpu_time_ns": bench.get("cpu_time", 0.0),
                "real_time_ns": bench.get("real_time", 0.0),
                "iterations": bench.get("iterations", 0),
            }
        )
    return rows


def plot(df: pd.DataFrame, output: Path) -> None:
    plt.figure(figsize=(10, 6))
    for heap, group in df.groupby("heap"):
        sorted_group = group.sort_values("size")
        plt.plot(
            sorted_group["size"],
            sorted_group["cpu_time_ns"] / 1e6,
            marker="o",
            label=heap,
        )
    plt.xlabel("Input size (elements)")
    plt.ylabel("CPU time (ms)")
    plt.title("Benchmark results")
    plt.grid(True, which="both", linestyle="--", alpha=0.5)
    plt.legend()
    plt.xscale("log")
    plt.tight_layout()
    plt.savefig(output)
    print(f"Plot saved to {output}")


def main():
    args = parse_args()

    args.artifacts.mkdir(parents=True, exist_ok=True)

    rows = load_rows(args.input)
    if not rows:
        raise SystemExit("No benchmark rows found in the input file.")

    df = pd.DataFrame(rows)
    df.sort_values(["heap", "size"], inplace=True)

    table_path = args.artifacts / args.output_table
    df.to_csv(table_path, index=False)
    print(f"Table saved to {table_path}")

    plot_path = args.artifacts / args.output_plot
    plot(df, plot_path)



if __name__ == "__main__":
    main()
