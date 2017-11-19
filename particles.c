#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>

#include "particles.h"
#include "io.h"
#include "io_hdf5.h"

void fill_particles(char *finbase, int file_idx, part_t particle_buffer)
{

  char fname[1024], dataset[1024];
  hdf5_header file_header; 
  int32_t type_idx;
  uint64_t TotalPart_ThisFile;
  uint64_t NumPart_ThisFile_ThisType, *pointer_long; 

  snprintf(fname, 1024, "%s.%d.hdf5", finbase, file_idx);

  file_header = malloc(sizeof(struct hdf5_header_struct));
  if (file_header == NULL)
  {
    fprintf(stderr, "Could not allocate memory for header in fill_particles for file %d\n", file_idx);
    exit(EXIT_FAILURE);
  }

  read_header(fname, file_header); // Passes the header struct to be filled.

  for (type_idx = 0, TotalPart_ThisFile = 0; type_idx < 6; ++type_idx)
  {

    NumPart_ThisFile_ThisType = file_header->NumPart_ThisFile[type_idx];

    printf("PartType %d, NumPart_ThisFile %d \t HighWord %d\n", type_idx, file_header->NumPart_ThisFile[type_idx], file_header->NumPart_Total_HighWord[type_idx]);
    if (NumPart_ThisFile_ThisType > 3.0e9) // Let's set a cap of ~100GB per file chunk.
    {
      fprintf(stderr, "This file has %ld particles. We have capped the max out to be 3.0e9 to roughly correspond to ~100GB per file chunk.\n", NumPart_ThisFile_ThisType);
      exit(EXIT_FAILURE);
    }

    TotalPart_ThisFile += NumPart_ThisFile_ThisType; 
    particle_buffer->NumParticles_Total[type_idx] = NumPart_ThisFile_ThisType;

  }  

  printf("The number of particles in this file is %ld\n", TotalPart_ThisFile);

  // Allocating Memory to fit all these particles. //

  // TODO: Keep a running check of the amount of memory allocated.

  particle_buffer->ID = malloc(sizeof(uint64_t) * TotalPart_ThisFile);
  if (particle_buffer->ID == NULL)
  {
    fprintf(stderr, "Could not allocate memory for the particle IDs\n"); 
    exit(EXIT_FAILURE);
  }
 
  particle_buffer->mass = malloc(sizeof(double) * TotalPart_ThisFile);
  if (particle_buffer->mass == NULL)
  {
    fprintf(stderr, "Could not allocate memory for the particle mass\n"); 
    exit(EXIT_FAILURE);
  }

  particle_buffer->posx = malloc(sizeof(double) * TotalPart_ThisFile);
  if (particle_buffer->posx == NULL)
  {
    fprintf(stderr, "Could not allocate memory for the particle posx\n"); 
    exit(EXIT_FAILURE);
  }

  particle_buffer->posy = malloc(sizeof(double) * TotalPart_ThisFile);
  if (particle_buffer->posy == NULL)
  {
    fprintf(stderr, "Could not allocate memory for the particle posy\n"); 
    exit(EXIT_FAILURE);
  }

  particle_buffer->posz = malloc(sizeof(double) * TotalPart_ThisFile);
  if (particle_buffer->posz == NULL)
  {
    fprintf(stderr, "Could not allocate memory for the particle posz\n"); 
    exit(EXIT_FAILURE);
  }

  particle_buffer->vx = malloc(sizeof(double) * TotalPart_ThisFile);
  if (particle_buffer->vx == NULL)
  {
    fprintf(stderr, "Could not allocate memory for the particle vx\n"); 
    exit(EXIT_FAILURE);
  }

  particle_buffer->vy = malloc(sizeof(double) * TotalPart_ThisFile);
  if (particle_buffer->vy == NULL)
  {
    fprintf(stderr, "Could not allocate memory for the particle vy\n"); 
    exit(EXIT_FAILURE);
  }

  particle_buffer->vz = malloc(sizeof(double) * TotalPart_ThisFile);
  if (particle_buffer->vz == NULL)
  {
    fprintf(stderr, "Could not allocate memory for the particle vz\n"); 
    exit(EXIT_FAILURE);
  }

  free(file_header);

  // Now all the arrays have been allocated, lets fill them with the particle data.

  for (type_idx = 0; type_idx < 6; ++type_idx)
  {
    
    if (particle_buffer->NumParticles_Total[type_idx] == 0) // If there are no particles of this type within the file, continue.
    {
      continue;
    }

    // Particle IDs //
    snprintf(dataset, 1024, "/PartType%d/ParticleIDs", type_idx);    
    pointer_long = malloc(sizeof(uint64_t) * particle_buffer->NumParticles_Total[type_idx]); 
 
    int count; 
    int offset;
    
    count = particle_buffer->NumParticles_Total[type_idx];
    offset = 0;

#ifdef DEBUG_PARTBUFFER 
    printf("Part_idx = %d \t Trying to read %d ParticlesIDs from file %s\n",part_idx, (int) (particle_buffer->NumParticles_Total[type_idx]), fname);
#endif 

    read_dataset_long(fname, dataset, 1, &offset, &count, pointer_long);

#ifdef DEBUG_PARTBUFFER
    printf("Read them in\n");
#endif 

    free(pointer_long);
  }

}

void free_localparticles(part_t *part_buffer)
{
 
  free((*part_buffer)->vz);  
  free((*part_buffer)->vy);  
  free((*part_buffer)->vx);  
  free((*part_buffer)->posz);  
  free((*part_buffer)->posy);  
  free((*part_buffer)->posx);  
  free((*part_buffer)->mass);  
  free((*part_buffer)->ID);  
  free((*part_buffer));
}


