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

#include "io.h"
#include "grid.h"
#include "particles.h"

// Local Variables //
uint32_t GridSize;
char *finbase;
char *foutbase;
int32_t num_files;
double BoxSize;

// Proto-types //

void parse_params(int argc, char **argv);
void init(grid_t *grid);

// Functions //

void parse_params(int argc, char **argv)
{

  if (argc != 4)
  {
    fprintf(stderr, "This is my version of densfield, tailored for usage with GADGET HDF5 files where a single file can (comfortably) fit in RAM.\n");
    fprintf(stderr, "Usage : ./densgrid <GridSize> <Input Snapshot Base> <Output Base>\n");
    fprintf(stderr, "The name format for the input snapshot is assumed to be '<Input Snapshot Base>.%%file_index%%.hdf5'\n");
    exit(0);
  }

  GridSize = atoi(argv[1]);
  if (GridSize < 64 || GridSize > 2048)
  {
    fprintf(stderr, "The GridSize must be between 64 and 2048\n"); 
    exit(0);
  }
  finbase = strdup(argv[2]);
  foutbase = strdup(argv[3]);
  printf("==================================================\n");
  printf("Running with a GridSize %d, input file base %s and output file base %s\n", GridSize, finbase, foutbase); 
  printf("==================================================\n\n");
}

void init(grid_t *grid)
{

  *grid = init_grid(GridSize);  
  get_header_params(finbase, &num_files, &BoxSize);


}

int main(int argc, char **argv)
{

  grid_t grid_local;
  int32_t file_idx, part_idx;

  parse_params(argc, argv); // Set the input parameters. 
  init(&grid_local); // Initialize parameters. 

  printf("We are reading from %d files\n", num_files); 

  for (file_idx = 0; file_idx < 1; ++file_idx)
  {
    printf("Reading file %d\n", file_idx);

    part_t particles_local;
    particles_local = malloc(sizeof(struct particle_struct));


    if (particles_local == NULL)
    {
      fprintf(stderr, "Could not allocate memory for local particles for file number %d\n", file_idx); 
      exit(EXIT_FAILURE);
    }

    fill_particles(finbase, file_idx, particles_local);

    // Get particles
    // // Create a particle struct that holds the particles.
    // // Malloc the struct and pass it to a function to be filled. Return the number of particles.
    // DONE

    for (part_idx = 0; part_idx < particles_local->NumParticles_Total_AllType; ++part_idx)
    {
      if (part_idx % 1000000 == 0)
      {
          printf("Gridding particle %d\n", part_idx);
      }
      place_particle(part_idx, particles_local, grid_local, BoxSize);
    }

    // For each particle determine the affected cells
    // Place particle into cell
    // // Loop over the number of particles (returned from filling function) and do this for each particle.
    // DONE (For particle-in-cell, need to update for TSC).

    free_localparticles(&particles_local);
  }
  // Once all files have been filled, go back and normalize all of the arrays. 

  free_localgrid(&grid_local);
  free(finbase);
  free(foutbase); 
  return 0;
  
} 
