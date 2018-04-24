@echo off

conan export-pkg . mumpscpp/1.0.0@tuncb/pangea  --force || goto :error
conan test ./test_package mumpscpp/1.0.0@tuncb/pangea || goto :error
conan test ./test_package mumpscpp/1.0.0@tuncb/pangea || goto :error

goto :success

:error
echo Failed with error #%errorlevel%.
exit /b %errorlevel%

:success
echo Success!
