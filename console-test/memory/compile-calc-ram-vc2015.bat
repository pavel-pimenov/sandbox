del *.exe
del *.obj

call "%VS140COMNTOOLS%\..\..\VC\bin\vcvars32.bat"

cl /D _CONSOLE /W4 /O2 calc-ram.cpp

