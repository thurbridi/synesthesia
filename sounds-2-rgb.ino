#include <Audio.h>
#include <Adafruit_NeoPixel.h>

AudioInputUSB        audioInput;           //xy=200,69  (must set Tools > USB Type to Audio)
AudioAnalyzePeak     peak_L;
AudioAnalyzePeak     peak_R;
AudioOutputI2S out1;

AudioConnection c1(audioInput,0,peak_L,0);
AudioConnection c2(audioInput,1,peak_R,0);

const auto LED_PIN = 0;
const auto NUM_LEDS = 19;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  AudioMemory(12);
  pinMode(LED_PIN, OUTPUT);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  delay(5000);
}

elapsedMillis fps;

void loop() {
  if(fps > 24) {
    if (peak_L.available() && peak_R.available()) {
      fps=0;
      float leftPeak = peak_L.read();
      float rightPeak = peak_R.read();

      byte output = ((leftPeak + rightPeak) / 2) * 255;

      for(auto i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, output, 0, 0);
      }

      strip.show();
    }
  }
}
