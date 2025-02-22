# type-trainer

Typing words displays receive accuracy(% of letters) and speed(words per minute).

## Dependencies
```gcc make words gnu-free-fonts sdl3 sdl3_ttf```


## Configuration

Configuration file is file is by default on "~/.config/type-trainer/config.txt".

## Config file syntax

Format is `key=value`.

Value can be
- int32
- string (with or without "quotes")
- boolean (with "true", "false", true, false, 0, or 1)
- color (R, G, B, A)

## Config file options

### Dictionary

The file where the random words are picked.

`dictionary=/usr/share/dict/spanish`

### Font

The application font.

`font=/usr/share/fonts/gnu-free/FreeMono.otf`

### Font size

The size of the font

`font_size=28`

### Total words

The amount of words displayed at once in the application.

`total_words=25`

### Advance on failure

Should the player move the next letter even if there was an mistake in the typing.

`advance_on_failure=true`

### Background color

The background color of the application.

`config_color_background=0,0,0,255`

### Text default color

The default color of the text in the application.

`color_text_default=10,40,40,255`

### Color error color

The color displayed by typo.

`color_text_error=30,250,30,255`

### Color typed text

The color of the typed text.

`color_text_typed=30,10,30,255`
