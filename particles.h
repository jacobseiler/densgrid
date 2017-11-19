#ifndef PARTICLES_H
#define PARTICLES_H

#include <stdint.h>

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

typedef struct particle_struct *part_t;

// Proto-Types //
void fill_particles(char *finbase, int file_idx, part_t particle_buffer);
void free_localparticles(part_t *part_buffer);
#endif
