#include <AudioInI2S.h>
#define SAMPLE_SIZE 1024  // Buffer size of read samples
#define SAMPLE_RATE 44100 // Audio Sample Rate
/* Required defines for audio analysis */
#define BAND_SIZE 8 // powers of 2 up to 64, defaults to 8
#include <AudioAnalysis.h>
AudioAnalysis audioInfo;
// ESP32 S2 Mini
#define BCK_PIN 39             // Clock pin from the mic.
#define WS_PIN 38             // WS pin from the mic.
#define DATA_PIN 41            // Data pin from the mic.
#define CHANNEL_SELECT_PIN -1 // Pin to select the channel output from the mic.
AudioInI2S mic(BCK_PIN, WS_PIN, DATA_PIN, CHANNEL_SELECT_PIN); // defaults to RIGHT channel.
int32_t samples[SAMPLE_SIZE]; // I2S sample data is stored here
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  //录音
  mic.begin(SAMPLE_SIZE, SAMPLE_RATE); // Starts the I2S DMA port.
  // audio analysis setup
  audioInfo.setNoiseFloor(10);       // sets the noise floor
  audioInfo.normalize(true, 0, 255); // normalize all values to range provided.
  // audioInfo.autoLevel(AudioAnalysis::ACCELERATE_FALLOFF, 1); // uncomment this line to set auto level falloff rate
  audioInfo.bandPeakFalloff(AudioAnalysis::EXPONENTIAL_FALLOFF, 0.05); // set the band peak fall off rate
  audioInfo.vuPeakFalloff(AudioAnalysis::EXPONENTIAL_FALLOFF, 0.05);    // set the volume unit peak fall off rate
  delay(300);
}

void loop() {
  // put your main code here, to run repeatedly:
  mic.read(samples); // Stores the current I2S port buffer into samples.
  audioInfo.computeFFT(samples, SAMPLE_SIZE, SAMPLE_RATE);
  audioInfo.computeFrequencies(BAND_SIZE);

  float *bands = audioInfo.getBands();
  float *peaks = audioInfo.getPeaks();
  float vuMeter = audioInfo.getVolumeUnit();
  float vuMeterPeak = audioInfo.getVolumeUnitPeak();

  // Send data to serial plotter
  for (int i = 0; i < BAND_SIZE; i++)
  {
    Serial.printf("%d:%.1f,", i, peaks[i]);
  }

  // also send the vu meter data
  Serial.printf("vuValue:%.1f,vuPeak:%.2f", vuMeter, vuMeterPeak);

  Serial.println();
}
