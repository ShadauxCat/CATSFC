#ifdef __cplusplus
extern "C" {
#endif
	void game_disableAudio();
	void game_set_frameskip();
	void game_set_fluidity();
	void game_set_retro();

	int game_load_state(char* file);
	int game_save_state(char* file);
	void S9xAutoSaveSRAM ();

	void game_restart(void);

	int load_gamepak(const char* file);
#ifdef __cplusplus
}
#endif

const char *S9xGetFilename (const char *ex);
