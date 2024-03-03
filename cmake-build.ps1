param(
    [string]
    $vcpkg_dir = "C:\Program Files\vcpkg\"
)

$qt5_widgets_cmake_path = "$($PSScriptRoot)/Qt/5.15.2/msvc2019_64/lib/cmake/Qt5Widgets/";
$qt5_network_cmake_path = "$($PSScriptRoot)/Qt/5.15.2/msvc2019_64/lib/cmake/Qt5Network/";
$qt5_printsupport_cmake_path = "$($PSScriptRoot)/Qt/5.15.2/msvc2019_64/lib/cmake/Qt5PrintSupport/";
$qt5_linguisttools_cmake_path = "$($PSScriptRoot)/Qt/5.15.2/msvc2019_64/lib/cmake/Qt5LinguistTools";

cmake -A x64 -B ./build -DPTE_ENABLE_PCH=1 -DCMAKE_TOOLCHAIN_FILE="$($vcpkg_dir)/scripts/buildsystems/vcpkg.cmake" -DCMAKE_PREFIX_PATH="$qt5_widgets_cmake_path;$qt5_network_cmake_path;$qt5_printsupport_cmake_path;$qt5_linguisttools_cmake_path"