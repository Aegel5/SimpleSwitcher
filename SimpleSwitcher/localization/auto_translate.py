from deep_translator import (GoogleTranslator)
from pathlib import Path
import polib
import asyncio
import sys

# pip install deep-translator polib

def translate(file):
    # Load the .po file
    po = polib.pofile(file)
    po.encoding = "utf-8"

    # Translate each entry
    for entry in po:
        if entry.msgstr == "":  # Only translate if there's no existing translation
            translated = GoogleTranslator(source="en", target=file.stem).translate(entry.msgid)
            print(f'{entry.msgid} -> {translated}')
            entry.msgstr = translated

    # Save the translated .po file
    po.save(file)

               
translate(Path.cwd() / f"{sys.argv[1]}.po")

