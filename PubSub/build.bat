IF NOT EXIST "%cd%\build\" (
    mkdir build
) 
set OUTPUT=%cd%\build\

cmake -DCMAKE_USER_MAKE_RULES_OVERRIDE=ClangOverrides.txt -H"%cd%" -B"%OUTPUT%"

cmake --build "%OUTPUT%"
pause