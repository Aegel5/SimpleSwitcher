import os
import shutil
import subprocess
import sys
from pathlib import Path

# === НАСТРОЙКА ДЛЯ PORTABLE VS CODE ===
# Укажите путь к вашей папке "data", которая лежит в корне Portable VS Code.
# Пример: r"D:\Programs\VSCode\data"
PORTABLE_DATA_DIR = r"D:/soft/vscode/data"

# Расширения файлов для форматирования
EXTENSIONS = {'.cpp', '.hpp', '.c', '.h'}


def find_clang_format():
    """Ищет clang-format в указанной папке data Portable-версии или в PATH."""
    # 1. Поиск внутри указанной Portable папки "data"
    if PORTABLE_DATA_DIR:
        data_path = Path(PORTABLE_DATA_DIR).resolve()
        if data_path.exists() and data_path.is_dir():
            # Задаем маску для рекурсивного поиска по всей папке data_path
            pattern = "**/clang-format.exe" if sys.platform == "win32" else "**/clang-format"
            
            # Находим все совпадения во всех подпапках
            found_paths = list(data_path.glob(pattern))
            if found_paths:
                # Возвращаем первый найденный исполняемый файл
                return str(found_paths[0])
        else:
            print(f"Предупреждение: Указанный путь PORTABLE_DATA_DIR '{PORTABLE_DATA_DIR}' не существует.", file=sys.stderr)


    # 2. Если в папке data не нашли (или она не указана), ищем в системном PATH
    system_path = shutil.which("clang-format")
    if system_path:
        return system_path

    return None


def format_directory(target_dir):
    """Находит все файлы с нужными расширениями за вычетом исключений и форматирует их."""
    clang_format_path = find_clang_format()

    if not clang_format_path:
        print("Ошибка: утилита clang-format не найдена.", file=sys.stderr)
        print("Пожалуйста, откройте скрипт и правильно укажите путь к папке 'data'", file=sys.stderr)
        print("в переменной `PORTABLE_DATA_DIR` в самом начале файла.", file=sys.stderr)
        sys.exit(1)

    print(f"Используется clang-format: {clang_format_path}")

    path_obj = Path(target_dir).resolve()
    if not path_obj.exists() or not path_obj.is_dir():
        print(f"Ошибка: Путь '{target_dir}' не существует.", file=sys.stderr)
        sys.exit(1)

    print(f"Сканирование директории: {path_obj}")

    files_to_format = []
    
    # Обходим дерево каталогов
    for root, dirs, files in os.walk(path_obj):
        # Исключаем ветки папок: скрытые (.*), extern* и build*
        dirs[:] = [
            d for d in dirs 
            if not d.startswith('.') 
            and 'extern' not in d.lower() 
            and 'build' not in d.lower()
        ]

        for file in files:
            file_path = Path(root) / file
            if file_path.suffix.lower() in EXTENSIONS:
                files_to_format.append(str(file_path))

    if not files_to_format:
        print("Подходящие файлы C++ не найдены (или все отфильтрованы).")
        return

    print(f"Найдено файлов для форматирования: {len(files_to_format)}")

    # Форматируем файлы пачками по 50 штук
    chunk_size = 2
    for i in range(0, len(files_to_format), chunk_size):
        chunk = files_to_format[i : i + chunk_size]
        # Извлекаем только имена файлов (с расширениями) из полных путей
        file_names = ", ".join(Path(f).name for f in chunk)        
        try:
            print(f"Запуск пачки: {file_names}")
            subprocess.run([clang_format_path, "-i"] + chunk, check=True)
        except subprocess.CalledProcessError as e:
            print(f"Ошибка при форматировании пачки файлов: {e}", file=sys.stderr)
            sys.exit(1)

    print("🎉 Форматирование успешно завершено!")


if __name__ == "__main__":
    target_directory = sys.argv[1] if len(sys.argv) > 1 else "."
    format_directory(target_directory)