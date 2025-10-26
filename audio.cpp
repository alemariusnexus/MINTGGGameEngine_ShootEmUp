#include "audio.h"


AudioClip createBackgroundAudio()
{
  AudioClip bgmClip;

  bgmClip.setTempo(2, 150);
  bgmClip.setNoteEndReleaseDuration(0.05f);

  // Base: Eighth note

  // Measure 1
  bgmClip.note(NOTE_E5, 2);
  bgmClip.note(NOTE_B4, 1);
  bgmClip.note(NOTE_C5, 1);
  bgmClip.note(NOTE_D5, 2);
  bgmClip.note(NOTE_C5, 1);
  bgmClip.note(NOTE_B4, 1);

  // Measure 2
  bgmClip.note(NOTE_A4, 2);
  bgmClip.note(NOTE_A4, 1);
  bgmClip.note(NOTE_C5, 1);
  bgmClip.note(NOTE_E5, 2);
  bgmClip.note(NOTE_D5, 1);
  bgmClip.note(NOTE_C5, 1);

  // Measure 3
  bgmClip.note(NOTE_B4, 3);
  bgmClip.note(NOTE_C5, 1);
  bgmClip.note(NOTE_D5, 2);
  bgmClip.note(NOTE_E5, 2);

  // Measure 4
  bgmClip.note(NOTE_C5, 2);
  bgmClip.note(NOTE_A4, 2);
  bgmClip.note(NOTE_A4, 2);
  bgmClip.pause(2);

  // Measure 5
  bgmClip.pause(1);
  bgmClip.note(NOTE_D5, 2);
  bgmClip.note(NOTE_F5, 1);
  bgmClip.note(NOTE_A5, 2);
  bgmClip.note(NOTE_G5, 1);
  bgmClip.note(NOTE_F5, 1);

  // Measure 6
  bgmClip.note(NOTE_E5, 3);
  bgmClip.note(NOTE_C5, 1);
  bgmClip.note(NOTE_E5, 2);
  bgmClip.note(NOTE_D5, 1);
  bgmClip.note(NOTE_C5, 1);

  // Measure 7
  bgmClip.note(NOTE_B4, 3);
  bgmClip.note(NOTE_C5, 1);
  bgmClip.note(NOTE_D5, 2);
  bgmClip.note(NOTE_E5, 2);

  // Measure 8
  bgmClip.note(NOTE_C5, 2);
  bgmClip.note(NOTE_A4, 2);
  bgmClip.note(NOTE_A4, 2);
  bgmClip.pause(2);

  // Measure 9
  bgmClip.note(NOTE_E5, 4);
  bgmClip.note(NOTE_C5, 4);

  // Measure 10
  bgmClip.note(NOTE_D5, 4);
  bgmClip.note(NOTE_B4, 4);

  // Measure 11
  bgmClip.note(NOTE_C5, 4);
  bgmClip.note(NOTE_A4, 4);

  // Measure 12
  bgmClip.note(NOTE_Gs4, 8);

  // Measure 13
  bgmClip.note(NOTE_E5, 4);
  bgmClip.note(NOTE_C5, 4);

  // Measure 14
  bgmClip.note(NOTE_D5, 4);
  bgmClip.note(NOTE_B4, 4);

  // Measure 15
  bgmClip.note(NOTE_C5, 2);
  bgmClip.note(NOTE_E5, 2);
  bgmClip.note(NOTE_A5, 2);
  bgmClip.note(NOTE_A5, 2);

  // Measure 16
  bgmClip.note(NOTE_Gs5, 8);

  // Measure 17
  bgmClip.note(NOTE_E5, 2);
  bgmClip.note(NOTE_B4, 1);
  bgmClip.note(NOTE_C5, 1);
  bgmClip.note(NOTE_D5, 2);
  bgmClip.note(NOTE_C5, 1);
  bgmClip.note(NOTE_B4, 1);

  // Measure 18
  bgmClip.note(NOTE_A4, 2);
  bgmClip.note(NOTE_A4, 1);
  bgmClip.note(NOTE_C5, 1);
  bgmClip.note(NOTE_E5, 2);
  bgmClip.note(NOTE_D5, 1);
  bgmClip.note(NOTE_C5, 1);

  // Measure 19
  bgmClip.note(NOTE_B4, 3);
  bgmClip.note(NOTE_C5, 1);
  bgmClip.note(NOTE_D5, 2);
  bgmClip.note(NOTE_E5, 2);

  // Measure 20
  bgmClip.note(NOTE_C5, 2);
  bgmClip.note(NOTE_A4, 2);
  bgmClip.note(NOTE_A4, 2);
  bgmClip.pause(2);

  // Measure 21
  bgmClip.pause(1);
  bgmClip.note(NOTE_D5, 2);
  bgmClip.note(NOTE_F5, 1);
  bgmClip.note(NOTE_A5, 2);
  bgmClip.note(NOTE_G5, 1);
  bgmClip.note(NOTE_F5, 1);

  // Measure 22
  bgmClip.pause(1);
  bgmClip.note(NOTE_E5, 2);
  bgmClip.note(NOTE_C5, 1);
  bgmClip.note(NOTE_E5, 2);
  bgmClip.note(NOTE_D5, 1);
  bgmClip.note(NOTE_C5, 1);

  // Measure 23
  bgmClip.pause(1);
  bgmClip.note(NOTE_B4, 2);
  bgmClip.note(NOTE_C5, 1);
  bgmClip.note(NOTE_D5, 2);
  bgmClip.note(NOTE_E5, 2);

  // Measure 24
  bgmClip.pause(1);
  bgmClip.note(NOTE_C5, 2);
  bgmClip.note(NOTE_A4, 1);
  bgmClip.note(NOTE_A4, 2);
  bgmClip.pause(2);

  return bgmClip;
}



AudioClip createShootAudio() {
  AudioClip clip;
  clip.setTempo(800);
  clip.note(NOTE_C7, 1);
  return clip;
}

AudioClip createAsteroidHitAudio() {
  AudioClip clip;
  clip.setTempo(300);
  clip.note(NOTE_G2, 2);
  return clip;
}

AudioClip createPlayerHitAudio() {
  AudioClip clip;
  clip.setTempo(200);
  clip.note(NOTE_G4, 1);
  clip.note(NOTE_Ds4, 1);
  clip.note(NOTE_C4, 1);
  clip.pause(3);
  return clip;
}

AudioClip createGameOverAudio() {
  AudioClip clip;
  clip.setTempo(220);
  clip.note(NOTE_G4, 1);
  clip.note(NOTE_Ds4, 1);
  clip.note(NOTE_C4, 1);
  clip.note(NOTE_G3, 1);
  clip.note(NOTE_Ds3, 1);
  clip.note(NOTE_C3, 1);
  clip.note(NOTE_G2, 1);
  clip.note(NOTE_Ds2, 1);
  clip.note(NOTE_C2, 1);
  return clip;
}
