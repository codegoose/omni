#!/bin/sh -l
apt update
apt install ninja-build -y
cd /emsdk
./emsdk activate
./emsdk_env.sh
export PATH=$PATH:/emsdk/upstream/emscripten
cd /github/workspace/build
emcmake cmake -G "Ninja" .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target install --config Release
