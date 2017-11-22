#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>

#include "../grid.h"
#include "../io.h"

#define	 CUBE(x) (x*x*x)

int32_t mode, GridSize, snap_low, snap_high;
char *fin_densgrid, *foutgrid, *fin_partialgrid;
grid_t *inputgrid, *densgrid; // Note: Input grid is either a clean grid if mode = 0 or the one read in if mode = 1;

void parse_params(int argc, char **argv)
{

  if (argc == 6 || argc == 7) // This can be run with either 6 or 7 input arguments.
  {
    // Just fall through and run the code properly.
  }
  else
  {
    fprintf(stderr, "This sums up the individual chunk grids from densfield into a combined grid.\nWe can either initialize an empty grid or read in a previously defined partially summed grid to continue summing it.\n");
    fprintf(stderr, "Usage : ./densgrid <Mode of Operation> <GridSize> <Input Densfield Grid> <Output Name> <Snapshot Low> <Snapshot High> <Input Summed Grid (if mode of Operation = 1)>\n");
    exit(0);
  }

  mode = atoi(argv[1]);
  if (mode != 0 || mode != 1)
  {
    printf("The mode of operation must be either 0 (allocating a clean grid for the first summation) or 1 (reading in a partially summed grid to continue summing)\n");
  }


  if(mode == 0)
  {
    printf("We are allocating a clean grid to do the first summation.\n");
  } else
  { 
    printf("We are reading in a partially summed grid to continue summation.\n");
  } 

  GridSize = atoi(argv[2]);
  if (GridSize < 64 || GridSize > 2048)
  {
    fprintf(stderr, "The GridSize must be between 64 and 2048\n"); 
    exit(0);
  }

  fin_densgrid = strdup(argv[3]);
  foutgrid = strdup(argv[4]);

  snap_low = atoi(argv[5]);
  snap_high = atoi(argv[6]);

  if (mode == 1)
  {
    fin_partialgrid= strdup(argv[7]);
  }

  printf("==================================================\n");
  if (mode == 0)
  {
    printf("Summing up the Densfield grids with Mode of operation %d, GridSize %d, Input Densfield Base %s, Output name %s, Input Snapshots %d-%d\n", mode, GridSize, fin_densgrid, foutgrid, snap_low, snap_high); 
  }
  else
  {
    printf("Summing up the Densfield grids with Mode of operation %d, GridSize %d, Input Densfield Base %s, Output name %s, Input Snapshots %d-%d and partial summed input grid %s\n", mode, GridSize, fin_densgrid, foutgrid, snap_low, snap_high, fin_partialgrid);
  } 
  printf("==================================================\n\n");
}


int main(int argc, char **argv)
{

  // Have two modes of operation: 
  // // For the first one we malloc clean grids.
  // // For the second one we read in the partially summed grids.
  // Open up the densfield grids and read in the data.
  // Add it to either the clean grid or the partially summed ones.
  // Write back out.
  

  parse_params(argc, argv); // Set the input parameters. 

  inputgrid = malloc_grid(inputgrid);

  if (mode == 0)
  {

  }
  else
  {
    
   

  return 0;
}
