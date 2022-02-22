#!/bin/bash

mpicc as2_par.c -o as2_par
for VAR in 10 100 1000 10000
do
	mpirun -np 16 --mca mpi_cuda_support 0 as2_par $VAR
	cp as2.png as2_par_$VAR.png
	rm as2.png
	rm as2.pnm
done
