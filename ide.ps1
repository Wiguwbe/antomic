Get-Command code -ErrorAction SilentlyContinue -ErrorVariable NoCommandError
if ($NoCommandError) {
    Write-Host "Please install Visual Studio Code (https://code.visualstudio.com/Download)"
    exit 1
}
$OLD_PATH = [System.Environment]::GetEnvironmentVariable("PATH")
[System.Environment]::SetEnvironmentVariable("PATH", "C:\msys64\mingw64\bin;" + $OLD_PATH)
[System.Environment]::SetEnvironmentVariable("MINGW_CHOST", "x86_64-w64-mingw32")
[System.Environment]::SetEnvironmentVariable("MINGW_PACKAGE_PREFIX", "mingw-w64-x86_64")
[System.Environment]::SetEnvironmentVariable("MINGW_PREFIX", "/mingw64")
[System.Environment]::SetEnvironmentVariable("MSYSCON", "mintty.exe")
[System.Environment]::SetEnvironmentVariable("MSYSTEM", "MINGW64")
[System.Environment]::SetEnvironmentVariable("MSYSTEM_CARCH", "x86_64")
[System.Environment]::SetEnvironmentVariable("MSYSTEM_CHOST", "x86_64-w64-mingw32")
[System.Environment]::SetEnvironmentVariable("MSYSTEM_PREFIX", "/mingw64")    

code .