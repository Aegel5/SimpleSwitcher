import sys
import os
import re
import json
import time
import subprocess
import hashlib
import zipfile
from datetime import datetime
from pathlib import Path

def message(text):
    """Выводит сообщение в консоль и мгновенно сбрасывает буфер (flush)."""
    print(text, flush=True)

def fail(message):
    print(f"\n::error::{message}\n", flush=True)
    raise RuntimeError(message)

def run_cmd(cmd, check=True, text=True, capture_output=True, env=None):
    """Хелпер для запуска консольных команд."""
    current_env = os.environ.copy()
    if env:
        current_env.update(env)
    try:
        res = subprocess.run(cmd, shell=True, check=check, text=text, capture_output=capture_output, env=current_env)
        # Если вывод не перехватывали (capture_output=False), возвращаем пустую строку
        return res.stdout.strip() if res.stdout else ""
    except subprocess.CalledProcessError as e:
        fail(f"Команда завершилась с ошибкой: {cmd}\nВывод: {e.output or e.stderr}")


# def check_stale_activity():
#     """Проверка свежести коммитов (за последние 25 часов)."""
        
#     last_commit_time = int(run_cmd("git log -1 --format=%ct"))
#     limit_time = int(time.time()) - (25 * 3600)
    
#     if last_commit_time < limit_time:
#         fail("::error::Нет свежих коммитов за 25 часов. Прерываю работу.")

# def cleanup_old_prereleases():
#     message("Удаление старых пре-релизов, оставляя 5 самых новых")
#     try:
#         raw_list = run_cmd("gh release list --limit 100 --json tagName,isPrerelease")
#         releases = json.loads(raw_list)
#         pre_releases = [r for r in releases if r.get('isPrerelease')]
        
#         # Пропускаем первые 5 (самые новые), остальные удаляем
#         for r in pre_releases[5:]:
#             tag = r['tagName']
#             message(f"Удаление релиза и тега: {tag}")
#             run_cmd(f'gh release delete "{tag}" --yes --cleanup-tag')
#     except Exception as e:
#         message(f"::warning::Ошибка при очистке релизов: {e}")

def get_version():
    """Парсинг версии из src/ver.h."""
    ver_file = Path("src/ver.h")
    if not ver_file.exists():
        fail("::error::Файл src/ver.h не найден")
        
    content = ver_file.read_text(encoding='utf-8')
    match = re.search(r'SW_VERSION\s*=\s*"([^"]+)"', content)
    if not match:
        fail("::error::Не удалось найти версию в файле ver.h")
        
    version = match.group(1)
    message(f"Найдена версия: {version}")
    return version

def calculate_sha256(file_path):
    """Подсчет SHA256 хеша для файла."""
    sha256_hash = hashlib.sha256()
    with open(file_path, "rb") as f:
        for byte_block in iter(lambda: f.read(4096), b""):
            sha256_hash.update(byte_block)
    return sha256_hash.hexdigest()

def prepare_artifacts_and_zip(build_dir_str, zip_name):
    """Хеширование EXE файлов и создание ZIP архива."""
    build_dir = Path(build_dir_str)
    if not build_dir.exists():
        fail(f"::error::Папка сборки {build_dir} не найдена")
        
    # Считаем SHA256 для всех .exe
    for exe_file in build_dir.glob("*.exe"):
        file_hash = calculate_sha256(exe_file)
        sha_file = exe_file.with_suffix(".exe.sha256")
        with open(sha_file, "w", encoding="ascii") as f:
            f.write(f"{file_hash} *{exe_file.name}\n")
                   
    # Создаем ZIP
    message(f"Создаем архив {zip_name}...")
    with zipfile.ZipFile(zip_name, 'w', zipfile.ZIP_DEFLATED) as zipf:
        
        for ext in ("*.exe", "*.sha256"):
            for file in build_dir.glob(ext):
                zipf.write(file, file.name)   
                
        bin_dir = Path("bin_files")
        for file in bin_dir.rglob("*"):
            if file.is_file():
                zipf.write(file, file.relative_to(bin_dir))
                
    return zip_name;

def install (path):
    cmd = [
        r"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vs_installer.exe",
        "modify",
        "--installPath", path,
        "--add", "Microsoft.VisualStudio.Component.VC.143.Toolset",
        "--quiet",
        "--norestart"
    ]

    try:
        # Метод subprocess.run по умолчанию блокирует поток и ждет завершения процесса
        process = subprocess.run(cmd, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    except subprocess.CalledProcessError as e:
        print(f"Error output: {e.stderr.decode('utf-8', errors='ignore')}", file=sys.stderr)
        sys.exit(e.returncode)    


def main():

    install(r'C:\Program Files\Microsoft Visual Studio\2022\Enterprise')

    # Получаем тип билда из аргументов командной строки
    build_type = sys.argv[1] if len(sys.argv) > 1 else ''
    
    # 1. Проверка активности
    #if build_type == 'preview': check_stale_activity()
    
    # 2. Очистка старых релизов
    # cleanup_old_prereleases()
    
    # 3. Получение SHA и Версии
    github_sha = os.getenv("GITHUB_SHA", "00000000")
    sha_short = github_sha[:3]
    version = get_version()
       
    # 4. Сборка (CMake)
    message("Конфигурация и сборка CMake...")
    cxx_flags = '-D PUBLIC_RELEASE' if build_type == 'publish' else ''
    env_vars = {"CXXFLAGS": cxx_flags} if cxx_flags else None

    run_cmd('cmake -S . -B build_win7 -A Win32 -T v143 -DWIN7_COMPAT=ON', env=env_vars, capture_output=False)
    run_cmd('cmake --build build_win7 --config Release', capture_output=False)   
    zip_name2 = prepare_artifacts_and_zip("build_win7/Release", f"SimpleSwitcher_v{version}_x86_Win7.zip")    
    
    run_cmd('cmake -S . -B build -DCMAKE_BUILD_TYPE=Release', env=env_vars, capture_output=False)
    run_cmd('cmake --build build --config Release', capture_output=False)
    zip_name = prepare_artifacts_and_zip("build/Release", f"SimpleSwitcher_v{version}.zip")
       
    # 6. Публикация релиза
    if build_type == 'publish':
        """Создание пре-релиза в GitHub."""
        tag_name = f"v{version}-{sha_short}"
        title = f"SimpleSwitcher {version}-{sha_short} PREVIEW"
        date_str = datetime.now().strftime('%Y-%m-%d %H:%M')
        notes = f"Автоматический билд от {date_str}"
        
        cmd = f'gh release create "{tag_name}" "./{zip_name}" --title "{title}" --notes "{notes}" --prerelease'
        message(f"Создаем релиз {tag_name}...")
        run_cmd(cmd)
    # else:
    #     current_date = datetime.now().strftime("%Y-%m-%d")
    #     artifact_zip_name = f"{zip_name.rsplit('.', 1)[0]}_{current_date}.zip"
             

        
    message("Скрипт успешно завершил работу!")

if __name__ == "__main__":
    main()

