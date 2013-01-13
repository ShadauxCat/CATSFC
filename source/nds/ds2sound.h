// The sound buffer sizes used on the DS2's side, for each value of
// Settings.SoundPlaybackRate.
#define DS2_BUFFER_SIZE_4  512 /* tested working */
#define DS2_BUFFER_SIZE_5 1024 /* like the SNES! tested working, but slow */
#define DS2_BUFFER_SIZE_6 1024 /* tested working, slow because of upsampling */
#define DS2_BUFFER_SIZE_7 1024 /* tested working, slow because of upsampling */

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
#define DS2_BUFFER_SIZE   DS2_BUFFER_SIZE_4
#define SND_SAMPLE_RATE   SND_SAMPLE_RATE_4
#define SNES9X_SRATE_ID   4
