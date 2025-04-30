#!/bin/bash

# put your gem5 STT path here
STT_PATH=$(realpath .)

SPEC_PATH=$(realpath ../spec2017)
PERL_PATH=$SPEC_PATH/500.perlbench_r/
EXE_PATH=$PERL_PATH/exe/perlbench_r_base.gem5_se-m64

cd "$PERL_PATH/data/all/input"

# gem5 output path
OUT_DIR=$STT_PATH/dopp_outputs/${EXE}

# gem5 configuration file
CONFIG_FILE=$STT_PATH/configs/example/se.py


"$STT_PATH/build/X86_MESI_Two_Level/gem5.opt" --outdir="$OUT_DIR" \
    "$CONFIG_FILE" \
    --num-cpus=1 --mem-size=4GB \
    --caches --l2cache --cpu-type=DerivO3CPU \
    --threat_model=Spectre --needsTSO=1 --STT=1 --implicit_channel=1 \
    --moreTransmitInsts=0 --ifPrintROB=0 \
    --DOPP=1 \
    --doppPredictorType=1 \
    --doppPredictorSize=500 \
    -c "$EXE_PATH" \
    -o "diffmail.pl"
