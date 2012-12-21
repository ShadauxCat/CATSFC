CATSFC version 1.08, 2012-12-20

A Super Nintendo emulator for the Supercard DSTWO.

Based on:
* Snes9x 1.43, by the Snes9x team (with research by the ZSNES folks, anomie,
  zsKnight, etc.)
* NDSSFC 1.06, by the Supercard team (porting to the MIPS processor)
* BAGSFC, by BassAceGold (improving over NDSSFC)
* CATSFC, by ShadauxCat (improving over BAGSFC)

# Compiling

Compiling CATSFC is best done on Linux. Make sure you have access to a Linux
system to perform these steps.

## The DS2 SDK
To compile CATSFC, you need to have the Supercard team's DS2 SDK.
The Makefile expects it at `/opt/ds2sdk`, but you can move it anywhere,
provided that you update the Makefile's `DS2SDKPATH` variable to point to it.

For best results, download version 0.13 of the DS2 SDK, which will have the
MIPS compiler (`gcc`), extract it to `/opt/ds2sdk`, follow the instructions,
then download version 1.2 of the DS2 SDK and extract its files into
`opt/ds2sdk`, overwriting version 0.13.

## The MIPS compiler (`gcc`)
You also need the MIPS compiler from the DS2 SDK.
The Makefile expects it at `/opt/mipsel-4.1.2-nopic`, but you can move it
anywhere, provided that you update the Makefile's `CROSS` variable to point to
it.

## Making the plugin
To make the plugin, `catsfc.plg`, use the `cd` command to change to the
directory containing your copy of the CATSFC source, then type
`make clean; make`. `catsfc.plg` should appear in the same directory.

# Installing

To install the plugin to your storage card after compiling it, copy
`catsfc.plg`, `catsfc.ini` and `catsfc.bmp` to the card's `_dstwoplug`
directory. Then, copy the source directory's CATSFC subdirectory to the
root of the card.

# The font

The font used by CATSFC is now similar to the Pictochat font. To modify it,
see `source/font/README.txt`.

# Translations

Translations for CATSFC may be submitted to the author(s) under many forms,
one of which is the Github pull request. To complete a translation, you will
need to do the following:

* Open `CATSFC/system/language.msg`.
* Copy what's between `STARTENGLISH` and `ENDENGLISH` and paste it at the end
  of the file.
* Change the tags. For example, if you want to translate to German, the tags
  will become `STARTGERMAN` and `ENDGERMAN`.
* Translate each of the messages, using the lines starting with `#MSG_` as a
  guide to the context in which the messages will be used.
* Edit `source/nds/message.h`. Find `enum LANGUAGE` and add the name of your
  language there. For the example of German, you would add this at the end of
  the list:
  ```
	,
	GERMAN
  ```
* Still in `source/nds/message.h`, just below `enum LANGUAGE`, you will find
  `extern char* lang[` *some number* `]`. Add 1 to that number.
* Edit `source/nds/gui.c`. Find `char *lang[` *some number* `] =`.
  Add the name of your language, in the language itself. For the example of
  German, you would add this at the end of the list:
  ```
	,
	"Deutsch"
  ```
* Still in `source/nds/gui.c`, find `char* language_options[]`, which is below
  the language names. Add an entry similar to the others, with the last number
  plus 1. For example, if the last entry is `, (char *) &lang[2]`, yours would
  be `, (char *) &lang[3]`.
* Still in `source/nds/gui.c`, find `case CHINESE_SIMPLIFIED`. Copy the lines
  starting at the `case` and ending with `break`, inclusively. Paste them
  before the `}`. Change the language name and tags, and update the `cmplen`
  to be the length of the `START` tag. For the example of German, you would
  use `cmplen= 11;`, because that's the length of `STARTGERMAN`:
  ```
	case GERMAN:
		strcpy(start, "STARTGERMAN");
		strcpy(end, "ENDGERMAN");
		cmplen = 11;
		break;
  ```

Compile again, copy the plugin and your new `language.msg` to your card
under `CATSFC/system`, and you can now select your new language in CATSFC!
