mkdir package_build
cd package_build
cmake -G "Visual Studio 17 2022" -A Win32 -DCMAKE_BUILD_TYPE=Release .
cmake --build .

rem $(SolutionDir)$(Configuration)\verinc.exe "$(ProjectDir)\ver.h"
rem signtool.exe sign /fd SHA256 /v /f "$(SolutionDir)\cert\SimpleSwitcher.pfx" /p m27SPnDeDZgz9Fvw483N /ph /t http://timestamp.comodoca.com/authenticode "$(SolutionDir)$(Configuration)\$(TargetName)$(TargetExt)" & $(SolutionDir)$(Configuration)\checksum.exe sha256 "$(SolutionDir)$(Configuration)\$(TargetName)$(TargetExt)" toFile