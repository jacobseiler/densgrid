#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>

#include "io.h"
#include "io_hdf5.h"

// Proto-types //
void read_header(char *fname, hdf5_header file_header);


// Functions //

// This function accepts an empty (but malloced) header struct to be read and filled.
void read_header(char *fname, hdf5_header file_header)
{
    
  if (file_header == NULL)
  {
    fprintf(stderr, "read_header was called but the header struct has not yet been malloced\n");
    exit(EXIT_FAILURE);
  }

  read_attribute_int_array(fname, "/Header", "NumPart_ThisFile", file_header->NumPart_ThisFile, 6);
  read_attribute_int_array(fname, "/Header", "NumPart_Total", file_header->NumPart_Total, 6);
  read_attribute_int_array(fname, "/Header", "NumPart_Total_HighWord", file_header->NumPart_Total_HighWord, 6);
  read_attribute_int(fname, "/Header", "NumFilesPerSnapshot", &(file_header->NumFilesPerSnapshot));
 
  read_attribute_double_array(fname, "/Header", "MassTable", file_header->MassTable, 6);
#ifdef BRITTON_SIM
  file_header->MassTable[5] = 0.47143176; // For some reason Britton's Simulation does not contain the mass of PartType5 in the header.
#endif

  read_attribute_double(fname, "/Header", "Time", &(file_header->Time));
  read_attribute_double(fname, "/Header", "Redshift", &(file_header->Redshift));
  read_attribute_double(fname, "/Header", "BoxSize", &(file_header->BoxSize));
  read_attribute_double(fname, "/Header", "Omega0", &(file_header->Omega0));
  read_attribute_double(fname, "/Header", "OmegaLambda", &(file_header->OmegaLambda));
  read_attribute_double(fname, "/Header", "HubbleParam", &(file_header->HubbleParam));

  read_attribute_int(fname, "/Header", "Flag_Sfr", &(file_header->Flag_Sfr));
  read_attribute_int(fname, "/Header", "Flag_Cooling", &(file_header->Flag_Cooling));
  read_attribute_int(fname, "/Header", "Flag_StellarAge", &(file_header->Flag_StellarAge));
  read_attribute_int(fname, "/Header", "Flag_DoublePrecision", &(file_header->Flag_DoublePrecision));
   
}

int32_t get_numfiles(char *finbase)
{

  int32_t numfiles;

  char buf[1024];
  hdf5_header file_header; 

  snprintf(buf, 1024, "%s.0.hdf5", finbase);
  printf("Reading header from file %s\n", buf);

  file_header = malloc(sizeof(struct hdf5_header_struct));
  if (file_header == NULL)
  {
    fprintf(stderr, "Could not allocate memory for header\n");
    exit(EXIT_FAILURE);
  }

  read_header(buf, file_header); // Passes the header struct to be filled.
  numfiles = file_header->NumFilesPerSnapshot; 
  free(file_header);
 
  return numfiles; 

}


