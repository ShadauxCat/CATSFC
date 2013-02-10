// The sound buffer sizes used on the DS2's side, for each value of
// Settings.SoundPlaybackRate.
// * Don't buffer too much, otherwise audio is delayed from video.
// * Don't go below one frame (20 milliseconds).
// * Buffer sizes must be multiples of 128.
#define DS2_BUFFER_SIZE_4  512 /* tested working */
#define DS2_BUFFER_SIZE_5  640 /* like the SNES! tested working */
#define DS2_BUFFER_SIZE_6  896 /* tested working */
#define DS2_BUFFER_SIZE_7 1024 /* tested working */

// The sampling rate for the sound, in Hz, for each value of
// Settings.SoundPlaybackRate.
#define SND_SAMPLE_RATE_1 8000
#define SND_SAMPLE_RATE_2 11025
#define SND_SAMPLE_RATE_3 16000
#define SND_SAMPLE_RATE_4 22050 /* NDSSFC 1.06 - CATSFC 1.28 used this one */
#define SND_SAMPLE_RATE_5 32000 /* like the SNES! */
#define SND_SAMPLE_RATE_6 44100
#define SND_SAMPLE_RATE_7 48000 /* CATSFC 1.25 made using this one possible */

// Settings in use. The number should match in all three settings.
#define DS2_BUFFER_SIZE   DS2_BUFFER_SIZE_5
#define SND_SAMPLE_RATE   SND_SAMPLE_RATE_5
#define SNES9X_SRATE_ID   5
