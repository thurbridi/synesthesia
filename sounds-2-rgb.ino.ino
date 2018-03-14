#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Adafruit_NeoPixel.h>

// GUItool: begin automatically generated code
AudioInputUSB            usb2;           //xy=553,481
AudioMixer4              mixer1;         //xy=726,466
AudioAnalyzeFFT1024      fft1024;        //xy=881,479
AudioConnection          patchCord1(usb2, 0, mixer1, 2);
AudioConnection          patchCord2(usb2, 1, mixer1, 3);
AudioConnection          patchCord3(mixer1, fft1024);
AudioOutputI2S       out1;
// GUItool: end automatically generated code

const auto LED_PIN = 0;
const auto NUM_LEDS = 19;

// An array to hold the 16 frequency bands
float level[10];

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  AudioMemory(12);
  pinMode(LED_PIN, OUTPUT);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  mixer1.gain(0, 0.5);
  mixer1.gain(1, 0.5);

  delay(5000);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(fft1024.available()){
    // read the 512 FFT frequencies into 16 levels
    // music is heard in octaves, but the FFT data
    // is linear, so for the higher octaves, read
    // many FFT bins together.
    level[0] =  fft1024.read(0);
    level[1] =  fft1024.read(1, 2);
    level[2] =  fft1024.read(3, 5);
    level[3] =  fft1024.read(6, 11);
    level[4] =  fft1024.read(12, 22);
    level[5] =  fft1024.read(23, 43);
    level[6] =  fft1024.read(44, 81);
    level[7] =  fft1024.read(82, 151);
    level[8] =  fft1024.read(152, 278);
    level[9] =  fft1024.read(279, 511);


    for(auto i = 0; i < 10; i++) {
      Serial.print("level ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(level[i]);
      level[i] = level[i] * 255;
    }
    
    strip.setPixelColor(0, level[9], 0, 0);
    strip.setPixelColor(1, level[8], 0, 0);
    strip.setPixelColor(2, level[7], 0, 0);
    strip.setPixelColor(3, level[6], 0, 0);
    strip.setPixelColor(4, level[5], 0, 0);
    strip.setPixelColor(5, level[4], 0, 0);
    strip.setPixelColor(6, level[3], 0, 0);
    strip.setPixelColor(7, level[2], 0, 0);
    strip.setPixelColor(8, level[1], 0, 0);
    
    strip.setPixelColor(9, level[0], 0, 0);

    strip.setPixelColor(10, level[1], 0, 0);
    strip.setPixelColor(11, level[2], 0, 0);
    strip.setPixelColor(12, level[3], 0, 0);
    strip.setPixelColor(13, level[4], 0, 0);
    strip.setPixelColor(14, level[5], 0, 0);
    strip.setPixelColor(15, level[6], 0, 0);
    strip.setPixelColor(16, level[7], 0, 0);
    strip.setPixelColor(17, level[8], 0, 0);
    strip.setPixelColor(18, level[9], 0, 0);
    
    strip.show();
  }
}
