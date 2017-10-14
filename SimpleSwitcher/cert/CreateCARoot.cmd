makecert.exe ^
-n "CN=SimpleSwitcher" ^
-r ^
-pe ^
-a sha512 ^
-len 4096 ^
-cy authority ^
-sv SimpleSwitcher.pvk ^
SimpleSwitcher.cer

pvk2pfx.exe ^
-pvk SimpleSwitcher.pvk ^
-spc SimpleSwitcher.cer ^
-pfx SimpleSwitcher.pfx ^
-po m27SPnDeDZgz9Fvw483N