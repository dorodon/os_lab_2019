#include "Sum.h"

int Sum(const struct SumArgs *args) {
  int sum = 0;
  int *i;
  for (i = args->array + args->begin; i < args->array + args->end; i++)
  {
      sum += *i;
  }
  return sum;
}