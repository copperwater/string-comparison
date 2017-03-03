/* longest_common_subsequence.c - File containing functions to compute and
   display longest common subsequence information for sets of strings. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "longest_common_subsequence.h"

typedef unsigned char uchar;
typedef unsigned short ushort;

static ushort* array;
static ushort array_dim;

// Macro for accessing data in the array
#define ARRAY(x, y) array[(array_dim*(y)) + (x)]
#define ARRAYCMP(x,y) ((array_dim*(y)) + (x))

/* Allocates array accordingly, zeroing memory. Prep for dynamic programming
   algorithm (setting all ARRAY(0,*) and ARRAY(*,0) to 0) is done automatically.
   Calloc is nice like that.
*/
static void lcsubseq_alloc() {
  if(array != NULL) free(array);
  array = calloc(array_dim*array_dim, sizeof(ushort));
}

/* Performs all necessary setup for use of these functions. */
void lcsubseq_init() {
  array = NULL;
  array_dim = 1024;
  lcsubseq_alloc();
}

/* Clears all memory used by these functions */
void lcsubseq_destroy() {
  free(array);
  array = NULL;
}

// simple max function
ushort max(ushort a, ushort b) {
  if(a > b) return a;
  else return b;
}

/* Takes in two strings and computes the length of the longest common
   subsequence. Slightly different dynamic programming algorithm from
   lcsubstring.
*/
ushort lcsubseq(char* str1, char* str2) {
  ushort len1 = strlen(str1);
  ushort len2 = strlen(str2);
  // handle especially long strings
  ushort lmax = (len1 > len2) ? len1 : len2;
  if(lmax > array_dim) {
    array_dim = lmax + 20 - (lmax % 20); // round up to next multiple of 20
    lcsubseq_alloc();
  }
  ushort tmp = 0;
  int x,y;

  //y iterates over str1 and the ARRAY( ,*), x over str2 and ARRAY(*, ).
  for(y=0;y<len1;++y) {
    for(x=0;x<len2;++x) {
      if(str1[y] == str2[x]) {
	tmp = ARRAY(x, y) + 1;
	ARRAY(x+1, y+1) = tmp;
      }
      else {
	ARRAY(x+1, y+1) = max(ARRAY(x, y+1), ARRAY(x+1, y));
      }
    }
  }

  return ARRAY(len2, len1);
}


/* Given two strings and a threshold, print data about their longest common
   subsequence if its length is greater than or equal to the threshold.
   Additional options: precede_str, which if non-empty gets printed before the
   output, and print_original, which will print str1 and str2.
*/
void lcsubseq_print(char* str1, char* str2, ushort threshold,
                    uchar perc_threshold,
                    char* precede_str, bool print_original) {
  ushort len1 = strlen(str1);
  ushort len2 = strlen(str2);
  ushort lcss_len = lcsubseq(str1, str2);
  float perc1 = ((float) lcss_len / len1) * 100;
  float perc2 = ((float) lcss_len / len2) * 100;
  if((perc1 >= perc_threshold || perc2 >= perc_threshold) && lcss_len >= threshold) {
    if(precede_str != NULL) {
      printf("%s\n", precede_str);
    }
    if(print_original) {
      printf("%s\n%s\n", str1, str2);
    }
    printf("LCSubSeq length: %d\n", lcss_len);
    printf("%f%% match: %s\n", perc1, str1);
    printf("%f%% match: %s\n", perc2, str2);
    printf("\n");
  }
}
