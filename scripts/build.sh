#!/bin/bash

cmake -B build -DCMAKE_BUILD_TYPE=Release
make -C build -j$(nproc)
ln -sf build/compile_commands.json
