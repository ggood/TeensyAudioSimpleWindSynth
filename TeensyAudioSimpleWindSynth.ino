/**
 * This is a simple wind controller that uses a pressure sensor
 * as a breath controller to control a Teensy Audio Board as the
 * audio source, while using a MIDI keyboard to select notes.
 * I suppose one could package these together into an electronic
 * melodicon. The Teensy Audio board generates the sound, which is
 * just a simple sine wave oscillator.
 */

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=334,401
AudioOutputI2S           i2s1;           //xy=509,400
AudioConnection          patchCord1(waveform1, 0, i2s1, 0);
AudioConnection          patchCord2(waveform1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=594,468
// GUItool: end automatically generated code

byte currentNote = 0;  // The currently sounding note

// Convert MIDI note to frequency. Borrowed from internet
double noteToFrequency(byte note) {
  //freq = 440 * 2^((n-58) / 12)
  return 440.0 * pow(2, ((note - 58) / 12.0));;
}

// Handler for note on events.
void onNoteOn(byte channel, byte note, byte velocity) {
  // Remember the note, and set the oscillator frequency
  currentNote = note;
  waveform1.frequency(noteToFrequency(note));
}


// Handler for note off events.
void onNoteOff(byte channel, byte note, byte velocity) {
  // If the currently sounding note was released, stop making
  // any sound
  if (note == currentNote) {
    currentNote = 0;
    waveform1.frequency(0);
  }
}

void setup() {
  AudioMemory(20);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
  
  waveform1.amplitude(0.5);
  waveform1.begin(WAVEFORM_SINE);
  
  usbMIDI.setHandleNoteOn(onNoteOn);
  usbMIDI.setHandleNoteOff(onNoteOff);
}

void loop() {
  usbMIDI.read();
  // Read pressure sensor
  int val = analogRead(0);
  // My sensor produces a value of about 75 with no air. So ignore
  // values less than, say, 100.
  if (val < 100) {
    return;
  }
  
  // Map the sensor values to a range 0.0 - 0.1...
  double volume = map(val, 100, 1023, 0, 1000) / 1000.0;
  // ...and set the oscillator volume
  waveform1.amplitude(volume);
}
