#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>

#ifdef MPI
#include <mpi.h>
#endif

#include "grid.h"
#include "io.h"

void malloc_grid(grid_t grid, int32_t GridSize)
{

  if (grid == NULL)
  {
    fprintf(stderr, "malloc_grid was called with a pointer that has not been malloced\n");
    exit(EXIT_FAILURE);
  } 

  // General Information //
  grid->GridSize = GridSize;
  grid->NumCellsTotal = (uint64_t) CUBE(GridSize); 
  uint64_t Ncells = CUBE(GridSize);

  // Property Arrays // 
  grid->density = malloc(sizeof(double) * Ncells); 
  if (grid->density == NULL)
  {
    fprintf(stderr, "Could not allocate memory for the density of the grid\n");
    exit(EXIT_FAILURE);
  }

  grid->vx = malloc(sizeof(double) * Ncells); 
  if (grid->vx == NULL)
  {
    fprintf(stderr, "Could not allocate memory for vx for the grid\n");
    exit(EXIT_FAILURE);
  }

  grid->vy = malloc(sizeof(double) * Ncells); 
  if (grid->vy == NULL)
  {
    fprintf(stderr, "Could not allocate memory for vy for the grid\n");
    exit(EXIT_FAILURE);
  }

  grid->vz = malloc(sizeof(double) * Ncells); 
  if (grid->vz == NULL)
  {
    fprintf(stderr, "Could not allocate memory for vz for the grid\n");
    exit(EXIT_FAILURE);
  }

}

void init_grid(grid_t local_grid)
{
  uint64_t i; 

  // Initialize all the arrays //
  for (i = 0; i < local_grid->NumCellsTotal; ++i)
  {
    local_grid->density[i] = 0.0;
    local_grid->vx[i] = 0.0;
    local_grid->vy[i] = 0.0;
    local_grid->vz[i] = 0.0;
  }    
}

void free_grid(grid_t *grid)
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
  int32_t grid_idx;

  Pos[0] = part_buffer->posx[part_idx];  
  Pos[1] = part_buffer->posy[part_idx];  
  Pos[2] = part_buffer->posz[part_idx];  

#ifdef BRITTON_SIM
  // For Britton's Simulation we only care about the high-resolution zoom-in region.
  // Hence discard any particles outside the region.
  
  int32_t dim;
  double bound_low = 775.0;
  double bound_high = 825.0;
  int32_t accept_particle = 1;

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

int32_t write_grids(char *foutbase, int32_t precision, grid_t local_grid)
{

  char outfile[1024];
  
  snprintf(outfile, 1024, "%s.dens.dat", foutbase);
  write_grid_to_file_double(outfile, local_grid->GridSize, local_grid->density);

  snprintf(outfile, 1024, "%s.vx.dat", foutbase);
  write_grid_to_file_double(outfile, local_grid->GridSize, local_grid->vx);
 
  snprintf(outfile, 1024, "%s.vy.dat", foutbase);
  write_grid_to_file_double(outfile, local_grid->GridSize, local_grid->vy);
 
  snprintf(outfile, 1024, "%s.vz.dat", foutbase);
  write_grid_to_file_double(outfile, local_grid->GridSize, local_grid->vz);

  return EXIT_SUCCESS;
}

// Small function to check that the input precision of other functions is valid.
int32_t check_precision(int32_t precision)
{
  if (precision == 0 || precision == 1 || precision == 2)
  {
    return EXIT_SUCCESS; // Acceptable precision format so continue.
  }
  else
  {
    fprintf(stderr, "Attempted to read a grid with an invalid format precision. The value entered was %d. Can only use 0 (integer), 1 (4 bit float), 2 (8 bit double)\n", precision);
    exit(EXIT_FAILURE); 
  }
  
  return EXIT_FAILURE;
}

int32_t read_grid_double(char *infile, int32_t GridSize, double *grid)
{

  FILE *read_file;  

  if (!(read_file = fopen(infile, "r")))
  {
    fprintf(stderr, "Can't open file %s for reading\n", infile);
    exit(EXIT_FAILURE);
  }
 
  check_file_size(read_file, CUBE(GridSize)*8, infile);    
  fread(grid, sizeof(double), CUBE(GridSize), read_file);

  printf("Successfully read from grid %s\n", infile);
  return EXIT_SUCCESS;
 
}


int32_t write_grid_to_file_double(char *outfile, int32_t GridSize, double *variable)
{

  FILE *save_file = NULL;
 
  if (!(save_file = fopen(outfile, "w")))
  {
    fprintf(stderr, "Can't open file %s for writing\n", outfile);
    exit(EXIT_FAILURE);
  }

  fwrite(variable, sizeof(double), CUBE(GridSize), save_file);

  fclose(save_file);  
  printf("Sucessfully wrote to %s\n", outfile);  

  return EXIT_SUCCESS;
}

#ifdef MPI
grid_t MPI_grid_normalize(int32_t ThisTask, int32_t GridSize, grid_t local_grid)
{

  grid_t master_grid;
  master_grid = malloc(sizeof(struct grid_struct));

  if (ThisTask == 0)
  {
    malloc_grid(master_grid, GridSize);
    init_grid(master_grid);
  }

  MPI_Reduce(local_grid->density, master_grid->density, local_grid->NumCellsTotal, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); 
  MPI_Reduce(local_grid->vx, master_grid->vx, local_grid->NumCellsTotal, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); 
  MPI_Reduce(local_grid->vy, master_grid->vy, local_grid->NumCellsTotal, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); 
  MPI_Reduce(local_grid->vz, master_grid->vz, local_grid->NumCellsTotal, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); 

  return master_grid;

}
#endif

// This function add the properties of grid1 into grid2.

int32_t add_grids(grid_t grid1, grid_t grid2)
{

  uint64_t cell_idx;

  if (grid1->GridSize != grid2->GridSize)
  {
    fprintf(stderr, "Attempting to add two grids together that have different sizes. Grid 1 has a size %d and grid 2 has a size %d\n", grid1->GridSize, grid2->GridSize);
    exit(EXIT_FAILURE);
  }

  for (cell_idx = 0; cell_idx < grid1->NumCellsTotal; ++cell_idx)
  {
    grid1->density[cell_idx] += grid2->density[cell_idx];
    grid1->vx[cell_idx] += grid2->vx[cell_idx];
    grid1->vy[cell_idx] += grid2->vy[cell_idx];
    grid1->vz[cell_idx] += grid2->vz[cell_idx];
  }
    
  return EXIT_SUCCESS;
}
