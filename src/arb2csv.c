// https://github.com/gnbl/csv2arb
// 
// Convert <input> 16 bit binary data to text <outfile> with one value per line.
// Detects and removes Point marker bit from data.
// Tested with Rigol DS1000Z-S series built-in arbitrary function generator 
// 
//  Input range:   0   ... 16383  (14 bit)
//  Output range: -1.0 ... 1.0
// 
// Compile on Windows using MinGW
//  - Install MinGW http://sourceforge.net/projects/mingw/files/Installer/
//  - Add <INSTALLPATH>/MinGW/bin/ to system PATH (to avoid a dialog window with this error message: "The program can't start because libgmp-10.dll is missing from your computer.")
//  - run "gcc arb2csv.c -o arb2csv.exe"


// Based on https://github.com/MatCat/csv2arb but largely modified.


#include <stdio.h>
#include <stdlib.h>

#include "stdint.h"


// Input file value options (for Rigol DS1000Z-S)
#define SAMPLES   16384L
#define DACBITS   14
#define FILEBYTES 2

#define OUTMAX  ((1<<DACBITS)-1)
#define OUTZERO (OUTMAX/2)
#define OUTMIN  0x0000

// Output (normalized)
#define INMIN -1.0
#define INMAX  1.0


// map value to DAC range
float              // 
map(
  long x,        // 
  long in_min,   // 
  long in_max,   // 
  float out_min,   // 
  float out_max    // 
  )
{
  if(x > in_max)
  {
    printf("WARNING: value out of range (%d > %d). The input should be normalized.\n", x, in_max);
  }
  if(x < in_min)
  {
    printf("WARNING: value out of range (%d < %d). The input should be normalized.\n", x, in_min);
  }
  
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void              // 
printHelp(
  char *argv[]    // 
  )
{
  printf("Usage: %s <input> <output>\n",argv[0]);
}

int               // 
main(
  int argc,       // 
  char *argv[]    // 
  )
{
  if (argc < 3)
  {
    printf("ARB to CSV Converter\n\nThis program is free and open source (https://github.com/gnbl/csv2arb, based on csv2arb by MatCat)\n");
    printf("\nPlease specify an input and output file.\n");
    printHelp(argv);
  }
  else
  {
    // open files
    printf("\nInput File: %s\n",argv[1]);
    FILE *ifp, *ofp;
    char *mode = "rb";
    char *oMode = "w";
    
    ifp = fopen(argv[1], mode);
    ofp = fopen(argv[2], oMode);
    
    if( ifp == NULL )
    {
      fprintf(stderr, "Error opening %s\n", argv[1]);
      exit(1);
    }
    if( ofp == NULL )
    {
      fprintf(stderr, "Error opening %s\n", argv[2]);
      exit(1);
    }
    
    // read
    uint16_t data[SAMPLES];
    fread( &data, sizeof(data), 1, ifp);
    fclose(ifp);
    
    // convert and write
    int n;
    for( n=0; n < SAMPLES; n++)
    {
      
      uint16_t d = ( data[n] );
      
      // Detect and remove Point marker bit
      if( d & 0x8000 )
      {
        printf("Sample %d is a point\n", n);
        d = d & ~0x8000;
      }
      
      // map DAC bits to values
      float oValue = map(  d,
                          OUTMIN, OUTMAX, // out
                           INMIN,  INMAX  // in
                          );
      
      // write ASCII float to output file
      fprintf(ofp, "%f\n", oValue);
    }
    
    // close file
    fclose(ofp);
  }
  return 1;
}
