#!/usr/bin/env bash

if [[ $# -ne 2 ]]; then
    echo "Usage: $0 [artifacts_dir] [output_json]"
    exit 1
fi

ARTIFACTS_DIR="$1"
OUT_JSON="$2"

mkdir -p "$ARTIFACTS_DIR"

JSON_PATH="$ARTIFACTS_DIR/$OUT_JSON"

./build/fheap_bench --benchmark_out="$JSON_PATH".json
