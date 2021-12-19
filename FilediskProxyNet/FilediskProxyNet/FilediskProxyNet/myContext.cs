using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using FilediskProxyManaged;
using System.IO;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Security;
using System.Security.Cryptography;
using System.Threading;
using System.Collections;
using System.Runtime.InteropServices;
using System.IO.MemoryMappedFiles;
using System.Diagnostics;
using System.Runtime;
//using System.Memory;

namespace FilediskProxyNet
{
    public class myContext
    {
        // native constants
        public const UInt32 INFINITE = 0xFFFFFFFF;
        public const UInt32 WAIT_ABANDONED = 0x00000080;
        public const UInt32 WAIT_OBJECT_0 = 0x00000000;
        public const UInt32 WAIT_TIMEOUT = 0x00000102;

        //public const UInt32 INFINITE = 0xFFFFFFFF;

        // native io functions
        public const byte IRP_MJ_READ = 0x03;
        public const byte IRP_MJ_WRITE = 0x04;

        // native io buffers
        public IntPtr __buffer0;
        public IntPtr __buffer1;
        public const int ShmSize = 104857600; //52428800; // 50 mb

        // sockets
        public const uint DEFAULT_PORT = 27015;
        public String filename = "";
        public FileStream fs = null;
        public char[] driveletter = new char[4];
        public String drivePath = "";
        public String drivePathComplete = "";
        public byte[] header = null;
        public long virtual_image_size = 0;
        public long file_size_true = 0;
        public long offset = 0;
        public Deveel.Math.BigDecimal totalDataRead_BigDecimal = 0;
        public Deveel.Math.BigDecimal totalDataWrite_BigDecimal = 0;
        public long bandwidthRead = 0, bandwidthWrite = 0;
        public int readOnlyVHD = 0;
        public bool liveLockVHDWriteAccess = false;
        public bool newVHDFile = false;
        public FilediskProxyManaged.FilediskProxyManaged fdpObject;
        public Int64 ctx = 0; // session context including all session configuration and handles located in Native C++ level.
        public Thread myThread = null;
        public EventWaitHandle myThreadWaitHandle = new EventWaitHandle(false, EventResetMode.ManualReset);
        public int usePipe = 0; // when set, we use a pipe at both sides.
        public int useShm = 0; // when set, we use shared memory at both sides.
        public int useSocket = 0; // when set, we use sockets at both sides.
        public uint port = 0;

        public myContext()
        {
        }

        public void init()
        {
            // initialize native io buffers on the context initialisation
            __buffer0 = Marshal.AllocHGlobal(ShmSize);
            __buffer1 = Marshal.AllocHGlobal(ShmSize);
        }

        public void DisposeFinalizeAll()
        {
            if (__buffer0 != IntPtr.Zero)
                Marshal.FreeHGlobal(__buffer0);

            if (__buffer1 != IntPtr.Zero)
                Marshal.FreeHGlobal(__buffer1);

            __buffer0 = __buffer1 = IntPtr.Zero;
            GC.RemoveMemoryPressure(ShmSize + ShmSize);
            GC.Collect();

            this.fs.Flush();
            this.fs.Close();
            this.fs.Dispose();

        }
    }

}
