# FilediskProxy.Net
A very-very simple tutorial level and basic example in C++ and C# and .Net of a simple ordinary file backend based full fledged Virtual Disk Drive. A basic virtual disk kernel driver for learning. We have a user interface combined Server-Client Application written in C# .Net which binds with the driver to build a file backed Virtual Disk Drive. The C# .Net Application is the Core Proxy Server, and it uses the Driver to run a Virtual Disk Drive through it. All the code including the kernel driver is included in this repository.

CAUTION
please be warned that this entire software set is in testing alpha stage, so it's buggy imperfect stage. it is not a final version. so please do not use it for storing any valuable data. always create 2+ backups of your valuable data in other safe places before using it with this software. there is no guarantee or warranty of this software when it crashes with data. please use it at your own risk. it is advised to create multiple safe backups of data in multiple places, not just a single place.

Q. now what's the purpose?

A. the purpose is quite simple to understand, and people certainly want it the most on entire globe. so we have a kernel driver, which in itself is complete and clean, as we have no need to further implement anything in it. the native interop libraries written in C++ are also complete and clean, we have no need to further implement anything new in them. but right in the C# .Net proxy application which is the actual core of the driver and virtual disk drive which runs through it, we can easily implement anything, any further processing, any mechanism and function, for example an on the fly virtual disk and file encryption including the Aes-XTS mode encryption. I have tested Aes-XTS mode encryption with this application and it works perfect. That's the main thing we can implement in the C# .Net proxy application right in the core Server-Client Threads in the application. So there are limitless possibilities, we can implement anything whatever works with this software. So it is true that we can build an encrypted vault system with this software. I myself built it and using it. so we have a perfect and complete clean base ready in our hands which is the driver and the C# .Net proxy core app, and above this base we have limitless possibilities of implementations we can implement whatever correctly works with the base. the main purpose is on the fly virtual disk and file encryption and the best we can implement is Aes-XTS mode in it.

The meaning of "Proxy" is for example an Oven which can not even bake breads of different kinds, but also cook or heat other "custom" things. So the bread's dough or any other "custom" thing is put in the Oven by the user or the application, and the Oven bakes cooks or heats it. "Proxy" means a Server which performs input and output of data, but relies only on the "custom" data provided by a backend core application. The Proxy Server does not own any data or file, so it relies on the "custom" data and file transmitted by the other application, so the owner of this "custom" data and file is the application, not the Proxy Server. Meaning a core application provides it's own file and data to the Proxy Server instead of the Proxy Server to have and process it's own data and it's own file on it's own. Please note that the driver cannot run virtual disk without this C# .Net proxy core app, meaning C# .Net proxy core app is the core Server-Client combined which drives the driver and runs a virtual disk drive through it.

All the nessessary code and the features have been completed. the driver and the C# .Net core app is the base. we can implement whatever works above this base. There might be bugs, so you may inform them to me or fix them yourself. Please note I take no responsibility for anything, so use this source code at your own risk. This source code is completely unlicensed free and public, there's nothing hidden or having any price.

there are 4 modes available in both the driver and the proxy application server. both software make use of them to run file based virtual disk drive. this is not a professional source code, offcourse bugs should exist but fixable.

1. shared memory
2. pipe
3. winsock2
4. file

please open the "filedisk-proxy-test" folder where you can find the binary executable files and test and use them.

1. firstly, please install Visual C++ 2015-2022 redistributable dependency file "VC_redist.x64.exe". This dependency is required to run C++ Native and Managed Code, or the source code won't run.
2. secondly, please install .Net Framework 4.8 for .Net4.8 version, or .Net 6.0.1 or latest for .Net60 version of the source code. or it won't do.
3. turn on the windows test signed drivers mode. then reboot is required.
4. run the script install.cmd
5. run the script start.cmd
6. then FilediskProxyNet.exe.
7. if everything is working OK, driver is installing and has started, then FilediskProxyNet.exe can run virtual disk through the driver.
8. please note that you must run everything in Administrator account with Administrative previliges. or it won't work at all.

to compile the solution:
1. use visual studio 2019 with latest sdk and wdk.
2. set the configuration for all projects to "release".
3. set the platform for all projects to "x64".
4. then run rebuild all.
5. please note that all projects must output to a single path.
6. for your convenience, precompiled binary files are available in "filedisk-proxy-test" folder.

