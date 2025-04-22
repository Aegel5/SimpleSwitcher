import pathlib
from pathlib import Path
import re
import os
import shutil 
import sys

result = ''

for root, dirs, files in os.walk('./png16_resized'):
    for file in files:
        if file.endswith('.png'):    
            result += '{}16 RCDATA "flags/png16_resized/{}"\n'.format(
                Path(file).stem.upper(), os.path.basename(file)
            )
            
Path("./flag_res16.rc").write_text(result)
    