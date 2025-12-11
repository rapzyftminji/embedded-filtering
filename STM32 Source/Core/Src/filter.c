#include "filter.h"

// --- BETTER TINY MATH (Bhaskara I approximation) ---
// Error < 0.0016 across 0 to PI
static float my_abs(float x) { return (x < 0) ? -x : x; }

static float my_sin(float x) {
    // Normalize to -PI to PI
    while (x > M_PI) x -= 2*M_PI;
    while (x < -M_PI) x += 2*M_PI;

    // If negative, flip sign
    float sign = 1.0f;
    if (x < 0) { x = -x; sign = -1.0f; }

    // Bhaskara I formula: 16*x*(pi-x) / (5*pi^2 - 4*x*(pi-x))
    float numerator = 16.0f * x * (M_PI - x);
    float denominator = (5.0f * M_PI * M_PI) - (4.0f * x * (M_PI - x));

    return sign * (numerator / denominator);
}

static float my_cos(float x) {
    return my_sin(x + 1.5707963f); // cos(x) = sin(x + PI/2)
}

static float my_tan(float x) {
    float c = my_cos(x);
    if (my_abs(c) < 1e-5f) return 10000.0f; // Avoid div by zero (saturate)
    return my_sin(x) / c;
}
// ----------------------------------------------------

void init_bw_low_pass(BWLowPass* filter, int order, FTR_PRECISION s, FTR_PRECISION f) {
    filter->n = order/2;
    if(filter->n > MAX_SECTIONS) filter->n = MAX_SECTIONS;
    for(int k=0; k<MAX_SECTIONS; k++) { filter->w0[k]=0; filter->w1[k]=0; filter->w2[k]=0; }

    FTR_PRECISION a = my_tan(M_PI * f / s);
    FTR_PRECISION a2 = a * a;

    for(int i=0; i < filter->n; ++i){
        FTR_PRECISION r = my_sin(M_PI * (2.0f * i + 1.0f) / (4.0f * filter->n));
        FTR_PRECISION s_val = (a2 + 2.0f * a * r + 1.0f);
        filter->A[i] = a2 / s_val;
        filter->d1[i] = 2.0f * (1.0f - a2) / s_val;
        filter->d2[i] = -(a2 - 2.0f * a * r + 1.0f) / s_val;
    }
}

void init_bw_high_pass(BWHighPass* filter, int order, FTR_PRECISION s, FTR_PRECISION f){
    filter->n = order/2;
    if(filter->n > MAX_SECTIONS) filter->n = MAX_SECTIONS;
    for(int k=0; k<MAX_SECTIONS; k++) { filter->w0[k]=0; filter->w1[k]=0; filter->w2[k]=0; }

    FTR_PRECISION a = my_tan(M_PI * f / s);
    FTR_PRECISION a2 = a * a;

    for(int i=0; i < filter->n; ++i){
        FTR_PRECISION r = my_sin(M_PI * (2.0f * i + 1.0f) / (4.0f * filter->n));
        FTR_PRECISION s_val = (a2 + 2.0f * a * r + 1.0f);
        filter->A[i] = 1.0f / s_val;
        filter->d1[i] = 2.0f * (1.0f - a2) / s_val;
        filter->d2[i] = -(a2 - 2.0f * a * r + 1.0f) / s_val;
    }
}

void init_bw_band_pass(BWBandPass* filter, int order, FTR_PRECISION s, FTR_PRECISION fl, FTR_PRECISION fu){
    filter->n = order/4;
    if(filter->n > MAX_SECTIONS) filter->n = MAX_SECTIONS;
    for(int k=0; k<MAX_SECTIONS; k++) {
        filter->w0[k]=0; filter->w1[k]=0; filter->w2[k]=0;
        filter->w3[k]=0; filter->w4[k]=0;
    }

    FTR_PRECISION a = my_cos(M_PI*(fu+fl)/s) / my_cos(M_PI*(fu-fl)/s);
    FTR_PRECISION a2 = a*a;
    FTR_PRECISION b = my_tan(M_PI*(fu-fl)/s);
    FTR_PRECISION b2 = b*b;

    for(int i=0; i<filter->n; ++i){
        FTR_PRECISION r = my_sin(M_PI * (2.0f * i + 1.0f) / (4.0f * filter->n));
        FTR_PRECISION s_val = (b2 + 2.0f * b * r + 1.0f);
        filter->A[i] = b2/s_val;
        filter->d1[i] = 4.0f * a * (1.0f + b * r) / s_val;
        filter->d2[i] = 2.0f * (b2 - 2.0f * a2 - 1.0f)/ s_val;
        filter->d3[i] = 4.0f * a * (1.0f - b * r ) / s_val;
        filter->d4[i] = -(b2 - 2.0f * b * r + 1.0f) / s_val;
    }
}

void init_bw_band_stop(BWBandStop* filter, int order, FTR_PRECISION s, FTR_PRECISION fl, FTR_PRECISION fu){
    filter->n = order/4;
    if(filter->n > MAX_SECTIONS) filter->n = MAX_SECTIONS;
    for(int k=0; k<MAX_SECTIONS; k++) {
        filter->w0[k]=0; filter->w1[k]=0; filter->w2[k]=0;
        filter->w3[k]=0; filter->w4[k]=0;
    }

    FTR_PRECISION a = my_cos(M_PI*(fu+fl)/s) / my_cos(M_PI*(fu-fl)/s);
    FTR_PRECISION a2 = a*a;
    FTR_PRECISION b = my_tan(M_PI*(fu-fl)/s);
    FTR_PRECISION b2 = b*b;

    for(int i=0; i<filter->n; ++i){
        FTR_PRECISION r = my_sin(M_PI * (2.0f * i + 1.0f) / (4.0f * filter->n));
        FTR_PRECISION s_val = (b2 + 2.0f * b * r + 1.0f);
        filter->A[i] = 1.0f / s_val;
        filter->d1[i] = 4.0f * a * (1.0f + b * r ) / s_val;
        filter->d2[i] = 2.0f * (b2 - 2.0f * a2 - 1.0f) / s_val;
        filter->d3[i] = 4.0f * a * (1.0f - b * r) / s_val;
        filter->d4[i] = -(b2 - 2.0f * b * r + 1.0f) / s_val;
    }
    filter->r = 4.0f * a;
    filter->s = 4.0f * a2 + 2.0f;
}

FTR_PRECISION bw_low_pass(BWLowPass* filter, FTR_PRECISION x){
    for(int i=0; i<filter->n; ++i){
        filter->w0[i] = filter->d1[i]*filter->w1[i] + filter->d2[i]*filter->w2[i] + x;
        x = filter->A[i] * (filter->w0[i] + 2.0f * filter->w1[i] + filter->w2[i]);
        filter->w2[i] = filter->w1[i];
        filter->w1[i] = filter->w0[i];
    }
    return x;
}
FTR_PRECISION bw_high_pass(BWHighPass* filter, FTR_PRECISION x){
    for(int i=0; i<filter->n; ++i){
        filter->w0[i] = filter->d1[i]*filter->w1[i] + filter->d2[i]*filter->w2[i] + x;
        x = filter->A[i] * (filter->w0[i] - 2.0f * filter->w1[i] + filter->w2[i]);
        filter->w2[i] = filter->w1[i];
        filter->w1[i] = filter->w0[i];
    }
    return x;
}
FTR_PRECISION bw_band_pass(BWBandPass* filter, FTR_PRECISION x){
    for(int i=0; i<filter->n; ++i){
        filter->w0[i] = filter->d1[i]*filter->w1[i] + filter->d2[i]*filter->w2[i]+ filter->d3[i]*filter->w3[i]+ filter->d4[i]*filter->w4[i] + x;
        x = filter->A[i] * (filter->w0[i] - 2.0f * filter->w2[i] + filter->w4[i]);
        filter->w4[i] = filter->w3[i];
        filter->w3[i] = filter->w2[i];
        filter->w2[i] = filter->w1[i];
        filter->w1[i] = filter->w0[i];
    }
    return x;
}
FTR_PRECISION bw_band_stop(BWBandStop* filter, FTR_PRECISION x){
    for(int i=0; i<filter->n; ++i){
        filter->w0[i] = filter->d1[i]*filter->w1[i] + filter->d2[i]*filter->w2[i]+ filter->d3[i]*filter->w3[i]+ filter->d4[i]*filter->w4[i] + x;
        x = filter->A[i]*(filter->w0[i] - filter->r*filter->w1[i] + filter->s*filter->w2[i]- filter->r*filter->w3[i] + filter->w4[i]);
        filter->w4[i] = filter->w3[i];
        filter->w3[i] = filter->w2[i];
        filter->w2[i] = filter->w1[i];
        filter->w1[i] = filter->w0[i];
    }
    return x;
}
