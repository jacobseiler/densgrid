#ifndef IO_H 
#define IO_H 

#include <stdint.h>

#define MAXLEN 1024;
struct hdf5_header_struct
{

  uint32_t numpart_thisfile[6];
  uint32_t numpart_total[6];
  uint32_t numpart_highword[6];
  double massarray[6];
  double Time;
  double Redshift;
  double BoxSize;
  int32_t NumFilesPerSnapshot;
  double Omega0;
  double OmegaLambda;
  double HubbleParam;
  uint32_t Flag_Sfr;
  uint32_t Flag_Cooling;
  uint32_t Flag_StellarAge;
  uint32_t Flag_DoublePrecision;

};

typedef struct hdf5_header_struct *hdf5_header;

// Proto-Types //
uint32_t get_numfiles(char *finbase); 

#endif
