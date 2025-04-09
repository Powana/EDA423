#ifndef UTILS_H
#define UTILS_H
#include "application.h"

void print(char*, int);
int compare( const void* a, const void* b);
void add_offset(int* array_out, int* array_in, int array_len, int offset);
int min(int*, int);
int max(int*, int);

#endif