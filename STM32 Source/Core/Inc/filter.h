/* filter.h - NO MALLOC VERSION */
#ifndef filter_h
#define filter_h

#if __cplusplus
extern "C"{
#endif

#define FTR_PRECISION float
#define M_PI 3.1415927f

// Max order 4 means n=2 for LPF/HPF, n=1 for BPF/BSF.
// We set array size to 2 to cover all cases safely.
#define MAX_SECTIONS 2

typedef struct {
    int n;
    FTR_PRECISION A[MAX_SECTIONS];
    FTR_PRECISION d1[MAX_SECTIONS];
    FTR_PRECISION d2[MAX_SECTIONS];
    FTR_PRECISION w0[MAX_SECTIONS];
    FTR_PRECISION w1[MAX_SECTIONS];
    FTR_PRECISION w2[MAX_SECTIONS];
} BWLowPass;
typedef BWLowPass BWHighPass;

typedef struct {
    int n;
    FTR_PRECISION A[MAX_SECTIONS];
    FTR_PRECISION d1[MAX_SECTIONS];
    FTR_PRECISION d2[MAX_SECTIONS];
    FTR_PRECISION d3[MAX_SECTIONS];
    FTR_PRECISION d4[MAX_SECTIONS];
    FTR_PRECISION w0[MAX_SECTIONS];
    FTR_PRECISION w1[MAX_SECTIONS];
    FTR_PRECISION w2[MAX_SECTIONS];
    FTR_PRECISION w3[MAX_SECTIONS];
    FTR_PRECISION w4[MAX_SECTIONS];
} BWBandPass;

typedef struct {
    int n;
    FTR_PRECISION r;
    FTR_PRECISION s;
    FTR_PRECISION A[MAX_SECTIONS];
    FTR_PRECISION d1[MAX_SECTIONS];
    FTR_PRECISION d2[MAX_SECTIONS];
    FTR_PRECISION d3[MAX_SECTIONS];
    FTR_PRECISION d4[MAX_SECTIONS];
    FTR_PRECISION w0[MAX_SECTIONS];
    FTR_PRECISION w1[MAX_SECTIONS];
    FTR_PRECISION w2[MAX_SECTIONS];
    FTR_PRECISION w3[MAX_SECTIONS];
    FTR_PRECISION w4[MAX_SECTIONS];
} BWBandStop;

// Changed to 'init' functions that take a pointer, instead of returning one
void init_bw_low_pass(BWLowPass* filter, int order, FTR_PRECISION s, FTR_PRECISION f);
void init_bw_high_pass(BWHighPass* filter, int order, FTR_PRECISION s, FTR_PRECISION f);
void init_bw_band_pass(BWBandPass* filter, int order, FTR_PRECISION s, FTR_PRECISION fl, FTR_PRECISION fu);
void init_bw_band_stop(BWBandStop* filter, int order, FTR_PRECISION s, FTR_PRECISION fl, FTR_PRECISION fu);

FTR_PRECISION bw_low_pass(BWLowPass* filter, FTR_PRECISION input);
FTR_PRECISION bw_high_pass(BWHighPass* filter, FTR_PRECISION input);
FTR_PRECISION bw_band_pass(BWBandPass* filter, FTR_PRECISION input);
FTR_PRECISION bw_band_stop(BWBandStop* filter, FTR_PRECISION input);

#if __cplusplus
}
#endif
#endif
