set term qt size 1000,600
set grid
set key left top
set xlabel "t (s)"
set ylabel "Amplitude"
set title "Equation 10 - Quantization vs MMSE"
set tmargin 3

plot "orig.dat"       using 1:2 with lines  title "Original", \
     "samp.dat"       using 1:2 with points title "Sampled", \
     "quant.dat"      using 1:2 with points title "Quantized", \
     "mmse.dat"       using 1:2 with points title "MMSE est.", \
     "recon.dat"      using 1:2 with lines  title "Recon ZOH", \
     "recon_mmse.dat" using 1:2 with lines  title "Recon ZOH + MMSE"
