// The sound buffer sizes used on the DS2's side, for each value of
// Settings.SoundPlaybackRate.
#define DS2_BUFFER_SIZE_4  512 /* tested working */
#define DS2_BUFFER_SIZE_5 1024 /* like the SNES! tested working, but slow */
#define DS2_BUFFER_SIZE_6 1024 /* tested working, slow because of upsampling */
#define DS2_BUFFER_SIZE_7 1024 /* tested working, slow because of upsampling */

// In microseconds.
// (512 samples / 22050 samples/sec) = 23000 microseconds.
// The value needs to be:
// * not too low, because then every time a new batch of close to 512 samples
//   is created anew, and that's EXPENSIVE.
// * not too high, because then missing the end of the buffer creates audible
//   underflows.
// * not exactly 16667 (NTSC 60 FPS) or 20000 (PAL 50 FPS), because then
//   the automatic frame skipper will be extremely messed up.
#define INTERRUPT_TIME_4 15360 /* 15360 tested working */
#define INTERRUPT_TIME_5  8000 /* like the SNES! tested working, but slow */
#define INTERRUPT_TIME_6  4000 /* tested working, slow because of upsampling */
#define INTERRUPT_TIME_7  4000 /* tested working, slow because of upsampling */

// The sampling rate for the sound, in Hz, for each value of
// Settings.SoundPlaybackRate.
#define SND_SAMPLE_RATE_1 8000
#define SND_SAMPLE_RATE_2 11025
#define SND_SAMPLE_RATE_3 16000
#define SND_SAMPLE_RATE_4 22050
#define SND_SAMPLE_RATE_5 32000
#define SND_SAMPLE_RATE_6 44100
#define SND_SAMPLE_RATE_7 48000

// Settings in use. The number should match in all three settings.
#define INTERRUPT_TIME    INTERRUPT_TIME_4
#define DS2_BUFFER_SIZE   DS2_BUFFER_SIZE_4
#define SND_SAMPLE_RATE   SND_SAMPLE_RATE_4
#define SNES9X_SRATE_ID   4
