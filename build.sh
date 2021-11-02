#!/bin/bash

if [ ! $1 ]; then
    DEVICE_BUILD_TARGET='debug'
else
    DEVICE_BUILD_TARGET="$1"
fi

if [ "$DEVICE_BUILD_TARGET" == "debug" ]; then

    cd device
        ./build.sh debug
    cd ..

    sudo docker-compose build
    sudo docker-compose up -d

elif [ "$DEVICE_BUILD_TARGET" == "release" ]; then
    echo "Not implemented."
    exit 0

else
    echo "Invalid build target."
    exit 0
fi