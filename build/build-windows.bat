REM Assumes that QTDIR\bin and mingw\bin are in the PATH

md build-windows
cd build-windows

echo "Running qmake ..."
qmake ..\..\source\source.pro

echo "Building ..."
mingw32-make.exe -j2 release

echo "Packaging ..."
cd build

md pte
xcopy /E /Y /I skins pte\skins
xcopy /Y powertabeditor.exe pte\
rem Adjust locations to Qt/Mingw dlls if necessary
xcopy /Y C:\Qt\2010.05\qt\bin\QtGui4.dll pte\
xcopy /Y C:\Qt\2010.05\qt\bin\QtCore4.dll pte\
xcopy /Y C:\Qt\2010.05\mingw\bin\mingwm10.dll pte\
xcopy /Y C:\Qt\2010.05\mingw\bin\libgcc_s_dw2-1.dll pte\

REM Change the location of 7-zip if necessary
"C:\Program Files\7-Zip\7z.exe" a -tzip -mx=9 powertabeditor-windows.zip pte

move /y powertabeditor-windows.zip ..\..\
rmdir /S /Q pte
cd ..\..\

pause

