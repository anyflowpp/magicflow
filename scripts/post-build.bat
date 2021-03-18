set f=%1%
set f=%f:/=\%

set t=%2%
set t=%t:/=\%

set orgdir=%cd%
pushd %~dp0
    copy %f% %t%
    set r=%errorlevel%
    if not %r% == 0 (
        cd %orgdir%
        exit %r%
    )
popd
