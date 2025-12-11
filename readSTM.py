import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
from collections import deque
import threading
import time
import sys

# --- CONFIGURATION ---
SERIAL_PORT = '/dev/ttyACM0' 
BAUD_RATE = 115200 
MAX_POINTS = 512  # Increased for better FFT resolution
FS = 1000 # Estimated Sampling Frequency (Hz) - Adjust based on your device

# --- SETUP SERIAL ---
try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print(f"Connected to {SERIAL_PORT} at {BAUD_RATE} baud.")
except serial.SerialException:
    print(f"Error: Could not open serial port {SERIAL_PORT}.")
    print("Check connection or permissions (sudo usermod -a -G dialout $USER).")
    exit()

# --- SETUP DATA ---
data = deque([0] * MAX_POINTS, maxlen=MAX_POINTS)

# --- SETUP PLOT ---
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))
plt.subplots_adjust(hspace=0.4)

# Time Domain Plot
ax1.set_title("Real-Time ADC Data (Type 's' to Start, 'p' to Stop)")
ax1.set_xlabel("Time (Samples)")
ax1.set_ylabel("ADC Value (0-4095)")
ax1.set_ylim(0, 4200)
line, = ax1.plot(range(MAX_POINTS), data)

# Frequency Domain Plot
ax2.set_title("Frequency Spectrum (FFT)")
ax2.set_xlabel("Frequency (Hz)")
ax2.set_ylabel("Magnitude")
ax2.set_xlim(0, FS / 2)
ax2.set_ylim(0, 1000) # Adjust based on signal magnitude
line_fft, = ax2.plot([], [], color='r')

# --- UPDATE FUNCTION (Restored) ---
def animate(i):
    # Read all available data from serial buffer to keep plot real-time
    while ser.is_open and ser.in_waiting:
        try:
            serial_string = ser.readline().decode('utf-8').strip()
            if serial_string:
                val = int(serial_string)
                data.append(val)
        except (ValueError, serial.SerialException):
            pass
    
    # Update Time Domain
    line.set_ydata(data)
    
    # Update Frequency Domain
    if len(data) == MAX_POINTS:
        signal = np.array(data)
        # Remove DC component for better visualization of AC signals
        signal_ac = signal - np.mean(signal)
        
        fft_vals = np.fft.fft(signal_ac)
        fft_freqs = np.fft.fftfreq(len(signal_ac), 1/FS)
        
        # Take positive half
        pos_mask = fft_freqs >= 0
        fft_freqs = fft_freqs[pos_mask]
        fft_mag = np.abs(fft_vals)[pos_mask] / len(signal) * 2 # Normalize
        
        line_fft.set_data(fft_freqs, fft_mag)
        
        # Dynamic Y-axis scaling for FFT
        if np.max(fft_mag) > 0:
             ax2.set_ylim(0, np.max(fft_mag) * 1.2)

    return line, line_fft

# --- USER INPUT THREAD ---
def user_input_loop():
    time.sleep(1)
    print("\n" + "="*30)
    print(" STREAMING CONTROLS")
    print(" s : START Streaming (Raw Data)")
    print(" a : Apply Filter A (Proof: Shift Up)")
    print(" b : Apply Filter B (Proof: Invert Signal)")
    print(" c : Apply Filter C (Proof: Divide by 2)")
    print(" p : PAUSE Streaming")
    print(" q : QUIT")
    print("="*30)
    
    while True:
        try:
            cmd = input("Command > ").strip().lower()
            
            if cmd == 's':
                print(">> Mode: RAW Streaming")
                ser.write(b's')
            elif cmd == 'p':
                print(">> Paused")
                ser.write(b'p')
            elif cmd == 'a':
                print(">> Mode: Filter A (Shift)")
                ser.write(b'a')
            elif cmd == 'b':
                print(">> Mode: Filter B (Invert)")
                ser.write(b'b')
            elif cmd == 'c':
                print(">> Mode: Filter C (Scale)")
                ser.write(b'c')
            elif cmd == 'd':
                print(">> Mode: Filter C (Scale)")
                ser.write(b'd')
            elif cmd == 'q':
                ser.write(b'p')
                ser.close()
                sys.exit()
            else:
                print("Unknown command.")
                
        except (EOFError, KeyboardInterrupt):
            ser.close()
            sys.exit()

# --- START INPUT LISTENER ---
# daemon=True means this thread will kill itself if the main plot window closes
input_thread = threading.Thread(target=user_input_loop, daemon=True)
input_thread.start()

# --- START ANIMATION ---
ani = animation.FuncAnimation(fig, animate, interval=20, blit=True)

print("Plotting window opening...")
plt.show() # This blocks the main thread (keeps graph open)

# --- CLEANUP ---
# This runs if you close the window manually using the X button
if ser.is_open:
    print("Window closed. Sending stop command.")
    ser.write(b'p')
    ser.close()