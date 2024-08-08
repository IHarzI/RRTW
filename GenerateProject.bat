call InternalVendor\premake\premake5.exe vs2022
if not exist "assets\" (
    echo Missing assets directory, creating one
    md "assets"
)
if not exist "assets\resourses" (
    echo Missing assets resourses directory, creating one
    md "assets\resourses"
)
if not exist "assets\resourses\images" (
    echo Missing assets images resourse directory, creating one
    md "assets\resourses\images"
)
pause