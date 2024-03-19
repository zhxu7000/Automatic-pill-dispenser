#include <Tone32.h>

const int BUZZER_PIN = 16;
const int BUZZER_CHANNEL = 1;

const int alarm_notes[] = {
  NOTE_E4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_F4,
  NOTE_E4, NOTE_D4, NOTE_C4, NOTE_C4, NOTE_D4, NOTE_E4,
  NOTE_E4, NOTE_D4, NOTE_D4
};

void ringBuzzer(const int* melody, int size) {
  for(int i = 0; i < size; i++) {
    if(i < size) {
      tone(BUZZER_PIN, melody[i], 500, BUZZER_CHANNEL);
      noTone(BUZZER_PIN, BUZZER_CHANNEL);
    }
  }
}