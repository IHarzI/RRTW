call InternalVendor\premake\premake5.exe vs2022
if not exist "assets\" (
    echo Missing shaders directory, creating one
    md "assets"
) else (
    echo Shaders directory already exists, copying shaders
)
pause