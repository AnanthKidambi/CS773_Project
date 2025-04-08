#!/bin/bash

# put your gem5 STT path here
STT_PATH=.

# put your executable path here
# EXE_PATH=../microbench/CCa/bench.X86
EXE_PATH=../gapbs/bfs 

# gem5 output path
OUT_DIR=$STT_PATH/output

# gem5 configuration file
CONFIG_FILE=$STT_PATH/configs/example/se.py


$STT_PATH/build/X86_MESI_Two_Level/gem5.opt --outdir=$OUT_DIR \
    $CONFIG_FILE \
    --num-cpus=1 --mem-size=4GB \
    --caches --l2cache --cpu-type=DerivO3CPU \
    --threat_model=Spectre --needsTSO=1 --STT=1 --implicit_channel=1 \
    --moreTransmitInsts=0 --ifPrintROB=0 \
    -c $EXE_PATH \
    -o "-g 8 -n 5"

