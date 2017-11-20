#ifndef PARTICLES_H
#define PARTICLES_H

#include <stdint.h>
#include "grid.h"

struct particle_struct
{
  uint64_t NumParticles_Total[6];
  uint64_t NumParticles_Total_AllType;
 
  uint64_t *ID;
  double *mass;

  double *posx;
  double *posy;
  double *posz;

  double *vx;
  double *vy;
  double *vz;

}; 

typedef struct particle_struct *part_t;

// Proto-Types //
void fill_particles(char *finbase, int32_t file_idx, part_t particle_buffer);
void free_localparticles(part_t *part_buffer);
void place_particle(int32_t part_idx, part_t particle_buffer, grid_t local_grid, double BoxSize);
int32_t determine_1D_idx(double *Pos, int32_t GridSize, double BoxSize);
#endif
