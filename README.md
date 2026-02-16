📖 Overview

- This project implements a complete Digital Signal Processing (DSP) pipeline in C, focusing on signal generation, sampling, quantization, reconstruction, and error minimization using Linear Minimum Mean Square Error (LMMSE) estimation.

- Multiple sinusoidal signal configurations are analyzed to study the impact of quantization and statistical estimation on reconstruction quality.

- The project includes performance evaluation through Mean Square Error (MSE) analysis and graphical visualization using Gnuplot.



🎯 Objectives

- Generate continuous-time sinusoidal signals

- Perform uniform sampling (respecting Nyquist-Shannon theorem)

- Apply uniform quantization (4-bit resolution)

- Encode quantized values into binary representation

- Reconstruct signals using Zero-Order Hold (ZOH)

- Apply LMMSE estimation to reduce reconstruction error

- Compare reconstruction performance using MSE metrics

- Visualize all processing stages with Gnuplot



🧠 Concepts Covered

- Digital Signal Processing (DSP)

- Nyquist-Shannon Sampling Theorem

- Uniform Quantization

- Zero-Order Hold (ZOH)

- Binary Encoding

- Mean Square Error (MSE)

- Linear Minimum Mean Square Error (LMMSE)

- Numerical Computation in C



🛠 Technologies Used

- C Programming Language

- GCC (MSYS2 / MinGW)

- Gnuplot

- Git & GitHub

- Command Line (CLI)



📂 Project Structure

Projet_MMSE/
│
├── Equation_1/
├── Equation_2/
├── ...
├── Equation_10/
│
└── README.md

Each equation folder contains:

- main.c → Signal generation and DSP implementation

- dsp.exe → Compiled executable

- orig.dat → Original continuous signal

- samp.dat → Sampled signal

- quant.dat → Quantized signal

- mmse.dat → MMSE estimated signal

- recon.dat → ZOH reconstruction

- recon_mmse.dat → ZOH + MMSE reconstruction

- plot.gp → Gnuplot visualization script



⚙️ How to Run

1️⃣ Compile:

gcc main.c -o dsp -lm

2️⃣ Execute:

./dsp


3️⃣ Plot results:

gnuplot -p plot.gp


🚀 Key Takeaways

Quantization introduces distortion proportional to signal dynamics.

Increasing signal complexity reduces relative MMSE gain.

LMMSE provides measurable error reduction across all configurations.

The full DSP pipeline can be implemented efficiently in low-level C.


👤 Author

Tohme Tohme
Engineering Student – Digital Systems & Signal Processing
GitHub: https://github.com/tto3me
LinkedIn: https://www.linkedin.com/in/tto3me/

