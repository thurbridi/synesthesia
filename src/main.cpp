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

const int LED_PIN = 0;
const int NUM_LEDS = 31;
const int N_BINS = ceil(NUM_LEDS / 2.0);

float level[N_BINS];
float red[N_BINS];
float green[N_BINS];
float blue[N_BINS];
auto hue = 0.0;
auto sat = 1.0;
auto elapsed_hue = 0;

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
    for (auto i = 0; i < N_BINS - 2; i++)
    {
      auto r = pow(512, 1.0 / (N_BINS - 1));
      level[i] = fft1024.read(floor(pow(r, i)) - 1, floor(pow(r, i + 1) - 1));
    }

    auto now = millis();
    if (now - elapsed_hue > 100)
    {
      hue += 5;
      hue = fmod(hue, 360);
      elapsed_hue = now;
    }

    for (auto i = 0; i < N_BINS; i++)
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

    for (auto i = 0; i < N_BINS; i++)
    {
      strip.setPixelColor(i, red[N_BINS - i - 1], blue[N_BINS - i - 1], green[N_BINS - i - 1]);
    }
    for (auto i = N_BINS; i < NUM_LEDS; i++)
    {
      strip.setPixelColor(i, red[i - N_BINS + 1], blue[i - N_BINS + 1], green[i - N_BINS + 1]);
    }

    strip.show();
  }
}