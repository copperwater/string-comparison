#ifndef LCSUBSEQ_H
#define LCSUBSEQ_H

#include <stdbool.h>

void lcsubseq_init();
void lcsubseq_destroy();
unsigned short lcsubseq(char* str1, char* str2);
void lcsubseq_print(char* str1, char* str2, unsigned short threshold,
                    unsigned char perc_threshold,
                    char* precede_str, bool print_original);

#endif
