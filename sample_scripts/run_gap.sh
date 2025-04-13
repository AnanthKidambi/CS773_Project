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
    echo "Processing: $gap_exe"

    EXE_PATH=../gapbs/$gap_exe
    CONFIG_FILE=$STT_PATH/configs/example/se.py

    # --------------------------------------------------------------- #
    OUT_DIR=$STT_PATH/orig_outputs/$gap_exe
    mkdir -p $OUT_DIR
    $STT_PATH/build/X86_MESI_Two_Level/gem5.opt --outdir=$OUT_DIR \
    $CONFIG_FILE \
    --num-cpus=1 --mem-size=4GB \
    --caches --l2cache --cpu-type=DerivO3CPU \
    --threat_model=Spectre --needsTSO=1 --STT=0 --implicit_channel=0 \
    --moreTransmitInsts=0 --ifPrintROB=0 \
    --DOPP=0 \
    -c $EXE_PATH \
    -o "-g $GRAPHSIZE -n $ITERS" 1>$OUT_DIR/out 2>$OUT_DIR/err&
    # --------------------------------------------------------------- #
    # --------------------------------------------------------------- #
    OUT_DIR=$STT_PATH/stt_outputs/$gap_exe
    mkdir -p $OUT_DIR
    $STT_PATH/build/X86_MESI_Two_Level/gem5.opt --outdir=$OUT_DIR \
    $CONFIG_FILE \
    --num-cpus=1 --mem-size=4GB \
    --caches --l2cache --cpu-type=DerivO3CPU \
    --threat_model=Spectre --needsTSO=1 --STT=1 --implicit_channel=1 \
    --moreTransmitInsts=0 --ifPrintROB=0 \
    --DOPP=0 \
    -c $EXE_PATH \
    -o "-g $GRAPHSIZE -n $ITERS" 1>$OUT_DIR/out 2>$OUT_DIR/err&
    # --------------------------------------------------------------- #
    # --------------------------------------------------------------- #
    OUT_DIR=$STT_PATH/dopp_outputs/$gap_exe
    mkdir -p $OUT_DIR
    $STT_PATH/build/X86_MESI_Two_Level/gem5.opt --outdir=$OUT_DIR \
    $CONFIG_FILE \
    --num-cpus=1 --mem-size=4GB \
    --caches --l2cache --cpu-type=DerivO3CPU \
    --threat_model=Spectre --needsTSO=1 --STT=1 --implicit_channel=1 \
    --moreTransmitInsts=0 --ifPrintROB=0 \
    --DOPP=1 \
    -c $EXE_PATH \
    -o "-g $GRAPHSIZE -n $ITERS" 1>$OUT_DIR/out 2>$OUT_DIR/err&
    # --------------------------------------------------------------- #

    # Wait for all background processes to finish before continuing
    wait -n 3
    echo "Finished processing: $gap_exe"

done