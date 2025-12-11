import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.widgets import Button
import numpy as np
from collections import deque
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
    print("Check connection or permissions.")
    sys.exit()

# --- SETUP DATA ---
data = deque([0] * MAX_POINTS, maxlen=MAX_POINTS)

# --- SETUP PLOT ---
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))
plt.subplots_adjust(hspace=0.4, bottom=0.2) 
ax1.set_title("Real-Time ADC Data")
ax1.set_xlabel("Time (Samples)")
ax1.set_ylabel("ADC Value")
ax1.set_ylim(0, 4200)
line, = ax1.plot(range(MAX_POINTS), data)

ax2.set_title("Frequency Spectrum (FFT)")
ax2.set_xlabel("Frequency (Hz)")
ax2.set_ylabel("Magnitude")
ax2.set_xlim(0, FS / 2)
ax2.set_ylim(0, 1000)
line_fft, = ax2.plot([], [], color='r')

def send_cmd(byte_cmd):
    if ser.is_open:
        try:
            ser.write(byte_cmd)
            print(f">> Sent: {byte_cmd}")
        except serial.SerialException:
            pass

def start_click(event): send_cmd(b's')
def lpf_click(event):   send_cmd(b'a')
def hpf_click(event):   send_cmd(b'b')
def bpf_click(event):   send_cmd(b'c')
def bsf_click(event):   send_cmd(b'd') 
def pause_click(event): send_cmd(b'p')

btn_width = 0.12
btn_height = 0.075
spacing = 0.02
start_x = 0.08

# 1. START
ax_start = plt.axes([start_x, 0.05, btn_width, btn_height])
btn_start = Button(ax_start, 'Start', color='#d9ead3', hovercolor='#b6d7a8')
btn_start.on_clicked(start_click)

# 2. LPF
ax_lpf = plt.axes([start_x + (btn_width + spacing), 0.05, btn_width, btn_height])
btn_lpf = Button(ax_lpf, 'LPF', color='#cfe2f3', hovercolor='#9fc5e8')
btn_lpf.on_clicked(lpf_click)

# 3. HPF
ax_hpf = plt.axes([start_x + 2*(btn_width + spacing), 0.05, btn_width, btn_height])
btn_hpf = Button(ax_hpf, 'HPF', color='#cfe2f3', hovercolor='#9fc5e8')
btn_hpf.on_clicked(hpf_click)

# 4. BPF
ax_bpf = plt.axes([start_x + 3*(btn_width + spacing), 0.05, btn_width, btn_height])
btn_bpf = Button(ax_bpf, 'BPF', color='#cfe2f3', hovercolor='#9fc5e8')
btn_bpf.on_clicked(bpf_click)

# 5. BSF 
ax_bsf = plt.axes([start_x + 4*(btn_width + spacing), 0.05, btn_width, btn_height])
btn_bsf = Button(ax_bsf, 'BSF', color='#cfe2f3', hovercolor='#9fc5e8')
btn_bsf.on_clicked(bsf_click)

# 6. PAUSE
ax_pause = plt.axes([start_x + 5*(btn_width + spacing), 0.05, btn_width, btn_height])
btn_pause = Button(ax_pause, 'Pause', color='#f4cccc', hovercolor='#ea9999')
btn_pause.on_clicked(pause_click)

def animate(i):
    while ser.is_open and ser.in_waiting:
        try:
            serial_string = ser.readline().decode('utf-8').strip()
            if serial_string:
                val = int(serial_string)
                data.append(val)
        except (ValueError, serial.SerialException):
            pass
    
    line.set_ydata(data)
    
    if len(data) == MAX_POINTS:
        signal = np.array(data)
        signal_ac = signal - np.mean(signal)
        
        fft_vals = np.fft.fft(signal_ac)
        fft_freqs = np.fft.fftfreq(len(signal_ac), 1/FS)
        
        pos_mask = fft_freqs >= 0
        fft_freqs = fft_freqs[pos_mask]
        fft_mag = np.abs(fft_vals)[pos_mask] / len(signal) * 2 
        
        line_fft.set_data(fft_freqs, fft_mag)
        
        if np.max(fft_mag) > 0:
             ax2.set_ylim(0, np.max(fft_mag) * 1.2)

    return line, line_fft

# --- START ANIMATION ---
ani = animation.FuncAnimation(fig, animate, interval=20, blit=True)

plt.show()

# --- CLEANUP ---
if ser.is_open:
    print("Window closed. Sending stop command.")
    ser.write(b'p')
    ser.close()