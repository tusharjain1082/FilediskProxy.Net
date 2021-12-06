sc stop filedisk
sc delete filedisk
sc create filedisk binpath="%CD%\filedisk.sys" type=kernel
sc start filedisk
filedisk.exe /mount 0 "%CD%\disk.img" O:
pause
