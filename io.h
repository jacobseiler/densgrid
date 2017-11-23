#ifndef IO_H 
#define IO_H 

#include <stdint.h>

#define MAXLEN 1024;
struct hdf5_header_struct
{

  int32_t NumPart_ThisFile[6];
  uint32_t NumPart_Total[6];
  int32_t NumPart_Total_HighWord[6];
  double MassTable[6];
  double Time;
  double Redshift;
  double BoxSize;
  int32_t NumFilesPerSnapshot;
  double Omega0;
  double OmegaLambda;
  double HubbleParam;
  int32_t Flag_Sfr;
  int32_t Flag_Cooling;
  int32_t Flag_StellarAge;
  int32_t Flag_DoublePrecision;

};
	 
typedef struct hdf5_header_struct *hdf5_header;

// Proto-Types //
void get_header_params(char *finbase, int32_t *num_files, double *BoxSize); 
void read_header(char *fname, hdf5_header file_header);
void check_file_size(FILE *file, uint64_t expected_size, char *fname);
#endif
