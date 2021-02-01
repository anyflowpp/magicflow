pushd %~dp0
call "D:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"
devenv build\threadflow.sln /clean
popd