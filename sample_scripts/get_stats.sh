#!/bin/bash

STT_PATH=.

GRAPHSIZE=14
ITERS=4

# Define a list of GAP executables
gap_executables=(
    "bc"
    "bfs"
    "cc_sv"
    "cc"
    "pr"
    "pr_spmv"
    "sssp"
    "tc"
)

# Iterate through the list
for gap_exe in "${gap_executables[@]}"; do
    EXE_PATH=../gapbs/$gap_exe
    CONFIG_FILE=$STT_PATH/configs/example/se.py

    orig_tick=$(cat $STT_PATH/orig_outputs/$gap_exe/out | grep Exit | awk '{print $4}')
    stt_tick=$(cat $STT_PATH/stt_outputs/$gap_exe/out | grep Exit | awk '{print $4}')
    dopp_tick=$(cat $STT_PATH/dopp_outputs/$gap_exe/out | grep Exit | awk '{print $4}')

    printf "'%s': %s,\n" \
        "$gap_exe" \
        "$dopp_tick"
done