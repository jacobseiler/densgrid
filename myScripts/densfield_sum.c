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

int32_t mode, GridSize, chunk_low, chunk_high; 
char *fin_densfield_grid, *foutgrid, *fin_partial_grid;

int32_t fill_grid_from_file(char *filebase, int32_t precision, grid_t grid);
void parse_params(int argc, char **argv);

void parse_params(int argc, char **argv)
{

  int32_t arg_idx;

  printf("Parsed %d parameters\n", argc);
  if (argc == 7 || argc == 8) // This can be run with either 7 or 8 input arguments.
  {
    // Just fall through and run the code properly.
  }
  else
  {
    fprintf(stderr, "This sums up the individual chunk grids from densfield into a combined grid.\nWe can either initialize an empty grid or read in a previously defined partially summed grid to continue summing it.\n");
    fprintf(stderr, "Usage : ./densgrid <Mode of Operation> <GridSize> <Input Densfield Grid> <Output Name> <Chunk Low (Inclusive)> <Chunk High (Inclusive)> <Input Summed Grid (if mode of Operation = 1)>\n");
    exit(0);
  }

  mode = atoi(argv[1]);
  if (mode == 0 || mode == 1)
  {
    // Acceptable mode values.
  }
  else
  {
    fprintf(stderr, "The mode of operation must be either 0 (allocating a clean grid for the first summation) or 1 (reading in a partially summed grid to continue summing)\n");
    exit(EXIT_FAILURE);
  }

  if (mode == 0 && argc == 8)
  {
    fprintf(stderr, "You have selected to allocate an empty grid but have also passed an argument for a partial summed grid.\n");
    exit(EXIT_FAILURE);
  }

  if (mode == 0)
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
    exit(EXIT_FAILURE);
  }

  fin_densfield_grid = strdup(argv[3]);
  foutgrid = strdup(argv[4]);

  chunk_low = atoi(argv[5]);
  chunk_high = atoi(argv[6]);

  if (mode == 1)
  {
    fin_partial_grid= strdup(argv[7]);
  }

  printf("==================================================\n");
  if (mode == 0)
  {
    printf("Summing up the Densfield grids with Mode of operation %d, GridSize %d, Input Densfield Base %s, Output name %s, Input chunks %d-%d\n", mode, GridSize, fin_densfield_grid, foutgrid, chunk_low, chunk_high); 
  }
  else
  {
    printf("Summing up the Densfield grids with Mode of operation %d, GridSize %d, Input Densfield Base %s, Output name %s, Input chunks %d-%d and partial summed input grid %s\n", mode, GridSize, fin_densfield_grid, foutgrid, chunk_low, chunk_high, fin_partial_grid);
  }

  printf("The run time command was ");
  for (arg_idx = 0; arg_idx < argc; ++arg_idx)
  {
    printf("%s ", argv[arg_idx]);
  }
  printf("\n");
  printf("==================================================\n\n");
}

int32_t fill_grid_from_file(char *filebase, int32_t precision, grid_t grid)
{

  char fname[1024];

  snprintf(fname, 1024, "%s.dens.dat", filebase);
  read_grid_double(fname, grid->GridSize, grid->density);

  snprintf(fname, 1024, "%s.vx.dat", filebase);
  read_grid_double(fname, grid->GridSize, grid->vx);

  snprintf(fname, 1024, "%s.vy.dat", filebase);
  read_grid_double(fname, grid->GridSize, grid->vy); 

  snprintf(fname, 1024, "%s.vz.dat", filebase);
  read_grid_double(fname, grid->GridSize, grid->vz);

  return EXIT_SUCCESS;

}

int main(int argc, char **argv)
{

  // Have two modes of operation: 
  // // For the first one we malloc clean grids.
  // // For the second one we read in the partially summed grids.
  // Open up the densfield grids and read in the data.
  // Add it to either the clean grid or the partially summed ones.
  // Write back out.
  
  char fname_densfield_gridchunk[1024]; 
  int32_t chunk_idx;

  grid_t my_grid, densfield_grid; // Note: Input grid is either a clean grid if mode = 0 or the one read in if mode = 1;

  parse_params(argc, argv); // Set the input parameters. 
 
  my_grid = malloc(sizeof(struct grid_struct));
  densfield_grid = malloc(sizeof(struct grid_struct));
  
  malloc_grid(my_grid, GridSize); 
  malloc_grid(densfield_grid, GridSize); 

  if (mode == 0) // If we want to start with a brand new grid, then initialize it all to 0.
  {
    init_grid(my_grid);
  }
  else
  {
    fill_grid_from_file(fin_partial_grid, 2, my_grid); // Otherwise read the grids from file.
  } 

  for (chunk_idx = chunk_low; chunk_idx < chunk_high + 1; ++chunk_idx)
  {
  
    snprintf(fname_densfield_gridchunk, 1024, "%s_chunk%d", fin_densfield_grid, chunk_idx);
    fill_grid_from_file(fname_densfield_gridchunk, 2, densfield_grid);

    add_grids(my_grid, densfield_grid);
  }  

  write_grids(foutgrid, 2, my_grid); 
 
  return EXIT_SUCCESS;
}
