#ifndef GRID_H
#define GRID_H

#define	 CUBE(x) (x*x*x)

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
void malloc_grid(grid_t grid, int32_t GridSize);
void init_grid(grid_t local_grid);
void free_grid(grid_t *grid);
void place_particle(int32_t part_idx, part_t particle_buffer, grid_t local_grid, double BoxSize);
int32_t determine_1D_idx(double *Pos, int32_t GridSize, double BoxSize);
void normalize_density(grid_t local_grid);
int32_t write_grids(char *foutbase, int32_t precison, grid_t local_grid);
int32_t write_grid_to_file_double(char *outfile, int32_t GridSize, double *variable);
int32_t read_grid_double(char *infile, int32_t GridSize, double *grid);
int32_t add_grids(grid_t grid1, grid_t grid2);
int32_t check_precision(int32_t precision);

#ifdef MPI
grid_t MPI_grid_normalize(int32_t ThisTask, int32_t GridSize, grid_t local_grid); 
#endif

#endif
