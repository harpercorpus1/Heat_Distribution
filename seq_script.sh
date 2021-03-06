#!/bin/bash

gcc as2_seq.c -o as2_seq
for VAR in 10 100 1000 10000 100000
do
	perf stat -o perf_seq_${VAR}.txt ./as2_seq ${VAR}
	cp as2.png as2_seq_${VAR}.png
	rm as2.png
	rm as2.pnm
done
