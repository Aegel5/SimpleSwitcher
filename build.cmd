download last release wxWidgets and unpack to folder wxWidgets
build release and debug with non shared runtime


$(SolutionDir)$(Configuration)\verinc.exe "$(ProjectDir)\ver.h"
signtool.exe sign /fd SHA256 /v /f "$(SolutionDir)\cert\SimpleSwitcher.pfx" /p m27SPnDeDZgz9Fvw483N /ph /t http://timestamp.comodoca.com/authenticode "$(SolutionDir)$(Configuration)\$(TargetName)$(TargetExt)" & $(SolutionDir)$(Configuration)\checksum.exe sha256 "$(SolutionDir)$(Configuration)\$(TargetName)$(TargetExt)" toFile