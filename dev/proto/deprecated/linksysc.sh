#!/usr/bin/env sh
set -o errexit

echo "Linking files from kernel examples..."
ln -f $SYSC_KERNEL/examples/sysc/counter/counter.cpp sysc_counter.cpp
ln -f $SYSC_KERNEL/examples/sysc/counter/counter_tb.cpp sysc_counter_tb.cpp
