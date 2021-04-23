set PATH=C:\MinGW\bin;%PATH%

if not exist "Data" mkdir "Data"
if not exist "Data/Archive" mkdir "Data/Archive"
if not exist "Data/Output" mkdir "Data/Output"
if not exist "Data/Region" mkdir "Data/Region"
if not exist "Data/Rejected" mkdir "Data/Rejected"
if not exist "Data/Sales" mkdir "Data/Sales"

if not exist "cpp_solution/bin" mkdir "cpp_solution/bin"

Rem Copy dll files from libs to bin Reference:http://www.askyb.com/cpp/c-postgresql-example/
copy .\cpp_solution\extern_libs\postgresql\lib\*.dll .\cpp_solution\bin\

g++.exe ^
-I"./cpp_solution/extern_libs/postgresql/include" ^
cpp_solution/src/simpleSalesprocessor.cpp ^
-static-libgcc ^
-static-libstdc++ ^
-L"./cpp_solution/extern_libs/postgresql/lib/" ^
-lpq ^
-o cpp_solution/bin/SimpleSalesProcessor.exe

pause