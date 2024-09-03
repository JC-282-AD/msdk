import numpy as np
import scipy.signal as signal

def calculate_fir_coefficients(cutoff, fs, num_taps):
    """
    Calculate FIR filter coefficients for a low-pass filter.

    Parameters:
    - cutoff: Cutoff frequency of the filter in Hz.
    - fs: Sampling frequency in Hz.
    - num_taps: Number of filter taps (coefficients).

    Returns:
    - fir_coeffs: Filter coefficients.
    """
    # Normalize the cutoff frequency
    normalized_cutoff = cutoff / (0.5 * fs)
    
    # Calculate FIR filter coefficients using the window method
    fir_coeffs = signal.firwin(num_taps, normalized_cutoff, window='hamming')
    
    return fir_coeffs

# Example usage
cutoff_frequency = 2  # Cutoff frequency in Hz
sampling_frequency = 40.0  # Sampling frequency in Hz
filter_order = 29  # Number of taps (filter order + 1)

fir_coeffs = calculate_fir_coefficients(cutoff_frequency, sampling_frequency, filter_order)

# Print the coefficients
print("FIR Filter Coefficients:")
print(fir_coeffs)
for i in fir_coeffs:
    print(i,",",end = " ")