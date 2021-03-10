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
    popd
    popd
)
@echo on