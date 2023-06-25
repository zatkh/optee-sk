#!/bin/bash

################################################################################
# Copyright (c) 2022 Microsoft Corporation
################################################################################

export TOP_DIR=$1
export INSTALL_DEPENDENCY=$2
export ARCH=x86
export CC=x86_64-linux-gnu-gcc
export CXX=x86_64-linux-gnu-gcc-g++
export TA_DEV_KIT_ROOT=$TOP_DIR/
export GCC_TOOLCHAIN_PATH=/usr/bin/
export CROSS_COMPILE=$GCC_TOOLCHAIN_PATH/x86_64-linux-gnu-
export CROSS_COMPILE64=$GCC_TOOLCHAIN_PATH/x86_64-linux-gnu-
export  O=$TOP_DIR/out/x64-plat-x86_64
export  CFG_TEE_BENCHMARK=n
export  CFG_TEE_CORE_LOG_LEVEL=4
export CFG_TEE_TA_LOG_LEVEL=4
export CFG_TEE_CORE_DEBUG=y
export CFG_DEBUG_INFO=y
export CFG_LVBS_KERNEL_HVCI=y
#export  CFG_WITH_USER_TA=n
export  DEBUG=1
export O=./out

if $INSTALL_DEPENDENCY
then
    echo "install some dependencies"
    pip3 install pycrypto
    pip3 install pyelftools
    pip3 install pycryptodomex

fi
