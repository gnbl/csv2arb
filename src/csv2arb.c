// https://github.com/gnbl/csv2arb
// 
// Convert <input> text file with one value per line to 16 bit binary data in <outfile>.
// Use Point marker bit for input data and for the first and last Point in padding data (0 V) to bring filesize to 32768 bytes (16384 values).
// Tested with Rigol DS1000Z-S series built-in arbitrary function generator 
// 
//  Input range: -1.0 ... 1.0
//  Output range: 0   ... 16383  (14 bit)
// 
// Compile on Windows using MinGW
//  - Install MinGW http://sourceforge.net/projects/mingw/files/Installer/
//  - Add <INSTALLPATH>/MinGW/bin/ to system PATH (to avoid a dialog window with this error message: "The program can't start because libgmp-10.dll is missing from your computer.")
//  - run "gcc csv2arb.c -o csv2arb.exe"


// Original source: https://github.com/MatCat/csv2arb
// Original header:
/*
 *  This source code is free and open source.  You may use it as you
 *  see fit so long as this header comment stays put!
 *
 * Author MatCat
 */


#include <stdio.h>
#include <stdlib.h>

#include "stdint.h"


// input file values should be normalized
#define INMIN -1.0
#define INMAX  1.0

// Some output options (for Rigol DS1000Z-S)
#define SAMPLES   16384L
#define DACBITS   14
#define FILEBYTES 2

#define OUTMAX  ((1<<DACBITS)-1)
#define OUTZERO (OUTMAX/2)
#define OUTMIN  0x0000
#define PAD     OUTZERO


// looking at waveform data generated on the scope, I notices the following value:
#define DATAMASK  0x3FFF  // 0011 1111 1111 1111
#define POINT     0x8000  // 1000 0000 0000 0000


// map value to DAC range
long              // 
map(
  float x,        // 
  float in_min,   // 
  float in_max,   // 
  long out_min,   // 
  long out_max    // 
  )
{
  if(x > in_max)
  {
    printf("WARNING: value out of range (%f > %f). The input should be normalized.\n", x, in_max);
  }
  if(x < in_min)
  {
    printf("WARNING: value out of range (%f < %f). The input should be normalized.\n", x, in_min);
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


int
write(
  uint16_t data,
  FILE *fp
  )
{
  //unsigned int byteswritten = 0;
  
  putc( data & 0xFF, fp ); // lower byte first -> little endian
  putc( data >> 8,   fp ); // 
  
  return 2;
}


int               // 
main(
  int argc,       // 
  char *argv[]    // 
  )
{
  if (argc < 3)
  {
    printf("CSV to ARB Converter\n\nThis program is free and open source (https://github.com/gnbl/csv2arb).\n\n Originally written by MatCat\n");
    printf("\nPlease specify an input and output file.\n");
    printHelp(argv);
  }
  else
  {
    // open files
    printf("\nInput File: %s\n",argv[1]);
    FILE *ifp, *ofp;
    char *mode = "r";
    char *oMode = "wb";
    
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
    
    // read, convert, write all values
    char line[1000];
    int lineCount = 0;
    int charCount = 0;
    while( fgets(line, sizeof line, ifp) != NULL )
    {
      if( line[0] == '#' )
      {
        ; // ignore comment lines
      }
      else
      {
        lineCount++;
        
        // read ascii float
        char * pEnd;
        float iValue = strtof(line, &pEnd);
        
        // map input values to DAC bits
        long oValue = map(  iValue,
                             INMIN,  INMAX, // in
                            OUTMIN, OUTMAX  // out
                            );
        
        // Rigol software needs POINT marker bit
        oValue |= POINT;
        
        // write two bytes to output file
        charCount++;
        putc( oValue & 0xFF, ofp ); // lower byte first -> little endian
        charCount++;
        putc( oValue >> 8,   ofp ); // 
      }
    }
    printf("%d points, wrote %d bytes\n", lineCount, charCount);
    
    // padding
    // Rigol DS100Z-S won't load small(er) files
    unsigned int filesize = (SAMPLES*FILEBYTES);
    unsigned int rest = filesize - charCount;
    if( rest > 0 )  /// \todo handle padding for few bytes
    {
      printf("Padding file from %d bytes to %d bytes with %d zero values..\n", charCount, filesize, rest);
      int i;
      
      // point marker
      write( PAD | POINT , ofp );
      
      // fill data
      for( i = 0; i<(rest/2)-2; i++)
      {
        write( PAD , ofp );
      }
      // point marker
      write( PAD | POINT, ofp );
    }
    
    // close files
    fclose(ifp);
    fclose(ofp);
  }
  return 1;
}
