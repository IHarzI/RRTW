call InternalVendor\premake\premake5.exe vs2022
if not exist "assets\" (
    echo Missing assets directory, creating one
    md "assets"
)

pause