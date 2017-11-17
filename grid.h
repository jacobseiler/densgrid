#ifndef GRID_H
#define GRID_H

#include <stdint.h>

struct grid_struct
{
  uint32_t GridSize; 
  uint64_t NumCellsTotal;
  double *density;
  double *vx;
  double *vy;
  double *vz;

}; 

typedef struct grid_struct *grid_t;

// Proto-Types //
grid_t init_grid(uint32_t GridSize);
void free_localgrid(grid_t *grid);
#endif
