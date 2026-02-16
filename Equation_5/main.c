#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ---------------- Equation 5 ----------------
// x(t) = A2 * sin(2*pi*f2*t + phi2)
static double x_signal(double t, double A2, double f2, double phi2) {
    return A2 * sin(2.0 * M_PI * f2 * t + phi2);
}

// Uniform quantization on [-Amax, +Amax] with (2^Nbits) levels
static double quantize_uniform(double x, double Amax, int Nbits, int *out_k) {
    const int L = 1 << Nbits;
    const double xmin = -Amax, xmax = Amax;
    const double delta = (xmax - xmin) / (L - 1);

    // Saturation
    if (x < xmin) x = xmin;
    if (x > xmax) x = xmax;

    // Nearest level index
    long long k = llround((x - xmin) / delta);
    if (k < 0) k = 0;
    if (k > (L - 1)) k = (L - 1);

    if (out_k) *out_k = (int)k;
    return xmin + (double)k * delta;
}

static void to_binary(int k, int Nbits, char *out) {
    for (int i = Nbits - 1; i >= 0; --i) {
        out[Nbits - 1 - i] = ((k >> i) & 1) ? '1' : '0';
    }
    out[Nbits] = '\0';
}

int main(void) {
    // ---------------- Parameters ----------------
    const double A2   = 1.0;
    const double f2   = 5.0;   // Hz
    const double phi2 = 0.0;   // rad

    const double Fcont = 2000.0; // for smooth "original"
    const double Fe    = 40.0;   // sampling frequency
    const int    Nbits = 4;      // quantization bits
    const double Tshow = 2.0 / f2; // 2 periods -> 0.4s for f2=5Hz

    const double Amax = fabs(A2);

    // ---------------- Output files ----------------
    FILE *forig  = fopen("orig.dat",  "w");
    FILE *fsamp  = fopen("samp.dat",  "w");
    FILE *fquant = fopen("quant.dat", "w");
    FILE *fmmse  = fopen("mmse.dat",  "w");
    FILE *fbits  = fopen("bits.dat",  "w");
    FILE *frecon = fopen("recon.dat", "w");
    FILE *frecon_mmse = fopen("recon_mmse.dat", "w");

    if (!forig || !fsamp || !fquant || !fmmse || !fbits || !frecon || !frecon_mmse) {
        fprintf(stderr, "Error: cannot open output files.\n");
        return 1;
    }

    // ---------------- Step 1: Original signal ----------------
    const int Ncont = (int)floor(Tshow * Fcont) + 1;
    for (int n = 0; n < Ncont; n++) {
        double t = n / Fcont;
        double x = x_signal(t, A2, f2, phi2);
        fprintf(forig, "%.10f %.10f\n", t, x);
    }

    // ---------------- Step 2: Sampling ----------------
    const int Ns = (int)floor(Tshow * Fe) + 1;

    double *t_s = (double*)malloc(Ns * sizeof(double));
    double *x_s = (double*)malloc(Ns * sizeof(double));
    double *x_q = (double*)malloc(Ns * sizeof(double));
    double *x_mmse = (double*)malloc(Ns * sizeof(double));
    int    *k_q = (int*)malloc(Ns * sizeof(int));

    if (!t_s || !x_s || !x_q || !x_mmse || !k_q) {
        fprintf(stderr, "Error: malloc failed.\n");
        return 1;
    }

    for (int n = 0; n < Ns; n++) {
        t_s[n] = n / Fe;
        x_s[n] = x_signal(t_s[n], A2, f2, phi2);
        fprintf(fsamp, "%.10f %.10f\n", t_s[n], x_s[n]);
    }

    // ---------------- Step 3: Quantization ----------------
    for (int n = 0; n < Ns; n++) {
        x_q[n] = quantize_uniform(x_s[n], Amax, Nbits, &k_q[n]);
        fprintf(fquant, "%.10f %.10f\n", t_s[n], x_q[n]);
    }

    // ---------------- Step 4: Binary encoding + size ----------------
    fprintf(fbits, "# n t x_quant index bits\n");
    for (int n = 0; n < Ns; n++) {
        char b[64];
        to_binary(k_q[n], Nbits, b);
        fprintf(fbits, "%d %.10f %.10f %d %s\n", n, t_s[n], x_q[n], k_q[n], b);
    }

    const long total_bits = (long)Ns * (long)Nbits;
    const double total_bytes = total_bits / 8.0;

    // ---------------- MMSE (LMMSE) estimation ----------------
    // x_hat[n] = alpha * x_q[n]
    double num = 0.0, den = 0.0;
    for (int n = 0; n < Ns; n++) {
        num += x_s[n] * x_q[n];
        den += x_q[n] * x_q[n];
    }
    const double alpha = (den > 1e-12) ? (num / den) : 1.0;

    for (int n = 0; n < Ns; n++) {
        x_mmse[n] = alpha * x_q[n];
        fprintf(fmmse, "%.10f %.10f\n", t_s[n], x_mmse[n]);
    }

    // ---------------- MSE comparison ----------------
    double mse_quant = 0.0, mse_mmse = 0.0;
    for (int n = 0; n < Ns; n++) {
        double e_q = x_s[n] - x_q[n];
        double e_m = x_s[n] - x_mmse[n];
        mse_quant += e_q * e_q;
        mse_mmse  += e_m * e_m;
    }
    mse_quant /= (double)Ns;
    mse_mmse  /= (double)Ns;

    // ---------------- Step 5: Reconstruction (ZOH) ----------------
    for (int n = 0; n < Ncont; n++) {
        double t = n / Fcont;
        int idx = (int)floor(t * Fe);
        if (idx < 0) idx = 0;
        if (idx >= Ns) idx = Ns - 1;

        fprintf(frecon, "%.10f %.10f\n", t, x_q[idx]);
        fprintf(frecon_mmse, "%.10f %.10f\n", t, x_mmse[idx]);
    }

    // ---------------- Summary ----------------
    printf("----- Summary -----\n");
    printf("Eq5: x(t)=A2*sin(2*pi*f2*t+phi2)\n");
    printf("A2=%.3f, f2=%.3f Hz, phi2=%.3f rad\n", A2, f2, phi2);
    printf("Tshow=%.6f s, Fe=%.3f Hz, Ns=%d samples\n", Tshow, Fe, Ns);
    printf("Quant: Nbits=%d -> L=%d levels\n", Nbits, (1 << Nbits));
    printf("Binary size: %ld bits (%.2f bytes)\n", total_bits, total_bytes);
    printf("MMSE (LMMSE): alpha=%.6f\n", alpha);
    printf("MSE quant=%.10f | MSE mmse=%.10f\n", mse_quant, mse_mmse);

    // ---------------- Clean up ----------------
    fclose(forig);
    fclose(fsamp);
    fclose(fquant);
    fclose(fmmse);
    fclose(fbits);
    fclose(frecon);
    fclose(frecon_mmse);

    free(t_s);
    free(x_s);
    free(x_q);
    free(x_mmse);
    free(k_q);

    return 0;
}
