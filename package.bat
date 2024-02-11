mkdir release
xcopy "static" "release/static" /E /I /Y
copy ".env.exmpl" "release/.env"
copy "out\build\x64-Release\WebCMD.exe" "release/WebCMD.exe"
cd "release"
tar.exe -acvf WebCMD.zip  --exclude=*.zip *
cd ".."
move "release\WebCMD.zip" "./"
rmdir release /S /Q