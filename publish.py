import subprocess
import sys

# Получаем все аргументы, кроме имени скрипта
args = sys.argv[1:]
args.append('/publish')

# Запускаем второй скрипт с аргументами
subprocess.run(['python', 'build.py'] + args)