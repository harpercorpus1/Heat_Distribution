General Info:

as2_seq.c - the sequential implementation

as2_par.c - the parallel implementation

par_script.sh - this will run the parallel implementation for iteration counts of (10, 100, 1000, 10000)
    it will produce 4 .png files that are named for their iteration counts

seq_script.sh - this will run the sequential implementation for iteration counts of (10, 100, 1000, 10000)
    it will produce 4 .png files that are named for their iteration counts


Runtime Instructions:

1. Create Iterative job outside of the login node 

Example: qsub -I -l select=1:ncpus=16:mpiprocs=16:mem=10gb:interconnect=1g,walltime=01:00:00

2. Load Necessary Modules 

module load imagemagick/7.0.8-7-gcc/8.3.1
module load openmpi/4.0.5-gcc/8.3.1-ucx

3. Run Program Using scripts or compile/run instructions below

scripts:
    run scripts with commands "./par_script.sh" or "seq_script.sh"

compile/run:
    Parallel Code: 
        Compiling: "mpicc as2_par.c -o as2_par"
        Running: "mpirun -np [ ARG 1 ] --mca mpi_cuda_support 0 as2_par [ ARG 2 ]"
        ARG 1: the number of processes you want to run on 
        ARG 2: (Optional) number of iterations (Defaults to 50,000)
    
    Sequential Code:
        Compiling: "gcc as2_seq.c -o as2_seq"
        Running: "./as2_seq [ ARG 1 ]
        ARG 1: (Optional) number of iterations (Defaults to 50,000)