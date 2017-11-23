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

char *fin_grid, *fout_grid;
int32_t GridSize;

void parse_params(int32_t argc, char **argv);

void parse_params(int32_t argc, char **argv)
{

  if (argc != 4)
  {
    fprintf(stderr, "This program reads in a density grid and normalizes it to rho/<rho>.\n");
    fprintf(stderr, "Usage: normalize_grid <GridSize> <Input Grid Name> <Output Grid Name>");
    exit(EXIT_FAILURE);
  }

  GridSize = atoi(argv[1]);
  if (GridSize < 64 || GridSize > 1028)
  {
    fprintf(stderr, "The grid size must be between 64 and 1028\n");
    exit(EXIT_FAILURE);
  }

  fin_grid = strdup(argv[2]); 

  printf("==================================================\n");
  printf("Running 'normalize_grid' with a GridSize %d, input grid %s and output grid %s\n", GridSize, fin_grid, fout_grid); 
  printf("==================================================\n\n");
}

int32_t main(int32_t argc, char **argv)
{

  grid_t my_grid;

  parse_params(argc, argv); 
 
  my_grid = malloc(sizeof(struct grid_struct));
  malloc_grid(my_grid, GridSize);  

  read_grid_double(fin_grid, GridSize, my_grid->density);
  normalize_density(my_grid);
  write_grid_to_file_double(fout_grid, GridSize, my_grid->density);

  return EXIT_SUCCESS;
}
