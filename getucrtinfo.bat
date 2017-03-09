@echo off
call "%VS140COMNTOOLS%vcvarsqueryregistry.bat"
echo %UniversalCRTSdkDir%
echo %UCRTVersion%
