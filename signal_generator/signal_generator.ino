/*
 * ESP32 Pure Signal Generator (No ADC)
 * Output: Gabungan Sinus 10Hz + 50Hz + 100Hz
 * Pin Output: GPIO 25 (DAC1)
 */

#include <math.h>

// Konfigurasi Pin DAC
const int dacPin = 25; 

// Parameter Frekuensi
const double f1 = 10.0;
const double f2 = 50.0;
const double f3 = 100.0;

// Amplitudo (Kekuatan Sinyal)
// Total amplitudo (3 x 40 = 120) + Offset 128 = 248. 
// Ini aman di bawah batas maksimal 255.
const double amplitude = 40.0; 

void setup() {
  Serial.begin(115200);
  // Tidak perlu setup khusus untuk dacWrite
  // DAC diaktifkan otomatis saat perintah dacWrite dipanggil
}

void loop() {
  // 1. Ambil waktu sistem dalam detik yang sangat presisi
  // micros() mengembalikan waktu dalam mikrodetik sejak board nyala
  double t = micros() / 1000000.0;

  // 2. Hitung 3 gelombang sinus berdasarkan waktu 't'
  double sin1 = amplitude * sin(2 * PI * f1 * t);
  double sin2 = amplitude * sin(2 * PI * f2 * t);
  double sin3 = amplitude * sin(2 * PI * f3 * t);

  // 3. Gabungkan sinyal + Offset (128)
  double signalTotal = 128 + sin1 + sin2 + sin3;
  // 4. Output ke Pin 25
  // Kita cast ke (int) karena DAC butuh integer 0-255
  dacWrite(dacPin, (int)signalTotal);
  
  // Tidak perlu delay, biarkan loop berjalan secepat mungkin
  // untuk resolusi gelombang yang paling halus.
}
