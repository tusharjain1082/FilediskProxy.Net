sc stop filedisk-proxy
sc delete filedisk-proxy
sc create filedisk-proxy binpath="%CD%\filedisk-proxy.sys" type=kernel
sc start filedisk-proxy
"%CD%\FilediskProxyNet.exe"
