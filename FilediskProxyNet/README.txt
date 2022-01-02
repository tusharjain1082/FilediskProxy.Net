FilediskProxy.Net
A very-very simple tutorial level and basic example in C++ and C# and .Net of a simple ordinary file backend based full fledged Virtual Disk Drive. A basic virtual disk kernel driver for learning. We have a user interface combined Server-Client Application written in C# .Net which binds with the driver to build a file backed Virtual Disk Drive. The C# .Net Application is the Core Proxy Server, and it uses the Driver to run a Virtual Disk Drive through it. All the code including the kernel driver is included in this repository.

The meaning of "Proxy" is for example an Oven which can not even bake breads of different kinds, but also cook or heat other "custom" things. So the bread's dough or any other "custom" thing is put in the Oven by the user or the application, and the Oven bakes cooks or heats it. "Proxy" means a Server which performs input and output of data, but relies only on the "custom" data provided by a backend core application. The Proxy Server does not own any data or file, so it relies on the "custom" data and file transmitted by the other application, so the owner of this "custom" data and file is the application, not the Proxy Server. Meaning a core application provides it's own file and data to the Proxy Server instead of the Proxy Server to have and process it's own data and it's own file on it's own.

All the nessessary code and the features have been completed. There may be bugs, so you inform them to me or fix them yourself.

there are 3 modes available in both the driver and the proxy application server. both software make use of them to run file based virtual disk drive. this is not a professional source code, offcourse bugs should exist but fixable.

shared memory
pipe
winsock2
please open the "filedisk-proxy-test" folder where you can find the binary executable files and test and use them.

firstly, please install Visual C++ 2015-2022 redistributable dependency file "VC_redist.x64.exe". This dependency is required to run C++ Native and Managed Code, or the source code won't run.
turn on the windows test signed drivers mode. then reboot is required.
run the script install.cmd
run the script start.cmd
then FilediskProxyNet.exe.
if everything is working OK, driver is installing and has started, then FilediskProxyNet.exe can run virtual disk through the driver.
please note that you must run everything in Administrator account with Administrative previliges. or it won't work at all.
to compile the solution:

use visual studio 2019 with latest sdk and wdk.
set the configuration for all projects to "release".
set the platform for all projects to "x64".
then run rebuild all.
please note that all projects must output to a single path.
for your convenience, precompiled binary files are available in "filedisk-proxy-test" folder.