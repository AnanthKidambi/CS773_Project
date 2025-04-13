#!/bin/bash

# put your gem5 STT path here
STT_PATH=.

# put your executable path here
# EXE_PATH=../gapbs/bc
# EXE_PATH=../gapbs/bfs 
# EXE_PATH=../gapbs/cc_sv
# EXE_PATH=../gapbs/cc
# EXE_PATH=../gapbs/pr
# EXE_PATH=../gapbs/pr_spmv
# EXE_PATH=../gapbs/sssp
# EXE_PATH=../gapbs/tc
# EXE_PATH=../gapbs/${EXE}
EXE_PATH=dopp_test/tainted_acc

# gem5 output path
OUT_DIR=$STT_PATH/stt_outputs/${EXE}

# gem5 configuration file
CONFIG_FILE=$STT_PATH/configs/example/se.py


$STT_PATH/build/X86_MESI_Two_Level/gem5.opt --outdir=$OUT_DIR \
    $CONFIG_FILE \
    --num-cpus=1 --mem-size=4GB \
    --caches --l2cache --cpu-type=DerivO3CPU \
    --threat_model=Spectre --needsTSO=1 --STT=1 --implicit_channel=1 \
    --moreTransmitInsts=0 --ifPrintROB=0 \
    --DOPP=0 \
    -c $EXE_PATH \
    -o "100000" \
    # -o "-g 7 -n 8"
