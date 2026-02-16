set grid
set key left top
set xlabel "t (s)"
set ylabel "Amplitude"
set title "Equation 1 - Quantization vs MMSE"

plot "orig.dat"       using 1:2 with lines  linewidth 2 title "Original", \
     "quant.dat"      using 1:2 with points pointtype 7 title "Quantized", \
     "mmse.dat"       using 1:2 with points pointtype 5 title "MMSE est.", \
     "recon.dat"      using 1:2 with lines  linewidth 2 title "Recon ZOH", \
     "recon_mmse.dat" using 1:2 with lines  linewidth 2 title "Recon ZOH + MMSE"
