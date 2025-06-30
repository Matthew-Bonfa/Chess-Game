@echo off
echo Cleaning build directory...

if exist build (
    rmdir /s /q build
    echo Build directory deleted.
) else (
    echo Build directory does not exist.
)

echo Done.
