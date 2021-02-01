@echo off
pushd %~dp0
pushd build
cmake --build  .  --config Debug
set ret=%errorlevel%
if not "%ret%" == "0" (
    popd
    popd
    echo "cmake fail"
    exit 1
) else (
    rem "D:\megvii\AirPortProject\build\debug\AirPortTestTool.exe"
    popd
    popd
)
@echo on