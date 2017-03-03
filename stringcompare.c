/* stringcompare.c - program to compute differences between two strings with a
   number of options
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "longest_common_substring.h"
#include "longest_common_subsequence.h"

// Now with ANSI color codes!

// future improvement idea: return all common substrings over the threshold
// instead of just the longest.
// won't even need its own file, just strcmp

// TODO: add "exact matches" as a compare mode

typedef enum { LINEMODE, SINGLEMODE, DOUBLEMODE } prog_mode;
typedef enum { SUBSTRING_MODE, SUBSEQUENCE_MODE } compare_mode;
typedef unsigned int uint;
typedef unsigned char uchar;


/* Given a formatted I/O file descriptor and string, perform a string
   comparison on the string with each line of the file. The file will be read through
   with formatted I/O.
   threshold determines whether a match is long enough to be worth printing.
   Return value is 0 for success or 1 for a failure of some type.
*/
int compare_line_against_file(FILE* fd, char* str, uint threshold,
                              uchar perc_threshold,
                              compare_mode mode) {
  char tmpstr[64];
  uint filelinectr = 0;
  char str2[1200];
  while(fgets(str2, 1200, fd) != NULL) {
    ++filelinectr;
    snprintf(tmpstr, 64, "Line \x1B[31m%d\x1B[0m", filelinectr);
    if(mode == SUBSTRING_MODE) {
      lcsubstr_print(str, str2, threshold, tmpstr, true, true);
    }
    else if(mode == SUBSEQUENCE_MODE) {
      lcsubseq_print(str, str2, threshold, perc_threshold, tmpstr, false);
    }
  }
  return 0;
}

/* Given an array of char* and a size of that array, compare each line in the array
   against every other line.
   Return 0 for success and 1 for failure.
*/
int compare_file_against_itself(char** lines, uint numlines, uint threshold,
                                uchar perc_threshold,
                                compare_mode mode) {
  int linectr = 0;
  int linectr2 = 0;
  char tmpstr[64];
  for(linectr=0; linectr<numlines; ++linectr) {
    for(linectr2=linectr+1; linectr2<numlines; ++linectr2) {

      snprintf(tmpstr, 64, "Line \x1B[31m%d\x1B[0m matched line \x1B[31m%d\x1B[0m:",
               linectr+1, linectr2+1);
      if(mode == SUBSTRING_MODE) {
        lcsubstr_print(lines[linectr], lines[linectr2], threshold, tmpstr, true, true);
      }
      else if(mode == SUBSEQUENCE_MODE) {
        lcsubseq_print(lines[linectr], lines[linectr2], threshold, perc_threshold, tmpstr, false);
      }
    }
  }
  return 0;
}

/* Given an array of char*, its size, and an open file descriptor for formatted I/O,
   read each line in the file and compare it against every line in the array.
   Return 0 for success and 1 for failure. */
/* NOTE that this function is more updated than the other two. */
int compare_two_files(char** lines, uint numlines, FILE* fd,
                      ushort min_lcsubstr, float min_lcsubstr_perc,
                      ushort min_lcsubseq, float min_lcsubseq_perc) {
  int linectr = 0;
  int linectr2 = 0; // for display purposes only
  char str2[1200];
  char* str1;
  ushort len1, len2;
  while(fgets(str2, 1200, fd) != NULL) {
    len2 = strlen(str2);
    // remove fgets trailing newline
    str2[len2-1] = '\0';
    for(linectr=0; linectr < numlines; ++linectr) {
      str1 = lines[linectr];
      len1 = strlen(str1);

      ushort lcsstr_len = lcsubstr(str1, str2);
      float lcsstr1_perc = ((float) lcsstr_len / len1) * 100;
      float lcsstr2_perc = ((float) lcsstr_len / len2) * 100;

      ushort lcsseq_len = lcsubseq(str1, str2);
      float lcsseq1_perc = ((float) lcsseq_len / len1) * 100;
      float lcsseq2_perc = ((float) lcsseq_len / len2) * 100;


      if(lcsstr_len >= min_lcsubstr &&
         lcsseq_len >= min_lcsubseq &&
         (lcsstr1_perc >= min_lcsubstr_perc || lcsstr2_perc >= min_lcsubstr_perc) &&
         (lcsseq1_perc >= min_lcsubseq_perc || lcsstr2_perc >= min_lcsubseq_perc)) {

         printf("File 1 line   %d   matched File 2 line   %d\n",
               linectr+1, linectr2+1);
         printf("LC substring: %d   LC subsequence: %d\n", lcsstr_len, lcsseq_len);
         printf("%.2f%% %.2f%% %s\n", lcsstr1_perc, lcsseq1_perc, str1);
         printf("%.2f%% %.2f%% %s\n", lcsstr2_perc, lcsseq2_perc, str2);
         printf("\n");
      }
    }
    ++linectr2;
  }
  return 0;
}

/* Argument handling function: given a string that should be a ushort,
 * attempt to parse it into a number and return it. If it fails,
 * terminate the program. */
ushort arg_to_ushort(char* arg) {
  char* strtol_test = NULL;
  unsigned int t;

  t = strtol(arg, &strtol_test, 10);
  if(*strtol_test) {
    fprintf(stderr,"Error: argument %s could not be interpreted as a number\n", arg);
    exit(1);
  }
  return t;
}

/* Same as above but for floats. */
float arg_to_float(char* arg) {
   char* strtof_test = NULL;
   float t;

   t = strtof(arg, &strtof_test);
   if(*strtof_test) {
      fprintf(stderr, "Error: argument %s could not be interpreted as a float\n", arg);
      exit(1);
   }
   return t;
}


void usage(char* argv0) {
  fprintf(stderr,"Usage: %s min_lcsubstr min_lcsubstring_percentage min_lcsubseq min_lcsubseq_percentage file [-L] [-2 file2]\n", argv0);
  fprintf(stderr," -L mode: read in a line and compare to all lines in file\n");
  fprintf(stderr," -2 mode: compare all lines in file with all lines in file 2\n");
  fprintf(stderr," No mode: compare all lines in file with itself\n");
  //fprintf(stderr," -e flag: show only exact matches\n");
}

int main(int argc, char* argv[]) {

  prog_mode mode;
  compare_mode cmode = SUBSEQUENCE_MODE;
  lcsubseq_init();
  unsigned int threshold = 0;
  unsigned int perc_threshold = 0;
  ushort lcsubstr_min, lcsubseq_min;
  float lcsubstr_perc_min, lcsubseq_perc_min;
  char* fname1; char* fname2;

  if(argc < 6) {
    // minimum 6 arguments, argv[0] min_lcsubstr min_lcsubstring_percentage min_lcsubseq min_lcsubseq_percentage file
    usage(argv[0]); return 1;
  }

  lcsubstr_min = arg_to_ushort(argv[1]);
  lcsubstr_perc_min = arg_to_float(argv[2]);
  lcsubseq_min = arg_to_ushort(argv[3]);
  lcsubseq_perc_min = arg_to_float(argv[4]);
  fname1 = argv[5];

  if(argc > 6) {
    // options specified

    if(strchr(argv[6], 'L')) {
      // -L only takes 1 file, so expect 7 arguments total
      if(argc != 7) {
	usage(argv[0]); return 1;
      }
      mode = LINEMODE;
    }

    else if (strchr(argv[6], '2')) {
      if(argc != 8) {
	usage(argv[0]); return 1;
      }
      fname2 = argv[7];
      mode = DOUBLEMODE;

    } else {
      usage(argv[0]); return 1;
    }

  }
  else {
    // no options specified
    // use single-file mode without exact matches
    mode = SINGLEMODE;
  }

  /////////////////////////////////////////////////////////////////////////
  // argument handling is finished
  /////////////////////////////////////////////////////////////////////////

  // If in line mode, file should just be looped over with I/O.
  // If in single mode, file should be transferred to memory.
  // If in double mode, file 1 should be put in memory and file 2 looped over

  if(mode == LINEMODE) {
    //won't need any transferral to memory
    //read in line to compare against
    char newstr[1200];
    fgets(newstr, 1200, stdin);
    newstr[strlen(newstr)-1] = '\0';

    // Open the file for a single readthrough
    FILE* fd = fopen(fname1, "r");
    if(!fd) {
      fprintf(stderr, "Could not open file %s\n", fname1);
      return 1;
    }
    return compare_line_against_file(fd, newstr, threshold, perc_threshold, cmode);
  }

  /* not in line mode, so there is one file at least which will get stored
     into memory. If single file mode, it's just that file. If double, it's
     file 1. */
  /* When comparing two files, it is best to have the larger file be file 2.
     Pretty sure O(m^2 * n) is better than O(m * n^2) when n is larger. Anyone please
     tell me if I'm wrong. Of course, the user controls which file goes where.
     Yes, you are wrong. It is O(mn). The file m lines long is transferred to memory and looped over n times.
  */

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
  /* At this point, we have file1lines, an array of all lines in file 1, and
     numlines, the number of those lines. */
  ///////////////////////////////////////////////////////////////////////////

  if(mode == SINGLEMODE) {
    return compare_file_against_itself(file1lines, numlines, threshold, perc_threshold,
                                       SUBSEQUENCE_MODE);
  }
  else if(mode == DOUBLEMODE) {
    // Open second file
    FILE* fd2 = fopen(fname2, "r");
    if(!fd2) {
      fprintf(stderr, "Could not open file 2\n");
      return 1;
    }
    return compare_two_files(file1lines, numlines, fd2,
                             lcsubstr_min, lcsubstr_perc_min, lcsubseq_min, lcsubseq_perc_min);
  }
  return 0;
}
