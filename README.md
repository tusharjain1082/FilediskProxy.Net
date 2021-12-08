# FilediskProxy.Net
A very-very simple tutorial level and basic example in C++ and C# and .Net of a simple ordinary file backend based full fledged Virtual Disk Drive. A basic virtual disk kernel driver for learning. We have a user interface combined Server-Client Application written in C# .Net which binds with the driver to build a file backed Virtual Disk Drive. The C# .Net Application is the Core Proxy Server, and it uses the Driver to run a Virtual Disk Drive through it. Almost all necessary code is completed, so you can download and run it. All the code including the kernel driver is included in this repository.

Please note that the work is in progress. Nessessary code has been completed. There might be bugs, so you may inform them to me or fix them yourself.

there are 3 modes available in both the driver and the proxy application server. both software make use of them to run file based virtual disk drive. offcourse bugs may exist but fixable.

1. shared memory
2. pipe
3. winsock2

please open the "filedisk-proxy-test" folder where you can find the binary executable files and test and use them.

1. turn on the windows test signed drivers mode. then reboot is required.
2. run the script install.cmd
3. run the script start.cmd
4. then FilediskProxyNet.exe.
5. if everything is working OK, driver is installing and has started, then FilediskProxyNet.exe can run virtual disk through the driver.
6. please note that you must run everything in Administrator account with Administrative previliges. or it won't work at all.

to compile the solution:
1. use visual studio 2019 with latest sdk and wdk.
2. set the configuration for all projects to "release".
3. set the platform for all projects to "x64".
4. then run rebuild all.
5. please note that all projects must output to a single path.
6. for your convenience, precompiled binary files are available in "filedisk-proxy-test" folder.
