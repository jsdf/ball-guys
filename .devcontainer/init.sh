#!/bin/bash
set -euxo pipefail

cd /n64_toolchain || exit
touch env.sh
git clone -b preview https://github.com/DragonMinded/libdragon.git &&
    cd libdragon &&
    echo "export LIBDRAGON_VERSION="$(git rev-parse --short HEAD) >>/n64_toolchain/env.sh &&
    ./build.sh
echo "exit code: $?"
cd ..
git clone https://github.com/HailToDodongo/tiny3d.git &&
    cd tiny3d &&
    echo "export TINY3D_VERSION="$(git rev-parse --short HEAD) >>/n64_toolchain/env.sh &&
    export N64_INST=/n64_toolchain &&
    ./build.sh
echo "exit code: $?"
cd ..
echo "export N64_INST=/n64_toolchain" >>env.sh &&
    echo "export T3D_INST=/n64_toolchain/tiny3d" >>env.sh
echo "source /n64_toolchain/env.sh" >>~/.bashrc
