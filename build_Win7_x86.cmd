cmake -S . -B build_win7 -G "Visual Studio 18 2026" -A Win32 -T v143 -DWIN7_COMPAT=ON
cmake --build build_win7 --config Release