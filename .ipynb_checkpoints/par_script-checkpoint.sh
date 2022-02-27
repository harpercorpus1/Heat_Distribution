#!/bin/bash

mpicc as2_par.c -o as2_par
for IT in 10 100 1000 10000 100000
do
	for NCPUS in 2 4 8 16
	do
		perf stat -o par_perf_${IT}_${NCPUS}.txt mpirun -np ${NCPUS} --mca mpi_cuda_support 0 as2_par ${IT}
		cp as2.png as2_par_${IT}.png
		rm as2.png
		rm as2.pnm
	done
done
