// The sound buffer sizes used on the DS2's side, for each value of
// Settings.SoundPlaybackRate.
#define DS2_BUFFER_SIZE_4  512 /* tested working */
#define DS2_BUFFER_SIZE_5 1024 /* like the SNES! tested working */
#define DS2_BUFFER_SIZE_6 1024 /* tested working */
#define DS2_BUFFER_SIZE_7 1024 /* tested working */

// In microseconds.
#define INTERRUPT_TIME_4  4000 /* tested working */
#define INTERRUPT_TIME_5  5000 /* like the SNES! underflows at 4000 */
#define INTERRUPT_TIME_6  4000 /* tested working */
#define INTERRUPT_TIME_7  4000 /* tested working */

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
#define INTERRUPT_TIME    INTERRUPT_TIME_7
#define DS2_BUFFER_SIZE   DS2_BUFFER_SIZE_7
#define SND_SAMPLE_RATE   SND_SAMPLE_RATE_7
#define SNES9X_SRATE_ID   7
