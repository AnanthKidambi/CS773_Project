#!/bin/bash

STT_PATH=.

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
printf "\nStt ticks\n"
for gap_exe in "${gap_executables[@]}"; do

    stt_tick=$(cat $STT_PATH/stt_outputs/$gap_exe/out | grep Exit | awk '{print $4}')
    printf "'%s': %s,\n" \
        "$gap_exe" \
        "$stt_tick" 
done

printf "\nIP Stride ticks\n"
for gap_exe in "${gap_executables[@]}"; do

    dopp_ip_tick=$(cat $STT_PATH/dopp_outputs_ip_stride/$gap_exe/out | grep Exit | awk '{print $4}')
    printf "'%s': %s,\n" \
        "$gap_exe" \
        "$dopp_ip_tick" 
done

printf "\nCPLX Stride ticks\n"
for gap_exe in "${gap_executables[@]}"; do

    dopp_cplx_tick=$(cat $STT_PATH/dopp_outputs_cplx_stride/$gap_exe/out | grep Exit | awk '{print $4}')
    printf "'%s': %s,\n" \
        "$gap_exe" \
        "$dopp_cplx_tick" 
done

printf "\nOracle ticks\n"
for gap_exe in "${gap_executables[@]}"; do

    dopp_oracle_tick=$(cat $STT_PATH/dopp_outputs_oracle/$gap_exe/out | grep Exit | awk '{print $4}')
    printf "'%s': %s,\n" \
        "$gap_exe" \
        "$dopp_oracle_tick" 
done

printf "\nIP Stride Predictor Accuracy\n"
for gap_exe in "${gap_executables[@]}"; do

    dopp_ip_acc=$(cat $STT_PATH/dopp_outputs_ip_stride/$gap_exe/stats.txt | grep doppAccuracy | awk '{print $2}')
    printf "'%s': %s,\n" \
        "$gap_exe" \
        "$dopp_ip_acc" 
done

printf "\nCPLX Stride Predictor Accuracy\n"
for gap_exe in "${gap_executables[@]}"; do

    dopp_cplx_acc=$(cat $STT_PATH/dopp_outputs_cplx_stride/$gap_exe/stats.txt | grep doppAccuracy | awk '{print $2}')
    printf "'%s': %s,\n" \
        "$gap_exe" \
        "$dopp_cplx_acc" 
done