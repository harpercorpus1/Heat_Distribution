/**
 * Harper Corpus
 * October 12, 2021
 * CPSC 4770
 * Assignment 2 Sequential Head Distribution Simulation
 */

#include <stdio.h>
#include <stdlib.h>

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
 * this function will initialize the points in the arrays as necessary
 * roomptr - void* - points to float array that holds the values for the room
 */
void roomArrayInit(void* roomptr){
   // creates a 2d float array from void pointer
   float (*room)[N] = roomptr;
   int i, j;
   // gets indices of 30th and 70th percentile positions
   // this is used to center the 40% for the fire place
   int thirty = (int)(0.3 * (float)N);
   int seventy = (int)(0.7 * (float)N);
   // loops through the first row of the array, and places the fireplace
   for(i = 0; i < thirty; i++) room[0][i] = 20.0;
   for(i = thirty; i < seventy; i++) room[0][i] = 300.0;
   for(i = seventy; i < N; i++) room[0][i] = 20.0;
   // populates all other values of the array
   for(i = 1; i < N; i++){
      for(j = 0; j < N; j++){
         room[i][j] = 20.0;
      }
   }
}

/**
 * CopyNewToOld takes two pointers, and copies all values from 
 *       oldptr to newptr
 * oldptr - void* - points to the original 2d array that will be 
 *       placed in the other array
 * newptr - void* - points to the array to be copied to
 */ 
void CopyNewToOld(void* oldptr, void* newptr){
   // creates 2 2d arrays to hold the values from the pointer
   float (*old)[N] = oldptr;
   float (*new)[N] = newptr;
   int i, j;
   // copies the values
   for(i = 0; i < N; i++){
      for(j = 0; j < N; j++){
         new[i][j] = old[i][j];
      }
   }
}

/**
 * CalculateNew takes two pointers, and calculates the average of all 
 * neighboring points, and places it in the ouput array
 * srcptr - void* - points to the 2d array that remains constant
 * destptr - void* - points to the 2d array that the arithmetic product will 
 *       be placed into
 */
void CalculateNew(void* destptr, void* srcptr){
   // creates 2 2d arrays to hold the values from the pointer
   float (*src)[N] = srcptr;
   float (*dest)[N] = destptr;
   int i, j;
   // populates product array with averages
   for(i = 1; i < N-1; i++){
      for(j = 1; j < N-1; j++){
         dest[i][j] = 0.25 * (src[i-1][j] + src[i+1][j] + src[i][j-1] + src[i][j+1]);
      }
   }
}

/**
 * PrintGrid outputs the values of the 2d array into a .pnm file as colors
 * roomptr - void* - pionts to the 2d array to be outputted
 */
void PrintGrid(void* roomptr){
   // creates 2 2d arrays to hold the values from the pointer
   float (*room)[N] = roomptr;
   
   // creates file pointer to write to
   FILE* fp = fopen("as2.pnm", "w");
   fprintf(fp, "P3\n%d %d\n15\n", N, N);
   int i, j;
   int tempVal_int;
   // loops through all values of 2d array, and outputs the values
   for (i = 0; i < N; i++) {
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
   // closes file pointer
   fclose(fp);

   // converts .pnm file to .png
   system("convert as2.pnm as2.png");
}

int main(int argc, char * argv[]) {
   int iterations = 50000;
    
   if(argc > 1) iterations = atoi(argv[1]);

   float room[N][N];
   float otherRoom[N][N];

   // sequential simulation logic
   roomArrayInit(room);
   for(int i = 0; i < iterations; i++){
      CopyNewToOld(room, otherRoom);
      CalculateNew(room, otherRoom);
   }
   PrintGrid(room);

   return 0;
}  
   

