/* This is NOT the longest common substring program. This is a program to
   calculate the length of the Greatest Sequential Set of characters in two
   strings.
   The Greatest Sequential Subset of two strings is the longest possible
   subset of characters that can be constructed from both strings by removing
   any amount of letters from them (but not switching them around). Example:
   gss("abcdefghijk", "arcsforgloriouskings") = "acfgik"
   Finding the actual substring is harder and outside the scope of this program;
   this is only to calculate its length.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define DIM 1200 // last longest line encountered = 1145

typedef enum { LINEMODE, SINGLEMODE, DOUBLEMODE } prog_mode;
typedef unsigned int uint;

unsigned short array[DIM][DIM];




/* lcs_line: given a filename and string, perform a lcs on the string with each
   line of the file. The file will be read through with formatted I/O.
   threshold determines whether a match is long enough to be worth printing.
   use_exact, if specified, will match exact lines.
   Return value is 0 for success or 1 for a failure of some type.
*/
int gss_line(char* fname, char* str, uint threshold, bool use_exact) {
  FILE* fd = fopen(fname, "r");
  if(!fd) {
    fprintf(stderr, "Could not open file %s\n", fname);
    return 1;
  }

  int gss_len;
  uint filelinectr = 0;
  char str2[DIM];
  while(fgets(str2, DIM, fd) != NULL) {
    ++filelinectr;
    gss_len = gss(str, str2);
    if(1) {
      //printf("Line \x1B[31m%d\x1B[0m\n", filelinectr);
      print_gss_strings(str, str2, gss_len);
    }
  }
}


/* Argument handling function: given a string that should represent the
   threshold, attempt to parse it into a number and return it. If it fails,
   terminate the program. */
unsigned int get_threshold(char* arg) {
  char* strtol_test = NULL;
  unsigned int t;
  
  t = strtol(arg, &strtol_test, 10);
  if(*strtol_test) {
    fprintf(stderr,"Error: threshold argument %s could not be interpreted as a number\n", arg);
    exit(1);
  }
  return t;
}

void usage(char* argv0) {
  fprintf(stderr,"Usage: %s [-L2[e]] threshold file [file2]\n", argv0);
  fprintf(stderr," -L mode: read in a line and compare to all lines in file\n");
  fprintf(stderr," -2 mode: compare all lines in file with all lines in file 2\n");
  fprintf(stderr," No mode: compare all lines in file with itself\n");
  //fprintf(stderr," -e flag: show only exact matches\n");
}

int main(int argc, char* argv[]) {

  printf("%d\n", gss("abcdefghijk", "arcsforgloriouskings"));

  prog_mode mode;
  unsigned int threshold = 20;
  char* fname1; char* fname2;
  bool use_exact;
  
  if(argc < 3) {
    // minimum 3 arguments, argv[0] threshold file
    usage(argv[0]); return 1;
  }
  if(argv[1][0] == '-') {
    // options specified
    if(strchr(argv[1], 'L')) {
      if(argc != 4) {
	usage(argv[0]); return 1;
      }
      threshold = get_threshold(argv[2]);
      fname1 = argv[3];
      mode = LINEMODE;
    }
    else if (strchr(argv[1], '2')) {
      if(argc != 5) {
	usage(argv[0]); return 1;
      }
      threshold = get_threshold(argv[2]);
      fname1 = argv[3];
      fname2 = argv[4];
      mode = DOUBLEMODE;
    } else {
      usage(argv[0]); return 1;
    }
    
    if(strchr(argv[1], 'e')) {
      use_exact = true;
    }
  }
  else {
    // no options specified
    // use single-file mode without exact matches
    if(argc != 3) {
      usage(argv[0]);
      return 1;
    }
    threshold = get_threshold(argv[1]);
    fname1 = argv[2];
    mode = SINGLEMODE;
  }

  /////////////////////////////////////////////////////////////////////////
  // argument handling is finished
  /////////////////////////////////////////////////////////////////////////

  // If in line mode, file should just be looped over with I/O.
  // If in single mode, file should be transferred to memory.
  // If in double mode, file 1 should be put in memory and file 2 looped over.
  
  // Set up lcs array (for dynamic programming)
  if(! use_exact) {
    int x;
    for(x=0;x<DIM;++x) {
      array[0][x] = 0;
      array[x][0] = 0;
    }
  }

  if(mode == LINEMODE) {
    //won't need any transferral to memory
    //read in line to compare against
    char newstr[DIM+1];
    int ctr;
    for(ctr=0; ctr<DIM; ++ctr) {
      newstr[ctr] = getchar();
      if(newstr[ctr] == '\n')
	break;
    }
    newstr[ctr] = '\0';
    return gss_line(fname1, newstr, threshold, use_exact);
  }

  /* not in line mode, so there is one file at least which will get stored
     into memory. If single file mode, it's just that file. If double, it's
     file 1. * /
  // When comparing two files, it is best to have the larger file be file 2.

  // Open file 1
  int fd1 = open(fname1, O_RDONLY);
  if(fd1 == -1) {
    fprintf(stderr, "Could not open file 1\n");
    return 1;
  }
  
  // Get size and create buffer
  struct stat st;
  fstat(fd1, &st);
  int file1size = (int) st.st_size;
  char* file1buf = malloc(file1size);
  
  // Store it all in buffer
  read(fd1, file1buf, file1size);

  // The file itself isn't needed anymore
  close(fd1);

  // First pass: Get number of lines
  unsigned int numlines = 0;
  int x;
  for(x=0;x<file1size;++x)
    if(file1buf[x] == '\n')
      numlines++;

  // Prepare array for other lines
  char** file1lines = calloc(numlines, sizeof(char*));

  // Second pass: Put all lines in array
  int flag = 1;
  int linectr = 0;
  for(x=0; x<file1size;++x) {
    if(file1buf[x] == '\n') {
      file1buf[x] = '\0';
      flag = 1;
      linectr++;
      continue;
    }
    if(flag) {
      file1lines[linectr] = &file1buf[x];
      flag = 0;
    }
  }

  ///////////////////////////////////////////////////////////////////////////
  // At this point, we have file1lines, an array of all lines in file 1, and
  // numlines, the number of those lines.
  ///////////////////////////////////////////////////////////////////////////
  
  if(mode == SINGLEMODE) {
    // If comparing one file against itself, there is no need to open anything
    // else, and the looping is different.
    int linectr2 = 0;  // linectr is already declared
    int vals[3];
    for(linectr=0; linectr<numlines; ++linectr) {
      for(linectr2=linectr+1; linectr2<numlines; ++linectr2) {
	
	lcs(file1lines[linectr],file1lines[linectr2],vals);
	if(vals[2] >= threshold) {
	  printf("Line \x1B[31m%d\x1B[0m matched line \x1B[31m%d\x1B[0m:\n",
		 linectr+1, linectr2+1);
	  print_lcs_strings(file1lines[linectr], file1lines[linectr2], vals);
	}
      }
    }
    return 0;
  }
  else if(mode == DOUBLEMODE) {
    // Open second file
    int linectr2 = 0;  // linectr is already declared
    int vals[3];
    FILE* fd2 = fopen(fname2, "r");
    if(!fd2) {
      fprintf(stderr, "Could not open file 2\n");
      return 1;
    }

    char str2[DIM];
    while(fgets(str2, DIM, fd2) != NULL) {
      for(linectr=0; linectr < numlines; ++linectr) {

	lcs(file1lines[linectr], str2, vals);
	if(vals[2] >= threshold) {
	  printf("File 1 line \x1B[31m%d\x1B[0m matched File 2 line \x1B[31m%d\x1B[0m:\n",
		 linectr+1, linectr2+1);
	  print_lcs_strings(file1lines[linectr], str2, vals);
	}
      }
      ++linectr2;
    }
  }
  */
  return 0;
}
