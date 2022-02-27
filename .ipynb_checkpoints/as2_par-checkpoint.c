/**
 * Harper Corpus
 * October 12, 2021
 * Parallel Heat Distribution Simulation
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define RED      "15 00 00 "
#define ORANGE   "15 05 00 "
#define YELLOW   "15 10 00 "
#define LTGREEN  "00 13 00 "
#define GREEN    "05 10 00 "
#define LTBLUE   "00 05 10 "
#define BLUE     "00 00 10 "
#define DARKTEAL "00 05 05 "
#define BROWN    "03 03 00 "
#define BLACK    "00 00 00 "

#define N 1000

/**
 * partial_room_init  initializes the points in the arrays as necessary
 * will populate a 2d array of size arr[numRows][N]
 * room - float** - 2d array holding temperatures of the room
 * numRows - int - number of rows in the 2d array
 * rank - int - rank of the process that the function is being called on
 */
void partial_room_init(float** room, int numRows, int rank){
 	int i = 0, j;
    // if condition exists so that the function can populate the first row as a 
    // fireplace only on the first process
    if(!rank){
        // gets indices of 30th and 70th percentile positions
        // this is used to center the 40% for the fire place
        int thirty = (int)(0.3 * N);
        int seventy = (int)(0.7 * N);
        for(i = 0; i < thirty; i++) room[0][i] = 20.0;
        for(i = thirty; i < seventy; i++) room[0][i] = 300.0;
        for(i = seventy; i < N; i++) room[0][i] = 20.0;
        i = 1;
    }
    // populates all other values of the array
	for( ; i < numRows; i++){
		for(j = 0; j < N; j++){
			room[i][j] = 20.0;
		}
	}
}

/**
 * CopyNewToOld takes two pointers, and an integer, copies all values from the source
 * pointer to the destination pointer
 * dest - float** - 2d array which will hold the copied values
 * src - float** - 2d array which will be copied into dest
 * numRows - int - number of rows in 2d array
 */ 
void CopyNewToOld(float** dest, float** src, int numRows){
    int i, j;
    // copies values 
    for(i = 0; i < numRows; i++){
        for(j = 0; j < N; j++){
            dest[i][j] = src[i][j];
        }
    }
}

/**
 * CalculateNew has two 2d pointers, and 2 1d pointers, this will be used to 
 * compute the average of neighboring points, and store them in dest
 * dest - float** - 2d array of floats that will hold the averages, used in 
 *      later computations
 * src - float** - 2d array of floats that will be used as the original points, 
 *      to calculate neighboring points
 * numRows - int - number of rows in the 2d array pointers
 * topRow - float* - 1d array meaning the row in the overall grid, that is located
 *      above the values of the smaller 2d arrays
 * botRow - float* - 1d array meaning the row in the overall grid, that is located
 *      below the values of the smaller 2d arrays
 * rank - int - holds the rank of the process that the function is being called on
 * size - int - the number of processes running the program
 */
void CalculateNew(float** dest, float** src, int numRows, float* topRow, float* botRow, int rank, int size){
    int i, j;
    // is the rank is 0, then we need to use the topRow array for the top neighbor
    if(rank != 0){
        for(j = 1; j < N-1; j++){
            dest[0][j] = 0.25 * (topRow[j] + src[1][j] + src[0][j-1] + src[0][j+1]);
        }
    }
    // gets average of neighboring points in the inner points
    for(i = 1 ; i < numRows-1; i++){
        for(j = 1; j < N-1; j++){
            dest[i][j] = 0.25 * (src[i-1][j] + src[i+1][j] + src[i][j-1] + src[i][j+1]);
        }
    }
    // is the rank is equal to size-1 then we need to use the botRow array for the
    // top neighbor
    if(rank != size-1){
        int last = numRows-1;
        for(j = 1; j < N-1; j++){
            dest[last][j] = 0.25 * (src[last-1][j] + botRow[j] + src[last][j-1] + src[last][j+1]);
        }
    }
}

/**
 * PrintGridToFile outputs the values of the array to the .pnm file
 * room - float** - 2d array that holds the temperatures of the room
 * numRows - int - number of rows in the 2d array
 * fp - FILE* - file to output values
 */
void PrintGridToFile(float** room, int numRows, FILE* fp){
    int i, j;
    int tempVal_int;
    for (i = 0; i < numRows; i++) {
        for (j = 0; j < N; j++){
            tempVal_int = (int)room[i][j];
            if(tempVal_int <= 20) fprintf(fp, "%s ", BLACK );
            else if(tempVal_int <= 30) fprintf(fp, "%s ", BROWN );
            else if(tempVal_int <= 40) fprintf(fp, "%s ", DARKTEAL );
            else if(tempVal_int <= 50) fprintf(fp, "%s ", BLUE );
            else if(tempVal_int <= 60) fprintf(fp, "%s ", LTBLUE );
            else if(tempVal_int <= 80) fprintf(fp, "%s ", GREEN );
            else if(tempVal_int <= 120) fprintf(fp, "%s ", LTGREEN );
            else if(tempVal_int <= 180) fprintf(fp, "%s ", YELLOW );
            else if(tempVal_int <= 250) fprintf(fp, "%s ", ORANGE );
            else fprintf(fp, "%s ", RED );
        }
        fprintf(fp, "\n");
   }
}

/**
 * get_ghost_rows function will calculate the rows above and below the values available
 * to the current process, and save them in top and bot
 * rank - int - rank of current process
 * rows_per - int - number of rows in 2d array given to current process
 * size - int - number of processes
 * top - float* - array used to store row above process
 * bot - float* - array used to store row below process
 * room - float** - portion of room delegated to process
 */
void get_ghost_rows(int rank, int rows_per, int size, float* top, float* bot, float** room){
    // these objects are required for send/recv
    MPI_Status status;
    int tag = 0;
    // splits the processes up into two groups, and communicates between the two, so
    // that we do not have a deadlock
    if(rank % 2 == 0){
        // if rank is 0, we want top to be NULL, but we still want to
        // assign bot
        if(rank == 0){
            top = NULL;
            MPI_Send(room[rows_per-1], N, MPI_FLOAT, rank+1, tag, MPI_COMM_WORLD);
            MPI_Recv(bot, N, MPI_FLOAT, rank+1, tag, MPI_COMM_WORLD,  &status);
        // if we are working with the process with the highest rank, there is
        // no row below
        }else if(rank == size-1){
            bot = NULL;
            MPI_Send(room[0], N, MPI_FLOAT, rank-1, tag, MPI_COMM_WORLD);
            MPI_Recv(top, N, MPI_FLOAT, rank-1, tag, MPI_COMM_WORLD,  &status);      
        }else{
            MPI_Send(room[0], N, MPI_FLOAT, rank-1, tag, MPI_COMM_WORLD);
            MPI_Recv(top, N, MPI_FLOAT, rank-1, tag, MPI_COMM_WORLD,  &status);
            MPI_Send(room[rows_per-1], N, MPI_FLOAT, rank+1, tag, MPI_COMM_WORLD);
            MPI_Recv(bot, N, MPI_FLOAT, rank+1, tag, MPI_COMM_WORLD,  &status);
        }
    }else{
        // with only odd values we do not have to worry about rank == 0, as that
        // is only even
        // if we are working with the process with the highest rank, there is
        // no row below
        if(rank == size-1){
            bot = NULL;
            MPI_Recv(top, N, MPI_FLOAT, rank-1, tag, MPI_COMM_WORLD,  &status); 
            MPI_Send(room[0], N, MPI_FLOAT, rank-1, tag, MPI_COMM_WORLD);
        }else{
            MPI_Recv(top, N, MPI_FLOAT, rank-1, tag, MPI_COMM_WORLD,  &status);
            MPI_Send(room[0], N, MPI_FLOAT, rank-1, tag, MPI_COMM_WORLD);
            MPI_Recv(bot, N, MPI_FLOAT, rank+1, tag, MPI_COMM_WORLD,  &status);
            MPI_Send(room[rows_per-1], N, MPI_FLOAT, rank+1, tag, MPI_COMM_WORLD);
        }
    }
}

int main(int argc, char* argv[]){
    int rank, size;
    int rows_per, i;
    int iterations = 50000;

    // MPI Creation
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // assigns iterations if provided by command line 
    if(argc > 1) iterations = atoi(argv[1]);

    // calculates the number of rows assigned to each process
    rows_per = N / size;
    if(rank < (N % size)) rows_per++;
    
    // creates arrays to hold room temperatures
    float** room = (float**)malloc(N * sizeof(float*));
    float** roomcpy = (float**)malloc(N * sizeof(float*));
    for(i = 0; i < rows_per; i++){
        room[i] = (float*)malloc(N * sizeof(float));
        roomcpy[i] = (float*)malloc(N * sizeof(float));
    }

    // MPI_Barrier(MPI_COMM_WORLD);
    // populates array
    partial_room_init(room, rows_per, rank);

    // arrays used as top and bottom for ghost rows
    float *top, *bot;
    if(!rank) top = NULL;
    else top = (float*)calloc(N, sizeof(float));
    if(rank == size-1) bot = NULL;
    else bot = (float*)calloc(N, sizeof(float));
    
    // Parallel Simulation Logic
    for(i = 0; i < iterations; i++){
        get_ghost_rows(rank, rows_per, size, top, bot, room);
        CopyNewToOld(roomcpy, room, rows_per);
        //MPI_Barrier(MPI_COMM_WORLD);
        CalculateNew(room, roomcpy, rows_per, top, bot, rank, size);
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // creates/opens file from process 0
    if(!rank){
        FILE* fp = fopen("as2.pnm", "w");
        fprintf(fp, "P3\n%d %d\n15\n", N, N);
        PrintGridToFile(room, rows_per, fp);
        fclose(fp);
    }
    // writes data from all processes to the output file
    for(int i = 1; i < size; i++){
        MPI_Barrier(MPI_COMM_WORLD);
        if(i == rank){
            FILE* fp = fopen("as2.pnm", "a");
            PrintGridToFile(room, rows_per, fp);
            fclose(fp);
        }
    }
    
    // frees dynamically allocated data
    if(!rank){
        system("convert as2.pnm as2.png");
        free(top); free(bot);
        free(room); free(roomcpy);
    }
    
    MPI_Finalize();
    return 0;
}
