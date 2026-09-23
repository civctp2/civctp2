@echo off
setlocal

set "ROOT=%~dp0"
if "%ROOT:~-1%"=="\" set "ROOT=%ROOT:~0,-1%"

if not defined CDKDIR set "CDKDIR=%ROOT%\bin"
set "VSDEVCMD="
set "MSBUILD_EXE="
set "VCTARGETS_DIR="
set "NASMPATH=%ROOT%\NASM\nasm-2.16.03-win64\nasm-2.16.03\"

for %%I in (
  "C:\Program Files\Microsoft Visual Studio\18\Insiders\Common7\Tools\VsDevCmd.bat"
  "H:\BuildTools\Common7\Tools\VsDevCmd.bat"
) do (
  if not defined VSDEVCMD if exist %%~I set "VSDEVCMD=%%~I"
)

for %%I in (
  "H:\BuildTools\MSBuild\Current\Bin\MSBuild.exe"
  "C:\Program Files\Microsoft Visual Studio\18\Insiders\MSBuild\Current\Bin\MSBuild.exe"
) do (
  if not defined MSBUILD_EXE if exist %%~I set "MSBUILD_EXE=%%~I"
)

for %%I in (
  "H:\BuildTools\MSBuild\Microsoft\VC\v180"
  "C:\Program Files\Microsoft Visual Studio\18\Insiders\MSBuild\Microsoft\VC\v180"
) do (
  if not defined VCTARGETS_DIR if exist "%%~I\Microsoft.Cpp.Default.props" set "VCTARGETS_DIR=%%~I"
)

if not defined VSCMD_VER (
  if not exist "%VSDEVCMD%" (
    echo ERROR: Visual Studio developer shell not found: "%VSDEVCMD%"
    exit /b 1
  )
  call "%VSDEVCMD%" -arch=x86
  if errorlevel 1 exit /b %errorlevel%
)

if defined VCTARGETS_DIR set "VCTargetsPath=%VCTARGETS_DIR%\"

if not exist "%MSBUILD_EXE%" (
  echo ERROR: MSBuild not found: "%MSBUILD_EXE%"
  exit /b 1
)

if not exist "%NASMPATH%\nasm.exe" (
  echo ERROR: NASM not found: "%NASMPATH%\nasm.exe"
  exit /b 1
)

pushd "%ROOT%"

"%MSBUILD_EXE%" ^
  ctp2_code\ctp\civctp.sln ^
  /p:Configuration=Final-SDL ^
  /p:Platform=Win32 ^
  /p:PlatformToolset=v145 ^
  /p:WindowsTargetPlatformVersion=10.0.26100.0 ^
  /m /nologo %*

set "BUILD_RC=%ERRORLEVEL%"
popd
endlocal
exit /b %BUILD_RC%
