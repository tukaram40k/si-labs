#ifndef SIGNAL_CONDITIONING_H
#define SIGNAL_CONDITIONING_H

#include "config.h"

float saturate(float value, float minVal, float maxVal);

typedef struct
{
  float buffer[MEDIAN_WINDOW_SIZE];
  int index;
  int count;
} median_filter_t;

void median_filter_init(median_filter_t *f);
float median_filter_apply(median_filter_t *f, float sample);

typedef struct
{
  float x[3];
  int count;
} wma_filter_t;

void wma_filter_init(wma_filter_t *f);
float wma_filter_apply(wma_filter_t *f, float sample);

typedef struct
{
  median_filter_t median;
  wma_filter_t wma;
} conditioning_pipeline_t;

void pipeline_init(conditioning_pipeline_t *p);
float pipeline_apply(conditioning_pipeline_t *p, float raw_temp);

#endif // SIGNAL_CONDITIONING_H