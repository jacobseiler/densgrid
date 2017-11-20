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
  int32_t type_idx, part_idx;
  uint64_t TotalPart_ThisFile;
  uint64_t NumPart_ThisFile_ThisType, particle_buffer_offset; 

  // HDF5 Reading Stuff //
  uint64_t *pointer_long;
  float *pointer_float;
  int32_t hdf5_count, hdf5_offset;
  int32_t array_hdf5_count[2], array_hdf5_offset[2];

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

  // Now all the arrays have been allocated, lets fill them with the particle data.

  particle_buffer_offset = 0;
  for (type_idx = 0; type_idx < 6; ++type_idx)
  {
    
    if (particle_buffer->NumParticles_Total[type_idx] == 0) // If there are no particles of this type within the file, continue.
    {
      continue;
    }

    hdf5_count = particle_buffer->NumParticles_Total[type_idx]; // Reading all the particles of each type.
    hdf5_offset = 0; // We read all the data from the HDF5 file so there's no offset.

    array_hdf5_count[0] = hdf5_count;
    array_hdf5_count[1] = 3;
    array_hdf5_offset[0] = 0;
    array_hdf5_offset[1] = 0;

    // Particle IDs //
    snprintf(dataset, 1024, "/PartType%d/ParticleIDs", type_idx);   
    pointer_long = malloc(sizeof(uint64_t) * hdf5_count); 
     
#ifdef DEBUG_PARTBUFFER 
    printf("Type_idx = %d \t Trying to read %d ParticlesIDs from file %s\n", type_idx, (int) (particle_buffer->NumParticles_Total[type_idx]), fname);
#endif 

    read_dataset_long(fname, dataset, 1, array_hdf5_offset, array_hdf5_count, pointer_long);

    for (part_idx = 0; part_idx < hdf5_count; ++part_idx)
    {
      particle_buffer->ID[part_idx + particle_buffer_offset] = pointer_long[part_idx]; // Since we are keeping particles of multiple type in one array we need to use an offset to properly fill each subsequent particle type. 
    }
    printf("Particle IDs all read and stored properly\n");

    
    // Particle Mass //    
    for (part_idx = 0; part_idx < hdf5_count; ++part_idx)
    {
      particle_buffer->mass[part_idx + particle_buffer_offset] = file_header->MassTable[type_idx]; // Since we are keeping particles of multiple type in one array we need to use an offset to properly fill each subsequent particle type. 
    }
    printf("Particle mass all stored properly\n"); 


    // Particle Positions //     
    snprintf(dataset, 1024, "/PartType%d/Coordinates", type_idx);   

    pointer_float = malloc(sizeof(float) * hdf5_count * 3); 
#ifdef DEBUG_PARTBUFFER 
    printf("Type_idx = %d \t Trying to read %d Positions from file %s\n", type_idx, (int) (particle_buffer->NumParticles_Total[type_idx]), fname);
#endif 
    
    read_dataset_float(fname, dataset, 2, array_hdf5_offset, array_hdf5_count, pointer_float);
    

    for (part_idx = 0; part_idx < hdf5_count; ++part_idx)
    {
      particle_buffer->posx[part_idx + particle_buffer_offset] = pointer_float[part_idx*3]; // All the entries are stores in blocks of length 3.
      particle_buffer->posy[part_idx + particle_buffer_offset] = pointer_float[(part_idx*3) + 1];
      particle_buffer->posz[part_idx + particle_buffer_offset] = pointer_float[(part_idx*3) + 2];
   
    }
    printf("Particle positions all read and stored properly\n");

    // Particle Velocities //     
    snprintf(dataset, 1024, "/PartType%d/Velocities", type_idx);   

    // Note: Pointer float has already been malloced for the particle positions.
#ifdef DEBUG_PARTBUFFER 
    printf("Type_idx = %d \t Trying to read %d Positions from file %s\n", type_idx, (int) (particle_buffer->NumParticles_Total[type_idx]), fname);
#endif 
    
    read_dataset_float(fname, dataset, 2, array_hdf5_offset, array_hdf5_count, pointer_float);
    

    for (part_idx = 0; part_idx < hdf5_count; ++part_idx)
    {
      particle_buffer->vx[part_idx + particle_buffer_offset] = pointer_float[part_idx*3]; // All the entries are stores in blocks of length 3.
      particle_buffer->vy[part_idx + particle_buffer_offset] = pointer_float[(part_idx*3) + 1];
      particle_buffer->vz[part_idx + particle_buffer_offset] = pointer_float[(part_idx*3) + 2];
   
    }
    printf("Particle velocities all read and stored properly\n");

/*
struct particle_struct
{
  uint64_t NumParticles_Total[6];
 
  uint64_t *ID;
  double *mass;

  double *posx;
  double *posy;
  double *posz;

  double *vx;
  double *vy;
  double *vz;

}; 
*/

#ifdef DEBUG_PARTBUFFER
    printf("Particle 100 has the following properties: \t ID %ld \t posx %.4f \t posy %.4f \t posz %.4f \t vx %.4f \t vy %.4f \t vz %.4f\n", particle_buffer->ID[100], particle_buffer->posx[100], particle_buffer->posy[100], particle_buffer->posz[100], particle_buffer->vx[100], particle_buffer->vy[100], particle_buffer->vz[100]);
#endif 

    particle_buffer_offset += hdf5_count;

#ifdef DEBUG_PARTBUFFER
    printf("Read them in\n");
#endif 

    free(pointer_long);
    free(pointer_float);
  } // Loop for reading each particle type.  

  free(file_header);
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


