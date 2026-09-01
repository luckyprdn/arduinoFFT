/*
  LGT8F328P uDSC-accelerated FFT example
  Generates a 1kHz + 3kHz test signal, runs computeLGT (Q15 fixed-point FFT
  using the uDSC coprocessor's MAC engine) and reports the dominant peak.

  Requires the LGT8F328P core + LGT8Unlocked library:
    - Board: LGT8F328 (328P-LQFP32 / LQFP48)
    - Serial monitor @ 115200
*/
#include <arduinoFFT.h>

const uint16_t samples = 64;               // power of 2
const float samplingFrequency = 5000;      // Hz

int16_t vReal[samples];                    // Q15 samples
int16_t vImag[samples];                    // zeroed

ArduinoFFT<int16_t> FFT(vReal, vImag, samples, samplingFrequency);

void setup()
{
  Serial.begin(115200);
  while (!Serial) {}
  Serial.println("LGT8F328P uDSC FFT demo");

  // Generate test signal: 1kHz (amp 0.5) + 3kHz (amp 0.2), Q15
  for (uint16_t i = 0; i < samples; i++) {
    float t = (float)i / samplingFrequency;
    float s = 0.5f * sin(2.0f * PI * 1000.0f * t)
            + 0.2f * sin(2.0f * PI * 3000.0f * t);
    vReal[i] = (int16_t)(s * 32767.0f);
    vImag[i] = 0;
  }

  uint32_t t0 = micros();
  uint_fast8_t power = 6; // 2^6 = 64 samples
  FFT.computeLGT(vReal, vImag, samples, power, FFTDirection::Forward);
  uint32_t dt = micros() - t0;

  FFT.complexToMagnitude(vReal, vImag, samples);

  // Find dominant bin (skip DC)
  uint16_t peakBin = 1;
  int16_t peakMag = 0;
  for (uint16_t i = 1; i < samples / 2; i++) {
    if (vReal[i] > peakMag) { peakMag = vReal[i]; peakBin = i; }
  }
  float peakFreq = (float)peakBin * samplingFrequency / samples;

  Serial.print("FFT time: "); Serial.print(dt); Serial.println(" us");
  Serial.print("Peak bin: "); Serial.println(peakBin);
  Serial.print("Peak freq: "); Serial.print(peakFreq, 1); Serial.println(" Hz");
  Serial.print("Peak mag: "); Serial.println(peakMag);
}

void loop() {}