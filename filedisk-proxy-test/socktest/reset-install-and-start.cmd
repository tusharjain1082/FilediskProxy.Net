sc stop socktest
sc delete socktest
sc create socktest binpath="%CD%\socktest.sys" type=kernel
sc start socktest
pause
