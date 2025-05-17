import polib
import sys
from pathlib import Path

#pip install polib

curpath = Path(__file__).parent.resolve()
name = Path(curpath / ".last_name").read_text()
path = curpath / name

po = polib.pofile(path)
po.encoding = "utf-8"

cont = Path(curpath / "flat.txt").read_text(encoding='utf-8')
cont = cont.replace("\r", "\n")
lines = cont.split("\n", )
lines = [x for x in lines if x]
cur_i = 0
# Translate each entry
for entry in po:
	if entry.msgstr == "":  # Only translate if there's no existing translation
		entry.msgstr = lines[cur_i]
		cur_i+=1

# Save the translated .po file
po.save(path)
print(f"applyed {cur_i} records")

