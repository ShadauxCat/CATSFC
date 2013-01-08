// The sound buffer sizes used on the DS2's side, for each value of
// Settings.SoundPlaybackRate.
#define DS2_BUFFER_SIZE_4  512 /* tested working */
// Others don't work, or don't work well.

// In microseconds.
#define INTERRUPT_TIME_4  2000 /* tested working */
// Others don't work, or don't work well.

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
