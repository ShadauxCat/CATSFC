#ifdef __cplusplus
extern "C" {
#endif
	int game_load_state(char* file);
	int game_save_state(char* file);
	void S9xAutoSaveSRAM ();

	void game_restart(void);

	int load_gamepak(const char* file);

	extern unsigned int AutoCPUFrequency;
#ifdef __cplusplus
}
#endif

const char *S9xGetFilename (const char *ex);
