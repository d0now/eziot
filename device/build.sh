#!/bin/bash

if [ ! $1 ]; then
    DEVICE_BUILD_TARGET='debug'
else
    DEVICE_BUILD_TARGET="$1"
fi

# Cleanup
if [ -d build ]; then
    rm -rf build/
    mkdir build
fi

if [ "$DEVICE_BUILD_TARGET" == "debug" ]; then

    cd service/build
        rm -rf ./*
        cmake .. -DCMAKE_C_COMPILER=clang -DCMAKE_ASM_COMPILER=clang
        cmake --build . --config Release -v
    cd ../..

    cp service/build/Release/bin/eziot-service initrd/debug

    tools/pack_initrd.sh initrd/debug build/initrd
    cp kernel/linux-5.10.74-arm.zImage build/kernel
    cp kernel/versatile-pb.dtb build/dtb
    cp start.sh build/start.sh
    cp Dockerfile build/Dockerfile

    sudo docker build -t eziot-device:debug build/

elif [ "$DEVICE_BUILD_TARGET" == "release" ]; then
    echo "Not implemented."
    exit 0

else
    echo "Invalid build target."
    exit 0
fi