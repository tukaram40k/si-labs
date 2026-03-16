#include "signal_conditioning.h"
#include <string.h>
#include <stdlib.h>

float saturate(float value, float minVal, float maxVal)
{
  if (value < minVal)
    return minVal;
  if (value > maxVal)
    return maxVal;
  return value;
}