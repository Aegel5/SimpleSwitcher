from googletrans import Translator
from pathlib import Path
import polib
import asyncio
import sys

# pip install googletrans polib

translator = Translator()

async def translate(file):
    # Load the .po file
    po = polib.pofile(file)
    po.encoding = "utf-8"

    # Translate each entry
    for entry in po:
        if entry.msgstr == "":  # Only translate if there's no existing translation
            translated = await translator.translate(entry.msgid, dest=file.stem)  # Change 'fr' to your target language code
            print(f"translated {entry.msgid} -> {translated.text}")
            entry.msgstr = translated.text

    # Save the translated .po file
    po.save(file)

async def start():
    p = Path.cwd()
    for item in p.rglob('*'):  # Используем rglob для рекурсивного обхода
        if item.is_dir():
            print(f'Директория: {item}')
        elif item.is_file():
            if item.suffix == ".po":
                await translate(item)
                

                
asyncio.run(translate(Path.cwd() / f"{sys.argv[1]}.po"))

