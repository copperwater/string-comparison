#ifndef LCSUBSTR_H
#define LCSUBSTR_H

#include <stdbool.h>

void lcsubstr_init();
void lcsubstr_destroy();
unsigned short lcsubstr(char* str1, char* str2);
void lcsubstr_extended(char* str1, char* str2, int* retn);
void lcsubstr_print(char* str1, char* str2, unsigned short threshold,
                    char* precede_str, bool print_original, bool use_color);

#endif
