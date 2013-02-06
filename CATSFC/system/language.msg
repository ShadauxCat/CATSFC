STARTCOMMENT

STARTVERSION
1.0
# 1. Lines starting with a # are comments.
# 2. No line may be longer than 250 bytes. As this file is UTF-8 encoded,
#    characters above U+007F take up 2 or more bytes.
# 3. If a line exceeds 250 characters, add the text "\n", without quotes,
#    then a newline. You may continue the text on the next line.
# 4. See the comment line ABOVE a message for its context. For example:
#  > #MSG_MAIN_MENU_SAVED_STATES
#  > Saved states
# 5. You may use *<Letter> in your message to insert a pictogram representing
#    a DS key. These replacements are case-sensitive.
#    Definitions are as follows:
#    *A  (A Button)  inserts  a circle containing the letter A
#    *B  (B Button)           a circle containing the letter B
#    *X  (X Button)           a circle containing the letter X
#    *Y  (Y Button)           a circle containing the letter Y
#    *L  (L shoulder button)  a cut rectangle containing the letter L
#    *R  (R shoulder button)  a cut rectangle containing the letter R
#    *S  (START button)       a smaller ST
#    *s  (SELECT button)      a smaller SEL
ENDVERSION

ENDCOMMENT

STARTENGLISH
#MSG_MAIN_MENU_NEW_GAME
New game
#MSG_MAIN_MENU_RETURN_TO_GAME
Return to game
#MSG_MAIN_MENU_RESET_GAME
Reset game
#MSG_MAIN_MENU_VIDEO_AUDIO
Video & audio
#MSG_MAIN_MENU_SAVED_STATES
Saved states
#MSG_MAIN_MENU_CHEATS
Cheats
#MSG_MAIN_MENU_TOOLS
Tools
#MSG_MAIN_MENU_OPTIONS
Options
#MSG_MAIN_MENU_EXIT
Exit
#FMT_VIDEO_AUDIO_FLUIDITY_PREFERENCE
Fluidity                %s
#MSG_VIDEO_AUDIO_FLUIDITY_PREFER_VIDEO
Prefer fluid video
#MSG_VIDEO_AUDIO_FLUIDITY_PREFER_AUDIO
Prefer fluid audio
#FMT_VIDEO_ASPECT_RATIO
Aspect ratio       %s
#FMT_VIDEO_FAST_FORWARD
Fast-forward     %s
#FMT_VIDEO_FRAME_SKIPPING
Frame skipping    %s
#FMT_AUDIO_SOUND
Sound                  %s
#FMT_SAVED_STATE_CREATE
Create saved state      %d
#FMT_SAVED_STATE_LOAD
Load saved state          %d
#MSG_SAVED_STATE_DELETE_GENERAL
Delete saved states...
#FMT_CHEAT_PAGE
Cheat code page %d
#MSG_CHEAT_LOAD_FROM_FILE
Load cheat file
#MSG_TOOLS_SCREENSHOT_GENERAL
Screenshots...
#MSG_TOOLS_GLOBAL_HOTKEY_GENERAL
Global hotkeys...
#MSG_TOOLS_GAME_HOTKEY_GENERAL
Game-specific hotkey overrides...
#FMT_OPTIONS_LANGUAGE
Language                          %s
#FMT_OPTIONS_CPU_FREQUENCY
CPU frequency                 %s
#MSG_OPTIONS_CPU_FREQUENCY_0
[0] 240 MHz
#MSG_OPTIONS_CPU_FREQUENCY_1
[1] 300 MHz
#MSG_OPTIONS_CPU_FREQUENCY_2
[2] 336 MHz
#MSG_OPTIONS_CPU_FREQUENCY_3
[3] 360 MHz
#MSG_OPTIONS_CPU_FREQUENCY_4
[4] 384 MHz
#MSG_OPTIONS_CPU_FREQUENCY_5
[5] 396 MHz
#MSG_OPTIONS_CARD_CAPACITY
Card capacity
#MSG_OPTIONS_RESET
Restore default settings
#MSG_OPTIONS_VERSION
Version information
#MSG_SCREENSHOT_CREATE
Save a screenshot
#MSG_SCREENSHOT_BROWSE
Browse screenshots
#MSG_HOTKEY_MAIN_MENU
Go to main menu
#MSG_HOTKEY_TEMPORARY_FAST_FORWARD
Temporary fast-forward
#MSG_HOTKEY_SOUND_TOGGLE
Toggle sound
#MSG_PROGRESS_HOTKEY_WAITING_FOR_KEYS
Press the buttons you wish to use to perform this action.
#MSG_HOTKEY_DELETE_WITH_A
*A Clear
#MSG_HOTKEY_CANCEL_WITH_B
*B Cancel
#MSG_LOAD_GAME_RECENTLY_PLAYED
Recently played games
#MSG_LOAD_GAME_FROM_CARD
Select a game from the card
#MSG_LOAD_GAME_MENU_TITLE
Select a game
#MSG_VIDEO_ASPECT_RATIO_0
[0] Entire screen
#MSG_VIDEO_ASPECT_RATIO_1
[1] Bottom, square pixels
#MSG_VIDEO_ASPECT_RATIO_2
[2] Top, square pixels
#MSG_VIDEO_ASPECT_RATIO_3
[3] Middle, square pixels
#MSG_VIDEO_ASPECT_RATIO_4
[4] Entire screen, smoothed
#MSG_VIDEO_FRAME_SKIPPING_AUTOMATIC
[-] Keep up with the game
#MSG_VIDEO_FRAME_SKIPPING_0
[0] Show all frames
#MSG_VIDEO_FRAME_SKIPPING_1
[1] Show 1 in 2 frames
#MSG_VIDEO_FRAME_SKIPPING_2
[2] Show 1 in 3 frames
#MSG_VIDEO_FRAME_SKIPPING_3
[3] Show 1 in 4 frames
#MSG_VIDEO_FRAME_SKIPPING_4
[4] Show 1 in 5 frames
#MSG_VIDEO_FRAME_SKIPPING_5
[5] Show 1 in 6 frames
#MSG_VIDEO_FRAME_SKIPPING_6
[6] Show 1 in 7 frames
#MSG_VIDEO_FRAME_SKIPPING_7
[7] Show 1 in 8 frames
#MSG_VIDEO_FRAME_SKIPPING_8
[8] Show 1 in 9 frames
#MSG_VIDEO_FRAME_SKIPPING_9
[9] Show 1 in 10 frames
#MSG_VIDEO_FRAME_SKIPPING_10
[10] Show 1 in 11 frames
#MSG_GENERAL_OFF
Off
#MSG_GENERAL_ON
On
#MSG_GENERAL_CONFIRM_WITH_A
*A Confirm
#MSG_GENERAL_CANCEL_WITH_B
*B Cancel
#MSG_AUDIO_ENABLED
On
#MSG_AUDIO_MUTED
Mute
#MSG_TOP_SCREEN_NO_GAME_LOADED
No game is currently loaded
#MSG_CHEAT_ELEMENT_NOT_LOADED
<None loaded here>
#MSG_SAVESTATE_FULL
The current slot holds a state. Do you wish to overwrite it?
#MSG_PROGRESS_SAVED_STATE_CREATING
Saving state...
#MSG_PROGRESS_SAVED_STATE_CREATION_FAILED
Failed!
#MSG_PROGRESS_SAVED_STATE_CREATION_SUCCEEDED
OK!
#MSG_TOP_SCREEN_NO_SAVED_STATE_IN_SLOT
No saved state here
#MSG_PROGRESS_SAVED_STATE_CORRUPTED
The game state file is not found or is corrupt.
#MSG_PROGRESS_SAVED_STATE_LOADING
Loading...
#MSG_PROGRESS_SAVED_STATE_LOAD_FAILED
Failed!
#MSG_PROGRESS_SAVED_STATE_LOAD_SUCCEEDED
OK!
#MSG_SAVED_STATE_DELETE_ALL
Delete all saved states
#FMT_SAVED_STATE_DELETE_ONE
Delete saved state %d
#MSG_DELETE_ALL_SAVESTATE_WARING
Really delete all saved states?
#FMT_DIALOG_SAVED_STATE_DELETE_ONE
Really delete saved state %d?
#MSG_PROGRESS_SAVED_STATE_ALREADY_EMPTY
There's already nothing here.
#MSG_PROGRESS_SCREENSHOT_CREATING
Saving a screenshot...
#MSG_PROGRESS_SCREENSHOT_CREATION_SUCCEEDED
OK!
#MSG_PROGRESS_SCREENSHOT_CREATION_FAILED
Failed!
#MSG_NO_SLIDE
No screenshots found
#MSG_PLAYING_SLIDE
Playing...
#MSG_PAUSE_SLIDE
Pause...
#MSG_PLAY_SLIDE1
Key UP    Speed up
#MSG_PLAY_SLIDE2
Key DOWN  Slow down
#MSG_PLAY_SLIDE3
Key LEFT  Last image
#MSG_PLAY_SLIDE4
Key RIGHT Next image
#MSG_PLAY_SLIDE5
Key A     Pause
#MSG_PLAY_SLIDE6
Key B     Return to the menu
#MSG_PROGRESS_LOADING_GAME
Loading...
#MSG_EMULATOR_NAME
CATSFC SNES emulator
#MSG_WORD_EMULATOR_VERSION
Version
#MSG_DIALOG_RESET
Reset all settings to defaults?
#MSG_PROGRESS_RESETTING
Please wait...

ENDENGLISH

STARTCHINESESIM
#MSG_MAIN_MENU_NEW_GAME
新游戏
#MSG_MAIN_MENU_RETURN_TO_GAME
返回游戏
#MSG_MAIN_MENU_RESET_GAME
重置游戏
#MSG_MAIN_MENU_VIDEO_AUDIO
图像声音
#MSG_MAIN_MENU_SAVED_STATES
存档
#MSG_MAIN_MENU_CHEATS
作弊器
#MSG_MAIN_MENU_TOOLS
工具
#MSG_MAIN_MENU_OPTIONS
综合设置
#MSG_MAIN_MENU_EXIT
退出模拟器
#FMT_VIDEO_AUDIO_FLUIDITY_PREFERENCE
Fluidity                     %s
#MSG_VIDEO_AUDIO_FLUIDITY_PREFER_VIDEO
Prefer fluid video
#MSG_VIDEO_AUDIO_FLUIDITY_PREFER_AUDIO
Prefer fluid audio
#FMT_VIDEO_ASPECT_RATIO
画面                         %s
#FMT_VIDEO_FAST_FORWARD
游戏快进                 %s
#FMT_VIDEO_FRAME_SKIPPING
跳帧方式                 %s
#FMT_AUDIO_SOUND
声音开关                 %s
#FMT_SAVED_STATE_CREATE
写入存档                 %d
#FMT_SAVED_STATE_LOAD
读取存档                 %d
#MSG_SAVED_STATE_DELETE_GENERAL
删除存档
#FMT_CHEAT_PAGE
作弊代码页   %d
#MSG_CHEAT_LOAD_FROM_FILE
加载作弊代码文件
#MSG_TOOLS_SCREENSHOT_GENERAL
屏幕快照
#MSG_TOOLS_GLOBAL_HOTKEY_GENERAL
Global hotkeys...
#MSG_TOOLS_GAME_HOTKEY_GENERAL
Game-specific hotkey overrides...
#FMT_OPTIONS_LANGUAGE
语言                               %s
#FMT_OPTIONS_CPU_FREQUENCY
CPU主频                          %s
#MSG_OPTIONS_CPU_FREQUENCY_0
[0] 240 MHz
#MSG_OPTIONS_CPU_FREQUENCY_1
[1] 300 MHz
#MSG_OPTIONS_CPU_FREQUENCY_2
[2] 336 MHz
#MSG_OPTIONS_CPU_FREQUENCY_3
[3] 360 MHz
#MSG_OPTIONS_CPU_FREQUENCY_4
[4] 384 MHz
#MSG_OPTIONS_CPU_FREQUENCY_5
[5] 396 MHz
#MSG_OPTIONS_CARD_CAPACITY
卡容量
#MSG_OPTIONS_RESET
缺省设置
#MSG_OPTIONS_VERSION
版本信息
#MSG_SCREENSHOT_CREATE
保存屏幕快照
#MSG_SCREENSHOT_BROWSE
浏览屏幕快照
#MSG_HOTKEY_MAIN_MENU
Go to main menu
#MSG_HOTKEY_TEMPORARY_FAST_FORWARD
Temporary fast-forward
#MSG_HOTKEY_SOUND_TOGGLE
Toggle sound
#MSG_PROGRESS_HOTKEY_WAITING_FOR_KEYS
Press the buttons you wish to use to perform this action.
#MSG_HOTKEY_DELETE_WITH_A
*A Clear
#MSG_HOTKEY_CANCEL_WITH_B
*B Cancel
#MSG_LOAD_GAME_RECENTLY_PLAYED
最近玩过的游戏
#MSG_LOAD_GAME_FROM_CARD
新游戏
#MSG_LOAD_GAME_MENU_TITLE
装载游戏
#MSG_VIDEO_ASPECT_RATIO_0
显示模式 0
#MSG_VIDEO_ASPECT_RATIO_1
显示模式 1
#MSG_VIDEO_ASPECT_RATIO_2
显示模式 2
#MSG_VIDEO_ASPECT_RATIO_3
显示模式 3
#MSG_VIDEO_ASPECT_RATIO_4
显示模式 4
#MSG_VIDEO_FRAME_SKIPPING_AUTOMATIC
[-] Keep up with the game
#MSG_VIDEO_FRAME_SKIPPING_0
[0] Show all frames
#MSG_VIDEO_FRAME_SKIPPING_1
[1] Show 1 in 2 frames
#MSG_VIDEO_FRAME_SKIPPING_2
[2] Show 1 in 3 frames
#MSG_VIDEO_FRAME_SKIPPING_3
[3] Show 1 in 4 frames
#MSG_VIDEO_FRAME_SKIPPING_4
[4] Show 1 in 5 frames
#MSG_VIDEO_FRAME_SKIPPING_5
[5] Show 1 in 6 frames
#MSG_VIDEO_FRAME_SKIPPING_6
[6] Show 1 in 7 frames
#MSG_VIDEO_FRAME_SKIPPING_7
[7] Show 1 in 8 frames
#MSG_VIDEO_FRAME_SKIPPING_8
[8] Show 1 in 9 frames
#MSG_VIDEO_FRAME_SKIPPING_9
[9] Show 1 in 10 frames
#MSG_VIDEO_FRAME_SKIPPING_10
[10] Show 1 in 11 frames
#MSG_GENERAL_OFF
关
#MSG_GENERAL_ON
开
#MSG_GENERAL_CONFIRM_WITH_A
*A 确认
#MSG_GENERAL_CANCEL_WITH_B
*B 取消
#MSG_AUDIO_ENABLED
开
#MSG_AUDIO_MUTED
静音
#MSG_TOP_SCREEN_NO_GAME_LOADED
没有加载游戏
#MSG_CHEAT_ELEMENT_NOT_LOADED
<没有加载>
#MSG_SAVESTATE_FULL
当前存档槽已有存档文件,覆盖已有存档吗?
#MSG_PROGRESS_SAVED_STATE_CREATING
正在写入...
#MSG_PROGRESS_SAVED_STATE_CREATION_FAILED
写入存档失败!
#MSG_PROGRESS_SAVED_STATE_CREATION_SUCCEEDED
写入存档成功!
#MSG_TOP_SCREEN_NO_SAVED_STATE_IN_SLOT
没有游戏截图
#MSG_PROGRESS_SAVED_STATE_CORRUPTED
存档文件找不到或者损坏!
#MSG_PROGRESS_SAVED_STATE_LOADING
正在加载...
#MSG_PROGRESS_SAVED_STATE_LOAD_FAILED
加载存档失败
#MSG_PROGRESS_SAVED_STATE_LOAD_SUCCEEDED
加载存档成功
#MSG_SAVED_STATE_DELETE_ALL
删除当前游戏的所有存档
#FMT_SAVED_STATE_DELETE_ONE
删除当前游戏的一个存档         %d
#MSG_DIALOG_SAVED_STATE_DELETE_ALL
确定要删除所有存档吗？
#FMT_DIALOG_SAVED_STATE_DELETE_ONE
确定要删除存档槽 %d 的存档吗?
#MSG_PROGRESS_SAVED_STATE_ALREADY_EMPTY
没有存档可删除
#MSG_PROGRESS_SCREENSHOT_CREATING
正在保存屏幕快照...
#MSG_PROGRESS_SCREENSHOT_CREATION_SUCCEEDED
完成!
#MSG_PROGRESS_SCREENSHOT_CREATION_FAILED
失败!
#MSG_NO_SLIDE
此目录没有屏幕快照，取消放映
#MSG_PLAYING_SLIDE
正在播放
#MSG_PAUSE_SLIDE
已暂停
#MSG_PLAY_SLIDE1
按键 上  加快播放
#MSG_PLAY_SLIDE2
按键 下  减慢播放
#MSG_PLAY_SLIDE3
按键 左  上一帧
#MSG_PLAY_SLIDE4
按键 右  下一帧
#MSG_PLAY_SLIDE5
按键 *A   暂停
#MSG_PLAY_SLIDE6
按键 *B   返回菜单
#MSG_PROGRESS_LOADING_GAME
正在装载游戏...
#MSG_EMULATOR_NAME
SFC模拟器
#MSG_WORD_EMULATOR_VERSION
版本
#MSG_DIALOG_RESET
确定将 SFC模拟器恢复为出厂设置吗?
#MSG_PROGRESS_RESETTING
正在设置,请稍候...

ENDCHINESESIM

STARTFRENCH
#MSG_MAIN_MENU_NEW_GAME
Nouveau jeu
#MSG_MAIN_MENU_RETURN_TO_GAME
Retour au jeu
#MSG_MAIN_MENU_RESET_GAME
Redémarrer jeu
#MSG_MAIN_MENU_VIDEO_AUDIO
Vidéo & audio
#MSG_MAIN_MENU_SAVED_STATES
Sauvegardes
#MSG_MAIN_MENU_CHEATS
Triche
#MSG_MAIN_MENU_TOOLS
Outils
#MSG_MAIN_MENU_OPTIONS
Options
#MSG_MAIN_MENU_EXIT
Quitter
#FMT_VIDEO_AUDIO_FLUIDITY_PREFERENCE
Fluidité                   %s
#MSG_VIDEO_AUDIO_FLUIDITY_PREFER_VIDEO
Préférer la vidéo fluide
#MSG_VIDEO_AUDIO_FLUIDITY_PREFER_AUDIO
Préférer l'audio fluide
#FMT_VIDEO_ASPECT_RATIO
Format d'image     %s
#FMT_VIDEO_FAST_FORWARD
Avance rapide       %s
#FMT_VIDEO_FRAME_SKIPPING
Omission d'images  %s
#FMT_AUDIO_SOUND
Son                         %s
#FMT_SAVED_STATE_CREATE
Créer la sauvegarde       %d
#FMT_SAVED_STATE_LOAD
Charger la sauvegarde   %d
#MSG_SAVED_STATE_DELETE_GENERAL
Supprimer une/des sauvegardes...
#FMT_CHEAT_PAGE
Page de codes de triche : %d
#MSG_CHEAT_LOAD_FROM_FILE
Charger un fichier de triche
#MSG_TOOLS_SCREENSHOT_GENERAL
Captures d'écran...
#MSG_TOOLS_GLOBAL_HOTKEY_GENERAL
Raccourcis globaux...
#MSG_TOOLS_GAME_HOTKEY_GENERAL
Raccourcis pour le jeu actuel...
#FMT_OPTIONS_LANGUAGE
Langue                              %s
#FMT_OPTIONS_CPU_FREQUENCY
Fréquence CPU                  %s
#MSG_OPTIONS_CPU_FREQUENCY_0
[0] 240 MHz
#MSG_OPTIONS_CPU_FREQUENCY_1
[1] 300 MHz
#MSG_OPTIONS_CPU_FREQUENCY_2
[2] 336 MHz
#MSG_OPTIONS_CPU_FREQUENCY_3
[3] 360 MHz
#MSG_OPTIONS_CPU_FREQUENCY_4
[4] 384 MHz
#MSG_OPTIONS_CPU_FREQUENCY_5
[5] 396 MHz
#MSG_OPTIONS_CARD_CAPACITY
Capacité de la carte
#MSG_OPTIONS_RESET
Remettre les paramètres à zéro
#MSG_OPTIONS_VERSION
Version
#MSG_SCREENSHOT_CREATE
Créer une capture d'écran
#MSG_SCREENSHOT_BROWSE
Parcourir les captures d'écran
#MSG_HOTKEY_MAIN_MENU
Menu principal
#MSG_HOTKEY_TEMPORARY_FAST_FORWARD
Avance rapide temporaire
#MSG_HOTKEY_SOUND_TOGGLE
(Dés)activer le son
#MSG_PROGRESS_HOTKEY_WAITING_FOR_KEYS
Appuyez sur les boutons que vous voulez utiliser pour effectuer cette action.
#MSG_HOTKEY_DELETE_WITH_A
*A Effacer
#MSG_HOTKEY_CANCEL_WITH_B
*B Annuler
#MSG_LOAD_GAME_RECENTLY_PLAYED
Jeux joués récemment
#MSG_LOAD_GAME_FROM_CARD
Sélectionner un jeu de la carte
#MSG_LOAD_GAME_MENU_TITLE
Sélectionner un jeu
#MSG_VIDEO_ASPECT_RATIO_0
[0] Écran entier
#MSG_VIDEO_ASPECT_RATIO_1
[1] Bas, pixels carrés
#MSG_VIDEO_ASPECT_RATIO_2
[2] Haut, pixels carrés
#MSG_VIDEO_ASPECT_RATIO_3
[3] Milieu, pixels carrés
#MSG_VIDEO_ASPECT_RATIO_4
[4] Écran entier lissé
#MSG_VIDEO_FRAME_SKIPPING_AUTOMATIC
[-] Suivre le jeu
#MSG_VIDEO_FRAME_SKIPPING_0
[0] N'omettre aucune image
#MSG_VIDEO_FRAME_SKIPPING_1
[1] Montrer 1 image sur 2
#MSG_VIDEO_FRAME_SKIPPING_2
[2] Montrer 1 image sur 3
#MSG_VIDEO_FRAME_SKIPPING_3
[3] Montrer 1 image sur 4
#MSG_VIDEO_FRAME_SKIPPING_4
[4] Montrer 1 image sur 5
#MSG_VIDEO_FRAME_SKIPPING_5
[5] Montrer 1 image sur 6
#MSG_VIDEO_FRAME_SKIPPING_6
[6] Montrer 1 image sur 7
#MSG_VIDEO_FRAME_SKIPPING_7
[7] Montrer 1 image sur 8
#MSG_VIDEO_FRAME_SKIPPING_8
[8] Montrer 1 image sur 9
#MSG_VIDEO_FRAME_SKIPPING_9
[9] Montrer 1 image sur 10
#MSG_VIDEO_FRAME_SKIPPING_10
[10] Montrer 1 image sur 11
#MSG_GENERAL_OFF
Hors fonction
#MSG_GENERAL_ON
En fonction
#MSG_GENERAL_CONFIRM_WITH_A
*A Confirmer
#MSG_GENERAL_CANCEL_WITH_B
*B Annuler
#MSG_AUDIO_ENABLED
Activé
#MSG_AUDIO_MUTED
Muet
#MSG_TOP_SCREEN_NO_GAME_LOADED
Aucun jeu n'est actuellement chargé
#MSG_CHEAT_ELEMENT_NOT_LOADED
<Aucun chargé ici>
#MSG_SAVESTATE_FULL
Il y a une sauvegarde dans cet emplacement. Voulez-vous l'écraser?
#MSG_PROGRESS_SAVED_STATE_CREATING
Sauvegarde en cours...
#MSG_PROGRESS_SAVED_STATE_CREATION_FAILED
Échec!
#MSG_PROGRESS_SAVED_STATE_CREATION_SUCCEEDED
Terminé!
#MSG_TOP_SCREEN_NO_SAVED_STATE_IN_SLOT
Aucune sauvegarde ici
#MSG_PROGRESS_SAVED_STATE_CORRUPTED
Le fichier de sauvegarde est introuvable ou corrompu.
#MSG_PROGRESS_SAVED_STATE_LOADING
Chargement en cours...
#MSG_PROGRESS_SAVED_STATE_LOAD_FAILED
Échec!
#MSG_PROGRESS_SAVED_STATE_LOAD_SUCCEEDED
Terminé!
#MSG_SAVED_STATE_DELETE_ALL
Supprimer toutes les sauvegardes
#FMT_SAVED_STATE_DELETE_ONE
Supprimer l'emplacement %d
#MSG_DELETE_ALL_SAVESTATE_WARING
Supprimer toutes les sauvegardes?
#FMT_DIALOG_SAVED_STATE_DELETE_ONE
Supprimer l'emplacement %d?
#MSG_PROGRESS_SAVED_STATE_ALREADY_EMPTY
Cet emplacement est déjà vide.
#MSG_PROGRESS_SCREENSHOT_CREATING
Capture de l'écran...
#MSG_PROGRESS_SCREENSHOT_CREATION_SUCCEEDED
Terminé!
#MSG_PROGRESS_SCREENSHOT_CREATION_FAILED
Échec!
#MSG_NO_SLIDE
Aucune capture d'écran trouvée
#MSG_PLAYING_SLIDE
En cours...
#MSG_PAUSE_SLIDE
Pause...
#MSG_PLAY_SLIDE1
HAUT     Accélérer
#MSG_PLAY_SLIDE2
BAS       Ralentir
#MSG_PLAY_SLIDE3
GAUCHE  Précédente
#MSG_PLAY_SLIDE4
DROITE  Suivante
#MSG_PLAY_SLIDE5
*A           Pause
#MSG_PLAY_SLIDE6
*B           Retour au menu
#MSG_PROGRESS_LOADING_GAME
Chargement...
#MSG_EMULATOR_NAME
Émulateur SNES CATSFC
#MSG_WORD_EMULATOR_VERSION
Version
#MSG_DIALOG_RESET
Remettre tous les paramètres à zéro?
#MSG_PROGRESS_RESETTING
Veuillez patienter...

ENDFRENCH
