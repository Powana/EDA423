#include "utils.h"

void print(char *format, int arg) {
  char buf[128];
  snprintf(buf, 128, format, arg);
  SCI_WRITE(&sci0, buf);
}


int compare( const void* a, const void* b)
{
   int int_a = * ( (int*) a );
   int int_b = * ( (int*) b );

   // an easy expression for comparing
   return (int_a > int_b) - (int_a < int_b);
}

void add_offset(int* array_out, int* array_in, int array_len, int offset) {
    for (int i =0; i <= array_len; i++) {
        array_out[i] = array_in[i] + offset;
    }
}

int max(int* array, int array_len) {
    int res = array[0];
    for (int i=0; i <= array_len; i++) {
        if (array[i] > res) res= array[i];
    } 
    return res;
}

int min(int* array, int array_len) {
    int res = array[0];
    for (int i=0; i <= array_len; i++) {
        if (array[i] < res) res= array[i];
    } 
    return res;
}

int get_rank_order(int rank, int* ranks, int ranks_len) {
    int less_than = 0;
    for(int i = 0; i < ranks_len; i++) {
        if (ranks[i] < rank) less_than++;
    }
    return less_than;
}