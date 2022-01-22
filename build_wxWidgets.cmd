cd wxWidgets
cmake -G "Visual Studio 17 2022" -A Win32 -DwxBUILD_SHARED=OFF -DwxBUILD_USE_STATIC_RUNTIME=ON .
rem cmake --build .