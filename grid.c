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

void place_particle(int32_t part_idx, part_t part_buffer, grid_t local_grid, double BoxSize)
{

  double Pos[3];
  double Vel[3];
  int32_t grid_idx, dim;

  Pos[0] = part_buffer->posx[part_idx];  
  Pos[1] = part_buffer->posy[part_idx];  
  Pos[2] = part_buffer->posz[part_idx];  

#ifdef BRITTON_SIM
  // For Britton's Simulation we only care about the high-resolution zoom-in region.
  // Hence discard any particles outside the region.

  double bound_low = 775.0;
  double bound_high = 825.0;
  int accept_particle = 1;

  for(dim = 0; dim < 3; ++dim)
  {
    if(Pos[dim] <= bound_low || Pos[dim] >= bound_high)
    {
      accept_particle = 0;
    }
  }

  if (accept_particle == 0)
  {
    return;
  }
#endif  
 
  Vel[0] = part_buffer->vx[part_idx];
  Vel[1] = part_buffer->vy[part_idx];
  Vel[2] = part_buffer->vz[part_idx];

  grid_idx = determine_1D_idx(Pos, local_grid->GridSize, BoxSize); // Convert 3D x,y,z to 1D grid index.

  // Now we have the grid location, add the properties to the grid. // 
  local_grid->density[grid_idx] += part_buffer->mass[part_idx];
  local_grid->vx[grid_idx] += part_buffer->vx[part_idx];
  local_grid->vy[grid_idx] += part_buffer->vy[part_idx];
  local_grid->vz[grid_idx] += part_buffer->vz[part_idx];

}

int32_t determine_1D_idx(double *Pos, int32_t GridSize, double BoxSize)
{

  int32_t grid_x, grid_y, grid_z, grid_1D;

#ifdef BRITTON_SIM
  double bound_low = 775.0;
  double BoxSize_Britton = 50.0;

  grid_x = (Pos[0] - bound_low) * GridSize / BoxSize_Britton; // Get the corresponding x, y, z indices.    
  grid_y = (Pos[1] - bound_low) * GridSize / BoxSize_Britton;
  grid_z = (Pos[2] - bound_low) * GridSize / BoxSize_Britton; 

#else

  grid_x = Pos[0] * GridSize / BoxSize; // Get the corresponding x, y, z indices.    
  grid_y = Pos[1] * GridSize / BoxSize;
  grid_z = Pos[2] * GridSize / BoxSize;
#endif

  grid_1D = grid_x * GridSize * GridSize + grid_y * GridSize + grid_z;  

  if (grid_1D < 0 || grid_1D > GridSize * GridSize * GridSize)
  {
    fprintf(stderr, "The grid index is %d.  This is either negative or beyond the number of grid cells available (%ld).\n", grid_1D, (uint64_t) GridSize*GridSize*GridSize);
    fprintf(stderr, "The position of the particle is x = %.4f \t y = %.4f \t z = %.4f\n", Pos[0], Pos[1], Pos[2]);
    exit(EXIT_FAILURE);
  }

  return grid_1D;

}

void normalize_density(grid_t local_grid)
{

  double total_mass, rho_norm;
  uint64_t cell_idx;
  double min_rho = 1.0e10;
  double max_rho = -1.0e10;
  

  for (cell_idx = 0, total_mass = 0; cell_idx < local_grid->NumCellsTotal; ++cell_idx)
  {
    total_mass += local_grid->density[cell_idx];
  }

  rho_norm = total_mass / local_grid->NumCellsTotal;

  printf("The total mass of particles within the grid is %.4e (1.0e10 Msun). The homogenous density is %.4e (1.0e10 Msun) per cell.\n", total_mass, rho_norm);

  for (cell_idx = 0; cell_idx < local_grid->NumCellsTotal; ++cell_idx)
  {
    local_grid->density[cell_idx] /= rho_norm; 

    if (local_grid->density[cell_idx] < min_rho)
    {
      min_rho = local_grid->density[cell_idx];
    }
    
    if (local_grid->density[cell_idx] > max_rho)
    {
      max_rho = local_grid->density[cell_idx];
    }
  }

  printf("Density grid has been normalized. Minimum (normalized) density is %.4e and maximum is %.4e\n", min_rho, max_rho);
}

void write_grids(char *foutbase, grid_t local_grid)
{

  char outfile[1024];

  snprintf(outfile, 1024, "%s_density", foutbase);
  write_grid_to_file(outfile, local_grid->GridSize, local_grid->density); 
 
}

void write_grid_to_file(char *outfile, int32_t GridSize, double *variable)
{

  FILE *save_file = NULL;

  if (!(save_file = fopen(outfile, "w")))
  {
    fprintf(stderr, "Can't open file %s for writing\n", outfile);
    exit(EXIT_FAILURE);
  }

  fwrite(variable, sizeof(double), GridSize*GridSize*GridSize, save_file);
  fclose(save_file);
  
  printf("Sucessfully wrote to %s\n", outfile);  


}

