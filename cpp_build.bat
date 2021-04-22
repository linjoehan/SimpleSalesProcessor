if not exist "Data" mkdir "Data"
if not exist "Data/Archive" mkdir "Data/Archive"
if not exist "Data/Output" mkdir "Data/Output"
if not exist "Data/Region" mkdir "Data/Region"
if not exist "Data/Rejected" mkdir "Data/Rejected"
if not exist "Data/Sales" mkdir "Data/Sales"

if not exist "cpp_solution/bin" mkdir "cpp_solution/bin"

set PATH=C:\MinGW\bin;%PATH%

g++.exe ^
cpp_solution/src/main.cpp ^
-static-libgcc ^
-static-libstdc++ ^
-o cpp_solution/bin/SimpleSalesProcessor.exe

pause