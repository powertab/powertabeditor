param(
    [string]
    $vcpkg_exe_path = "C:\Program Files\vcpkg\vcpkg.exe",

    [string]
    $python_exe_path = "$($HOME)\AppData\Local\Programs\Python\Python39\python.exe"
)

if (!(Test-Path $python_exe_path)) {
    throw "python exe path not found please update to your path to python";
}

if (!(Test-Path $vcpkg_exe_path)) {
    throw "vcpkg exe path not found please update to your path to vcpkg and/or install vcpkg first";
}

& $vcpkg_exe_path install --triplet x64-windows boost-algorithm boost-date-time boost-endian boost-functional boost-iostreams boost-rational boost-signals2 boost-stacktrace doctest minizip nlohmann-json pugixml;

& $python_exe_path -m pip install setuptools wheel py7zr==0.20.*;

& $python_exe_path -m pip install aqtinstall==3.1.*;

& $python_exe_path -m aqt install-qt windows desktop 5.15.2 win64_msvc2019_64 --outputdir "$($PSScriptRoot)\..\Qt";