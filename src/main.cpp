#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Adafruit_NeoPixel.h>

#include "color.hpp"

// GUItool: begin automatically generated code
AudioInputUSB usb2;          //xy=553,481
AudioMixer4 mixer1;          //xy=726,466
AudioAnalyzeFFT1024 fft1024; //xy=881,479
AudioConnection patchCord1(usb2, 0, mixer1, 2);
AudioConnection patchCord2(usb2, 1, mixer1, 3);
AudioConnection patchCord3(mixer1, fft1024);
AudioOutputI2S out1;
// GUItool: end automatically generated code

const auto LED_PIN = 0;
const auto NUM_LEDS = 19;

// An array to hold the 10 frequency bands
float level[10];
double red[10];
double green[10];
double blue[10];
auto hue = 0.0;
auto sat = 1.0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  Serial.begin(9600);
  AudioMemory(12);
  pinMode(LED_PIN, OUTPUT);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  mixer1.gain(0, 1);
  mixer1.gain(1, 1);

  delay(5000);
}

void loop()
{
  if (fft1024.available())
  {
    // read the 512 FFT frequencies into 10 levels
    // music is heard in octaves, but the FFT data
    // is linear, so for the higher octaves, read
    // many FFT bins together.
    level[0] = fft1024.read(0);
    level[1] = fft1024.read(1, 2);
    level[2] = fft1024.read(3, 5);
    level[3] = fft1024.read(6, 11);
    level[4] = fft1024.read(12, 22);
    level[5] = fft1024.read(23, 43);
    level[6] = fft1024.read(44, 81);
    level[7] = fft1024.read(82, 151);
    level[8] = fft1024.read(152, 278);
    level[9] = fft1024.read(279, 511);

    hue++;
    hue = fmod(hue, 360);

    for (auto i = 0; i < 10; i++)
    {
      auto chroma = level[i] * sat;
      auto X = chroma * (1 - abs(fmod(hue / 60.0, 2) - 1));
      auto m = level[i] - chroma;

      auto Rs = 0.0;
      auto Gs = 0.0;
      auto Bs = 0.0;

      if (hue >= 0 && hue < 60)
      {
        Rs = chroma;
        Gs = X;
      }
      else if (hue >= 60 && hue < 120)
      {
        Rs = X;
        Gs = chroma;
      }
      else if (hue >= 120 && hue < 180)
      {
        Gs = chroma;
        Bs = X;
      }
      else if (hue >= 180 && hue < 240)
      {
        Gs = X;
        Bs = chroma;
      }
      else if (hue >= 240 && hue < 300)
      {
        Rs = X;
        Bs = chroma;
      }
      else
      {
        Rs = chroma;
        Bs = X;
      }

      red[i] = (Rs + m) * 255;
      green[i] = (Gs + m) * 255;
      blue[i] = (Bs + m) * 255;
    }

    for (auto i = 0; i < 9; i++)
    {
      strip.setPixelColor(i, red[9 - i], blue[9 - i], green[9 - i]);
    }

    strip.setPixelColor(9, red[0], blue[0], green[0]);

    for (auto i = 0; i < 9; i++)
    {
      strip.setPixelColor(i + 10, red[i + 1], blue[i + 1], green[i + 1]);
    }

    strip.show();
  }
}