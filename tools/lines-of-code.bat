@echo off

Setlocal EnableDelayedExpansion

set /a lines = 0

Call :countLines "../legion/"
Call :countLines "../applications/"

echo total lines is %lines%

:End
pause
Endlocal
@Echo on
@exit /b

:countLines
echo counting lines in %1
for /r %1 %%a in (*.cpp, *.hpp, *.inl)  do (
  for /f "usebackq" %%b in (`type "%%a" ^| find "" /v /c`) do (
    set /a lines += %%b
    )
  )