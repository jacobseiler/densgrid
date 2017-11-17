#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>

#include "grid.h"

grid_t init_grid(uint32_t GridSize)
{

  grid_t grid;
  uint64_t i;

  grid = malloc(sizeof(struct grid_struct));
  if (grid == NULL)
  {
    fprintf(stderr, "Could not allocate memory for the grid struct\n");
    exit(EXIT_FAILURE);
  }

  // General Information //
  grid->GridSize = GridSize;
  grid->NumCellsTotal = (uint64_t) GridSize * GridSize * GridSize;

  // Property Arrays // 
  grid->density = malloc(sizeof(double) * grid->NumCellsTotal);
  if (grid->density == NULL)
  {
    fprintf(stderr, "Could not allocate memory for the density of the grid\n");
    exit(EXIT_FAILURE);
  }

  grid->vx = malloc(sizeof(double) * grid->NumCellsTotal);
  if (grid->vx == NULL)
  {
    fprintf(stderr, "Could not allocate memory for vx for the grid\n");
    exit(EXIT_FAILURE);
  }

  grid->vy = malloc(sizeof(double) * grid->NumCellsTotal);
  if (grid->vy == NULL)
  {
    fprintf(stderr, "Could not allocate memory for vy for the grid\n");
    exit(EXIT_FAILURE);
  }

  grid->vz = malloc(sizeof(double) * grid->NumCellsTotal);
  if (grid->vz == NULL)
  {
    fprintf(stderr, "Could not allocate memory for vz for the grid\n");
    exit(EXIT_FAILURE);
  }

  // Initialize all the arrays //
  for (i = 0; i < grid->NumCellsTotal; ++i)
  {
    grid->density[i] = 0.0;
    grid->vx[i] = 0.0;
    grid->vy[i] = 0.0;
    grid->vz[i] = 0.0;
  }    

  return grid;  
}

void free_localgrid(grid_t *grid)
{

  free((*grid)->vz);  
  free((*grid)->vy);  
  free((*grid)->vx);  
  free((*grid)->density);
  free((*grid));
}
