set
set CONFIG=%1
echo %CD%
echo %~dp0
xcopy /s /y DLL\Externals x64\%CONFIG%
xcopy /s /y Assets x64\Assets\
xcopy /s /y Internal x64\Internal\