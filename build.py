import pathlib
from pathlib import Path
import re
import os
import shutil 

curpath = pathlib.Path(__file__).parent.resolve()
print(f"curpath is {curpath}");

os.chdir(curpath)

# пропатчим версию
ver_path = curpath / "SimpleSwitcher/src/ver.h"

contents = Path(ver_path).read_text()

curv = re.search("\"(.*)\"", contents).group(1)
a = curv.replace(".","")
aa = int(a)
aa+=1
aaa = str(aa)

curv2 = ""
for b in aaa:
    curv2 += b
    curv2 += '.'

curv2 = curv2[:-1]
contents = contents.replace(curv,curv2)
print(curv2)
Path(ver_path).write_text(contents)

def build(subfold, is64):

    print(f'*** BUILD {subfold} {("64" if is64 else "32")} ***');
    
    package_build_folder = pathlib.Path("package_build")
    build_folder = subfold
    if is64:
        build_folder += "_64"
    path = package_build_folder / build_folder

    pathlib.Path(path).mkdir(parents=True, exist_ok=True)
    
    os.system(f'cmake -G "Visual Studio 17 2022"  {" " if is64 else "-A Win32"} -DCMAKE_BUILD_TYPE=Release ./{subfold} -B {path}')
    os.system(f'cmake --build {path} --parallel --config Release')
    
    result_dir = package_build_folder / "OUT"
    result_dir.mkdir(parents=True, exist_ok=True)
    
    release_folder = path / "Release"
    tocopy = ['.exe', '.dll']
    for root, dirs, files in os.walk(release_folder):
        for file in files:
            for ext in tocopy:
                if file.endswith(ext):    
                    file2 = file
                    if is64:
                        file2 = file.replace(ext, "64"+ext)
                    print("copy ", file2)
                    shutil.copy(os.path.join(root, file), os.path.join(result_dir, file2))
            

build("loader_dll", is64=False)
build("loader_dll", is64=True)    
build("SimpleSwitcher", is64=False)

#rem $(SolutionDir)$(Configuration)\verinc.exe "$(ProjectDir)\ver.h"
#rem signtool.exe sign /fd SHA256 /v /f "$(SolutionDir)\cert\SimpleSwitcher.pfx" /p m27SPnDeDZgz9Fvw483N /ph /t http://timestamp.comodoca.com/authenticode "$(SolutionDir)$(Configuration)\$(TargetName)$(TargetExt)" & $(SolutionDir)$(Configuration)\checksum.exe sha256 "$(SolutionDir)$(Configuration)\$(TargetName)$(TargetExt)" toFile

print("\n***DONE***")
