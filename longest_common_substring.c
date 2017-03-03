/* longest_common_substring.c - File containing functions to compute and display
   longest common substring information for sets of strings. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "longest_common_substring.h"

typedef unsigned short ushort;

static ushort* array;
static ushort array_dim;

// Macro for accessing data in the array
#define ARRAY(x, y) array[(array_dim*(y)) + (x)]

/* Allocates array accordingly, zeroing memory. Prep for dynamic programming
   algorithm (setting all ARRAY(0,*) and ARRAY(*,0) to 0) is done automatically.
   Calloc is nice like that.
*/
static void lcsubstr_alloc() {
  if(array != NULL) free(array);
  array = calloc(array_dim*array_dim, sizeof(ushort));
}

/* Performs all necessary setup for use of these functions. */
void lcsubstr_init() {
  array = NULL;
  array_dim = 1024;
  lcsubstr_alloc();
}

/* Clears all memory used by these functions */
void lcsubstr_destroy() {
  free(array);
  array = NULL;
}

/* Takes in two strings and computes the longest common substring.
   If either string is longer than the array can hold, array is reallocated to
   the new size.
*/
ushort lcsubstr(char* str1, char* str2) {
  ushort len1 = strlen(str1);
  ushort len2 = strlen(str2);
  // handle especially long strings
  ushort max = (len1 > len2) ? len1 : len2;
  if(max > array_dim) {
    array_dim = (max + 20) - (max % 20); // round up to next multiple of 20
    lcsubstr_alloc();
  }
  max = 0;
  ushort x,y, tmp;

  //y iterates over str1 and the array[*][], x over str2 and array[][*].
  for(y = 0; y < len1; ++y) {
    for(x = 0; x < len2; ++x) {
      if(str1[y] == str2[x]) {
	tmp = ARRAY(x, y) + 1;
	ARRAY(x+1, y+1) = tmp;
	if (tmp > max) {
	  max = tmp;
	}
      }
      else
	ARRAY(x+1,y+1) = 0;
    }
  }

  return max;
}

/* Like lcsubstr, but returns an int[3] containing more information
 * about the position of the longest common substring.
   Third argument is a pointer to an int[3] array that holds the results.
   retn[0] = offset from the start of the first string
   retn[1] = offset from the start of the second string
   retn[2] = length of the lcs
*/
void lcsubstr_extended(char* str1, char* str2, int* retn) {
  ushort len1 = strlen(str1);
  ushort len2 = strlen(str2);
  // handle especially long strings
  ushort max = (len1 > len2) ? len1 : len2;
  if(max > array_dim) {
    array_dim = (max + 19) % 20; // round up to next multiple of 20
    lcsubstr_alloc();
  }
  ushort xpt,ypt,tmp;
  max = xpt = ypt = tmp = 0;
  ushort x,y;

  //y iterates over str1 and the array[*][], x over str2 and array[][*].
  for(y = 0; y < len1; ++y) {
    for(x = 0; x < len2; ++x) {
      if(str1[y] == str2[x]) {
	tmp = ARRAY(x, y) + 1;
	ARRAY(x+1, y+1) = tmp;
	if (tmp > max) {
	  max = tmp;
	  xpt = x;
	  ypt = y;
	}
      }
      else
	ARRAY(x+1,y+1) = 0;
    }
  }

  retn[0] = 1+ypt-max;
  retn[1] = 1+xpt-max;
  retn[2] = max;
}

/* Given two strings and a threshold, print the longest
   common substring if its length is greater than or equal to the threshold.
   Additional options: precede_str, which if non-empty gets printed before the
   output; print_original, which will print str1 and str2;
   and use_color, which will insert ANSI color sequences to color the strings.
   Notes:
   precede_str need not be newline-terminated.
   print_original without use_color will print the common substring after the
   two original strings.
*/
void lcsubstr_print(char* str1, char* str2, ushort threshold,
                    char* precede_str, bool print_original, bool use_color) {
  ushort len1 = strlen(str1);
  ushort len2 = strlen(str2);
  int vals[3];
  lcsubstr_extended(str1, str2, vals);
  if(vals[2] >= threshold) {
    if(precede_str != NULL) {
      printf("%s\n", precede_str);
    }
    if(print_original) {
      if(use_color) {
        printf("%.*s", vals[0], str1);
        printf("\x1B[32m%.*s\x1B[0m", vals[2], str1+vals[0]);
        printf("%.*s\n", len1-vals[0]-vals[2], str1+vals[0]+vals[2]);

        printf("%.*s", vals[1], str2);
        printf("\x1B[32m%.*s\x1B[0m", vals[2], str2+vals[1]);
        printf("%.*s\n", len2-vals[1]-vals[2], str2+vals[1]+vals[2]);
      }
      else {
        printf("%s\n%s\n%.*s\n", str1, str2, vals[2], str1+vals[0]);
      }
    }
    else {
      printf("%.*s\n", vals[2], str1+vals[0]);
    }
    printf("\n");
  }
}
