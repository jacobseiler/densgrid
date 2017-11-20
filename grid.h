#ifndef GRID_H
#define GRID_H

#include <stdint.h>
#include "particles.h"

struct grid_struct
{
  int32_t GridSize;   
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
void place_particle(int32_t part_idx, part_t particle_buffer, grid_t local_grid, double BoxSize);
int32_t determine_1D_idx(double *Pos, int32_t GridSize, double BoxSize);
void normalize_density(grid_t local_grid);
void write_grids(char *foutbase, grid_t local_grid);
void write_grid_to_file(char *outfile, int32_t GridSize, double *variable); 
#endif
