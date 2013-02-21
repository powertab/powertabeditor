rem Replace with the path to your Wix installation directory.
set WIX_BIN="C:\Program Files (x86)\Wix Toolset v3.7\bin"

%WIX_BIN%"\candle.exe" pte_windows_build.wxs
%WIX_BIN%"\light.exe" -out powertabeditor.msi pte_windows_build.wixobj

pause
