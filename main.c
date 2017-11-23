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

#define	 CUBE(x) (x*x*x)

// Local Variables //
int32_t GridSize;
char *finbase;
char *foutbase;
int32_t num_files;
double BoxSize;

#ifdef MPI
int ThisTask, NTask, nodeNameLen;
char *ThisNode;
#endif

// Proto-types //

void parse_params(int argc, char **argv);
void init();

// Functions //

void bye()
{
#ifdef MPI
  MPI_Finalize();
  free(ThisNode);
#endif
}

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
    exit(EXIT_FAILURE);
  }
  finbase = strdup(argv[2]);
  foutbase = strdup(argv[3]);
  printf("==================================================\n");
  printf("Running with a GridSize %d, input file base %s and output file base %s\n", GridSize, finbase, foutbase); 
  printf("==================================================\n\n");
}

void init()
{

  get_header_params(finbase, &num_files, &BoxSize);

}

int main(int argc, char **argv)
{

  grid_t local_grid; 
  int32_t file_idx, part_idx;

#ifdef MPI
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &ThisTask);
  MPI_Comm_size(MPI_COMM_WORLD, &NTask);

  ThisNode = malloc(MPI_MAX_PROCESSOR_NAME * sizeof(char));

  MPI_Get_processor_name(ThisNode, &nodeNameLen);
  if (nodeNameLen >= MPI_MAX_PROCESSOR_NAME)
  {
    printf("Node name string not long enough!...\n");
    exit(EXIT_FAILURE);
  }
#endif

  atexit(bye);
 
  parse_params(argc, argv); // Set the input parameters. 

  init(); // Grab a few parameters from the header file.

  local_grid = malloc(sizeof(struct grid_struct));
  malloc_grid(local_grid, GridSize);
  init_grid(local_grid);  // Zero the grid cells. 

  printf("We are reading from %d files\n", num_files); 
  num_files = 1;

#ifdef MPI
  for (file_idx = ThisTask; file_idx < num_files; file_idx += NTask)
#else
  for (file_idx = 0; file_idx < num_files; ++file_idx)
#endif
  {
#ifdef MPI
    printf("I am Task %d and I am reading file %d\n", ThisTask, file_idx);
#else
    printf("Reading from file %d\n", file_idx);
#endif
    part_t particles_local;
    particles_local = malloc(sizeof(struct particle_struct));

    if (particles_local == NULL)
    {
      fprintf(stderr, "Could not allocate memory for local particles for file number %d\n", file_idx); 
      exit(EXIT_FAILURE);
    }

    fill_particles(finbase, file_idx, particles_local);

    for (part_idx = 0; part_idx < particles_local->NumParticles_Total_AllType; ++part_idx)
    {
      if (part_idx % 10000000 == 0)
      {
          printf("Gridding particle %d\n", part_idx);
      }
      place_particle(part_idx, particles_local, local_grid, BoxSize);
      // TODO: Implement a TSC.
    }

    free_localparticles(&particles_local);
  }

  
#ifdef MPI
  // If we are running in parallel, want to sum all the grids back onto the master process before normalizing only the master grid.

  printf("Now summing all grids on Task 0 and then normalizing\n");
  grid_t master_grid; 

  master_grid = MPI_grid_normalize(ThisTask, GridSize, local_grid); // Sum all the grids back onto Task0.

  if (ThisTask == 0)
  {    
    normalize_density(master_grid); // Only normalize the master grid.
    write_grids(foutbase, 2, master_grid);
    free_grid(&master_grid); // Only Task0 had master_grid malloced.
  }
#else
  printf("Now normalizing the density grid\n");
  normalize_density(local_grid);
  write_grids(foutbase, 2, local_grid);
#endif 
 
  free_grid(&local_grid);
  free(finbase);
  free(foutbase); 
  return 0;  
} 
