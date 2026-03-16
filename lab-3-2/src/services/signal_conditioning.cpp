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

void median_filter_init(median_filter_t *f)
{
  memset(f, 0, sizeof(median_filter_t));
}

static int float_compare(const void *a, const void *b)
{
  float fa = *(const float *)a;
  float fb = *(const float *)b;
  if (fa < fb)
    return -1;
  if (fa > fb)
    return 1;
  return 0;
}

float median_filter_apply(median_filter_t *f, float sample)
{
  f->buffer[f->index] = sample;
  f->index = (f->index + 1) % MEDIAN_WINDOW_SIZE;
  if (f->count < MEDIAN_WINDOW_SIZE)
  {
    f->count++;
  }

  float sorted[MEDIAN_WINDOW_SIZE];
  int n = f->count;
  for (int i = 0; i < n; i++)
  {
    sorted[i] = f->buffer[i];
  }
  qsort(sorted, n, sizeof(float), float_compare);

  return sorted[n / 2];
}

void wma_filter_init(wma_filter_t *f)
{
  memset(f, 0, sizeof(wma_filter_t));
}

float wma_filter_apply(wma_filter_t *f, float sample)
{
  f->x[2] = f->x[1];
  f->x[1] = f->x[0];
  f->x[0] = sample;
  f->count++;

  if (f->count < 3)
  {
    return sample;
  }

  return WMA_W1 * f->x[0] + WMA_W2 * f->x[1] + WMA_W3 * f->x[2];
}

void pipeline_init(conditioning_pipeline_t *p)
{
  median_filter_init(&p->median);
  wma_filter_init(&p->wma);
}

float pipeline_apply(conditioning_pipeline_t *p, float raw_temp)
{
  float clamped = saturate(raw_temp, TEMP_MIN, TEMP_MAX);
  float median_out = median_filter_apply(&p->median, clamped);
  float wma_out = wma_filter_apply(&p->wma, median_out);

  return wma_out;
}

conditioning_result_t pipeline_apply_with_intermediate(conditioning_pipeline_t *p, float raw_temp)
{
  conditioning_result_t result;
  result.saturated = saturate(raw_temp, TEMP_MIN, TEMP_MAX);
  result.median = median_filter_apply(&p->median, result.saturated);
  result.wma = wma_filter_apply(&p->wma, result.median);

  return result;
}