using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Security;
using System.Security.Cryptography;
using System.Threading;
using System.Windows.Forms.VisualStyles;
using System.Runtime.InteropServices;
using System.Numerics;
using System.IO.MemoryMappedFiles;
using System.Diagnostics;

namespace FilediskProxyNet
{
    public partial class FormRoot : Form
    {
        myContext filehandle = null;
        bool terminateProxy = false;
        Stopwatch myStopWatchRunningTime = new Stopwatch();
        bool properExit = false;

        public FormRoot()
        {
            InitializeComponent();
        }

        private void FrmRoot_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (!properExit)
            {
                var res = MessageBox.Show(this, "you cannot directly close this form. please click on exit button to close the form.", "error",
                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                e.Cancel = true;
            }
            else
            {
                e.Cancel = false;
            }
            base.OnClosing(e);

            //            if (res != DialogResult.Yes)
            //          {
            //            e.Cancel = true;
            //          return;
            //    }
        }


        private void FormRoot_Load(object sender, EventArgs e)
        {
            DateTime outputDateTime = new DateTime();
            String strBuildDateTime = commonMethods1.GenerateTimeDateFromString(FilediskProxyNet.Properties.Resources.BuildDate, FilediskProxyNet.Properties.Resources.BuildTime, out outputDateTime);
            this.Text = this.Text + " Version " + Application.ProductVersion + ", Compiled/Built on: " + strBuildDateTime;
            //            String[] VersionInfo = Application.ProductVersion.Split(new char[] { '.' });
            //          lblVersion.Text = String.Format(lblVersion.Text, Application.ProductVersion, VersionInfo[3], ((VersionInfo[3] == "1") ? " (first) " : " "));

            cmbDriveLetter.SelectedIndex = cmbDriveLetter.FindString(String.Format("{0}", "Z"));

            txtPort.Text = myContext.DEFAULT_PORT.ToString();
        }


        public bool initializeFileSchedule(bool newVHDFile, myContext filehandle, bool loadVirtualDrive)
        {
            if (txtContainerFile.Text.Length > 0)
                return false;

            long newFileSize = -1;
            long newFileSizeMB = -1;
            long newFileSizeGB = -1;
            if (radioUseSizeMB.Checked)
            {
                if (!long.TryParse(txtConfigureVaultSizeMB.Text, out newFileSizeMB))
                {
                    MessageBox.Show("Error: Please provide a valid Vault file size in the Configuration page. Vault file size not provided, or is invalid.", "Vault file size not provided or invalid.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return false;
                }

                if (newFileSizeMB < 32)
                {
                    MessageBox.Show("Error: Vault size in megabytes, cannot be less than 32 megabytes. Please retry with number higher than 32 megabytes. ", "Vault file size invalid.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return false;
                }
                newFileSize = ((newFileSizeMB * 1024) * 1024);
            }
            else if (radioUseSizeGB.Checked)
            {
                if (!long.TryParse(txtConfigureVaultSizeGB.Text, out newFileSizeGB))
                {
                    MessageBox.Show("Error: Please provide a valid Vault file size in the Configuration page. Vault file size not provided, or is invalid.", "Vault file size not provided or invalid.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return false;
                }

                if (newFileSizeGB <= 0)
                {
                    MessageBox.Show("Error: Vault size in gigabytes invalid. Please retry with number higher than 0 gigabytes. ", "Vault file size invalid.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return false;
                }

                //ByteSize bsize = ByteSize.FromMegaBytes(newFileSizeMB);
                newFileSize = (((newFileSizeGB * 1024) * 1024) * 1024);
                //newFileSize = long.Parse(//bsize.Bytes.ToString());

            }

            String driveLetter = (String)cmbDriveLetter.SelectedItem;
            if (Directory.Exists(String.Format(@"{0}:\", driveLetter)))
            {
                // this drive letter the user has selected, is already in use, so abort with error.
                MessageBox.Show("Error: The drive letter you selected, is already in use. Please retry with another drive letter.", "drive letter in use.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }

            String filename = "";

            if (sfdVaultFile.FileName.Length <= 0)
                sfdVaultFile.InitialDirectory = commonMethods1.GetDrivePathByFolderPath(commonMethods1.GetFullAppStartupPath());// Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments); // Environment.GetFolderPath(Environment.SpecialFolder.MyComputer); //primaryMethodsCS.cCommonFunctions.GetMyComputerFolder();
            else
                sfdVaultFile.InitialDirectory = commonMethods1.getFullPathFromPathFile(sfdVaultFile.FileName);

            if (ofdVaultFile.FileName.Length <= 0)
                ofdVaultFile.InitialDirectory = commonMethods1.GetDrivePathByFolderPath(commonMethods1.GetFullAppStartupPath());// Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments); // Environment.GetFolderPath(Environment.SpecialFolder.MyComputer); //primaryMethodsCS.cCommonFunctions.GetMyComputerFolder();
            else
                ofdVaultFile.InitialDirectory = commonMethods1.getFullPathFromPathFile(ofdVaultFile.FileName);

            if (newVHDFile)
            {
                if (sfdVaultFile.ShowDialog(this) != DialogResult.OK) { return false; }
                filename = sfdVaultFile.FileName;
            }
            else
            {
                if (ofdVaultFile.ShowDialog(this) != DialogResult.OK) { return false; }
                filename = ofdVaultFile.FileName;
            }

            filehandle.filename = filename;
            filehandle.driveletter[0] = driveLetter[0];
            filehandle.file_size = ((newVHDFile) ? newFileSize : new FileInfo(filename).Length);
            filehandle.newVHDFile = newVHDFile;
            filehandle.readOnlyVHD = chkReadOnlyVHD.Checked;
            filehandle.drivePath = filehandle.driveletter[0] + @":";
            filehandle.drivePathComplete = String.Format(@"{0}:\", driveLetter);

            String destFile = filehandle.filename;
            if (newVHDFile)
            {
                // delete old file and create new file always

                if (File.Exists(destFile))
                    commonMethods1.DeleteFile(destFile);

                filehandle.fs = commonMethods1.CreateInitializeFile(destFile, filehandle.file_size, true);
            }
            else
            {
                // open existing file
                filehandle.fs = commonMethods1.CreateInitializeFile(destFile, filehandle.file_size, false);
            }

            // now initialize native i/o buffers
            filehandle.init();

            return true;

        }

        private void cmdCreateNewFile_Click(object sender, EventArgs e)
        {
            CreateOrOpenFile();
        }

        public void CreateOrOpenFile(bool newFile = true)
        {

            Int64 ctxref = 0;
            int usePipe = 0;
            int useShm = 0;
            int useSocket = 0;

            if (radioUsePipe.Checked)
                usePipe = 1;

            if (radioUseSHM.Checked)
                useShm = 1;

            if (radioUseSocket.Checked)
                useSocket = 1;

            uint port = 0;

            if (!uint.TryParse(txtPort.Text, out port))
            {
                MessageBox.Show("Error: Please provide a valid port from 0 to 65535.", "error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            myContext filehandle = new myContext();
            if (!initializeFileSchedule(newFile, filehandle, true))
            {
                MessageBox.Show("critical error creating/loading file. aborted", "critical error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            this.filehandle = filehandle;

            // finally set the loaded vault file name
            txtContainerFile.Text = ofdVaultFile.FileName = filehandle.filename;

            filehandle.fdpObject = new FilediskProxyManaged.FilediskProxyManaged();

            int result = filehandle.fdpObject.init_ctx((byte)filehandle.driveletter[0], (ulong)filehandle.file_size, usePipe, useShm, useSocket, port, ref ctxref);
            if (result != 0)
            {
                filehandle.usePipe = usePipe;
                filehandle.useShm = useShm;
                filehandle.useSocket = useSocket;
                filehandle.port = port;
                filehandle.ctx = ctxref;
                myStopWatchRunningTime.Restart();
                timerUpdateStatus.Enabled = false;
                timerUpdateStatus.Start();
                tssLabelVaultFile1.Text = "file loaded. drive path: " + filehandle.drivePath;
                txtDrivePath.Text = filehandle.drivePathComplete;
                terminateProxy = false;
                initThread(filehandle);
                //MessageBox.Show("success");
            }
            else
            {
                MessageBox.Show("failure");
            }

        }

        public void initThread(myContext obj)
        {
            obj.myThreadWaitHandle.Reset();
            if (obj.usePipe == 1)
            {
                obj.myThread = new Thread(new ParameterizedThreadStart(ProxyPipeServerThreadFunction));
            }
            else if (obj.useShm == 1)
            {
                obj.myThread = new Thread(new ParameterizedThreadStart(ProxySHMServerThreadFunction));
            }
            else if (obj.useSocket == 1)
            {
                obj.myThread = new Thread(new ParameterizedThreadStart(ProxySocketServerThreadFunction));
            }
            obj.myThread.Start(obj);

            // open the folder
            Thread.Sleep(2000);
            commonMethods1.openFolder(obj.drivePathComplete);

        }

        private void ProxySocketServerThreadFunction(Object obj)
        {
            myContext filehandle = (myContext)obj;
            UInt32 WaitStatus;

            // initialize a buffer
            byte[] buffer = new byte[myContext.ShmSize];

            // notify windows of drive add
            filehandle.fdpObject.NotifyWindows(filehandle.ctx, 1);

            // phase 1 = infinite loop server and client
            while (true)
            {
                // terminate if user set the termination flag
                if (terminateProxy)
                {
                    filehandle.myThreadWaitHandle.Set();
                    return;
                }

                // wait for driver event to occur
                WaitStatus = filehandle.fdpObject.WaitEventDriverRequestDataSet(filehandle.ctx, 1000);
                if (WaitStatus == myContext.WAIT_OBJECT_0)
                {
                    // success, the driver set the event flag
                    // reset the same event and proces the request
                    filehandle.fdpObject.SetEventDriverRequestDataSet(filehandle.ctx, 0);                    
                }
                else if (WaitStatus == myContext.WAIT_TIMEOUT)
                {
                    // the driver did not set the event flag, so continue
                    continue;
                }
                else
                {
                    // error or exception, continue
                    continue;
                }

                
                // step 1: receive request

                // client connected at this point
                // we directly connect to the socket and receive request upon connection.
                // accept the incoming client connection
                filehandle.fdpObject.SocketAcceptClient(filehandle.ctx);

                UInt64 offset = 0;
                UInt32 length = 0;
                byte function = 0;
                UInt32 totalBytesReadWrite = 0;
                // Step 1: receive request from the client
                filehandle.fdpObject.Step1SocketGetRequest(filehandle.ctx, ref offset, ref length, ref function, ref totalBytesReadWrite);



                // step 2: io



                // set position in backend disk file
                filehandle.fs.Seek((long)offset, SeekOrigin.Begin);

                // set the proxy idle event for the client to unblock because we have processed the request.
                // then we must wait for the client to process and set the flag for us to unblock and only then proceed.
                filehandle.fdpObject.SetEventProxyIdle(filehandle.ctx, 1);
                filehandle.fdpObject.WaitEventRequestComplete(filehandle.ctx, myContext.INFINITE);
                filehandle.fdpObject.SetEventRequestComplete(filehandle.ctx, 0);

                // now process the i/o
                if (function == myContext.IRP_MJ_READ)
                {
                    // driver request read into buffer
                    filehandle.fs.Read(buffer, 0, (int)length);
                    CopyToGlobalBuffer(buffer, 0, filehandle.__buffer0, 0, (int)length);
                    filehandle.fdpObject.WriteSocket(filehandle.ctx, (long)filehandle.__buffer0, length);
                    filehandle.totalDataRead_BigDecimal += length;
                    filehandle.bandwidthRead += length;
                }
                else
                {
                    // driver request write through buffer into virtual disk file
                    filehandle.fdpObject.ReadSocket(filehandle.ctx, (long)filehandle.__buffer0, length);
                    CopyFromGlobalBuffer(filehandle.__buffer0, 0, buffer, 0, (int)length);
                    filehandle.fs.Write(buffer, 0, (int)length);
                    filehandle.fs.Flush();
                    filehandle.totalDataWrite_BigDecimal += length;
                    filehandle.bandwidthWrite += length;
                }

                // set the proxy idle event for the client to unblock because we have processed the request.
                // then we must wait for the client to process and set the flag for us to unblock and only then proceed.
                filehandle.fdpObject.SetEventProxyIdle(filehandle.ctx, 1);
                filehandle.fdpObject.WaitEventRequestComplete(filehandle.ctx, myContext.INFINITE);
                filehandle.fdpObject.SetEventRequestComplete(filehandle.ctx, 0);



                // Step 3: set/reset the events and reset and reloop as the request has been processed at this point.



                // finally set the proxy idle event this final time, for the driver to unblock and reset and reloop or complete the session context.
                filehandle.fdpObject.SetEventProxyIdle(filehandle.ctx, 1);
                filehandle.fdpObject.WaitEventRequestComplete(filehandle.ctx, myContext.INFINITE);
                filehandle.fdpObject.SetEventRequestComplete(filehandle.ctx, 0);

                

                // Step 4: close the client socket
                filehandle.fdpObject.CloseClientSocket(filehandle.ctx);



                // Step 5: completed. reset reloop

            }
        }

        private void ProxyPipeServerThreadFunction(Object obj)
        {
            myContext filehandle = (myContext)obj;
            UInt32 WaitStatus;
            int result0 = 0;

            // initialize a buffer
            byte[] buffer = new byte[myContext.ShmSize];

            // notify windows of drive add
            filehandle.fdpObject.NotifyWindows(filehandle.ctx, 1);

            // phase 1 = infinite loop server and client
            while (true)
            {
                // terminate if user set the termination flag
                if (terminateProxy)
                {
                    filehandle.myThreadWaitHandle.Set();
                    return;
                }

                // wait for driver event to occur
                WaitStatus = filehandle.fdpObject.WaitEventDriverRequestDataSet(filehandle.ctx, 1000);
                if (WaitStatus == myContext.WAIT_OBJECT_0)
                {
                    // success, the driver set the event flag
                }
                else if (WaitStatus == myContext.WAIT_TIMEOUT)
                {
                    // the driver did not set the event flag, so continue
                    continue;
                }
                else
                {
                    // error or exception, continue
                    continue;
                }

                // at this point, the driver has requested data i/o, so connect to it through the pipe and process the request.

                // Step 1: wait and connect to a client at pipe.
                result0 = filehandle.fdpObject.ConnectPipe(filehandle.ctx);

                // client connected at this point
                // there is no wait here, we only use connect pipe and the function waits.
                // we directly connect to the pipe and receive request upon connection.
                UInt64 offset = 0;
                UInt32 length = 0;
                byte function = 0;
                UInt32 totalBytesReadWrite = 0;
                // Step 1: receive request from the client
                filehandle.fdpObject.Step1PipeGetRequest(filehandle.ctx, ref offset, ref length, ref function, ref totalBytesReadWrite);




                // Step 2: i/o



                // set position in backend disk file
                filehandle.fs.Seek((long)offset, SeekOrigin.Begin);

                // now process the i/o
                if (function == myContext.IRP_MJ_READ)
                {
                    // driver request read into buffer
                    filehandle.fs.Read(buffer, 0, (int)length);
                    CopyToGlobalBuffer(buffer, 0, filehandle.__buffer0, 0, (int)length);
                    filehandle.fdpObject.WritePipe(filehandle.ctx, (long)filehandle.__buffer0, length);
                    filehandle.totalDataRead_BigDecimal += length;
                    filehandle.bandwidthRead += length;
                }
                else 
                {
                    // driver request write through buffer into virtual disk file
                    filehandle.fdpObject.ReadPipe(filehandle.ctx, (long)filehandle.__buffer0, length);
                    CopyFromGlobalBuffer(filehandle.__buffer0, 0, buffer, 0, (int)length);
                    filehandle.fs.Write(buffer, 0, (int)length);
                    filehandle.fs.Flush();
                    filehandle.totalDataWrite_BigDecimal += length;
                    filehandle.bandwidthWrite += length;
                }

                // Step 2: close the connection to client.
                filehandle.fdpObject.DisconnectPipe(filehandle.ctx);



                // Step 3: set/reset the events and reset and reloop as the request has been processed at this point.

                // finally set the proxy idle event for the driver to unblock and process the reply and unset the driver data set event
                filehandle.fdpObject.SetEventDriverRequestDataSet(filehandle.ctx, 0);
                filehandle.fdpObject.SetEventProxyIdle(filehandle.ctx, 1);


                // Step 4: completed.
            }
        }

        private void ProxySHMServerThreadFunction(Object obj)
        {
            myContext filehandle = (myContext)obj;

            UInt32 WaitStatus;

            // initialize a buffer
            byte[] buffer = new byte[myContext.ShmSize];

            // notify windows of drive add
            filehandle.fdpObject.NotifyWindows(filehandle.ctx, 1);

            // phase 1 = infinite loop server and client
            while (true)
            {
                // terminate if user set the termination flag
                if (terminateProxy)
                {
                    filehandle.myThreadWaitHandle.Set();
                    return;
                }

                // wait for driver event to occur
                WaitStatus = filehandle.fdpObject.WaitEventDriverRequestDataSet(filehandle.ctx, 1000);
                if (WaitStatus == myContext.WAIT_OBJECT_0)
                {
                    // success, the driver set the event flag
                }
                else if (WaitStatus == myContext.WAIT_TIMEOUT)
                {
                    // the driver did not set the event flag, so continue
                    continue;
                }
                else
                {
                    // error or exception, continue
                    continue;
                }

                // at this point, the driver has requested data i/o, so connect to it and process the request.

                // unblock when the driver sets the event
                // success, the driver set the event flag
                // then process the driver's request here

                // open and read shared memory
                Int64 offset = 0;
                UInt32 length = 0;
                byte function = 0;
                UInt32 totalBytesReadWrite = 0;
                filehandle.fdpObject.GetSHMHeader(filehandle.ctx, ref offset, ref length, ref function, ref totalBytesReadWrite);

                // set position in backend disk file
                filehandle.fs.Seek(offset, SeekOrigin.Begin);

                if (function == myContext.IRP_MJ_READ)
                {
                    // driver request read into buffer
                    filehandle.fs.Read(buffer, 0, (int)length);
                    CopyToGlobalBuffer(buffer, 0, filehandle.__buffer0, 0, (int)length);
                    filehandle.fdpObject.SetSHMBuffer(filehandle.ctx, 0, length, (long)filehandle.__buffer0);
                    filehandle.totalDataRead_BigDecimal += length;
                    filehandle.bandwidthRead += length;
                }
                else if (function == myContext.IRP_MJ_WRITE)
                {
                    // driver request write through buffer into virtual disk file
                    filehandle.fdpObject.GetSHMBuffer(filehandle.ctx, 0, length, (long)filehandle.__buffer0);
                    CopyFromGlobalBuffer(filehandle.__buffer0, 0, buffer, 0, (int)length);
                    filehandle.fs.Write(buffer, 0, (int)length);
                    filehandle.fs.Flush();
                    filehandle.totalDataWrite_BigDecimal += length;
                    filehandle.bandwidthWrite += length;
                }
                else
                {
                    // (syncronization) error or bug
                }

                // finally set the proxy idle event for the driver to unblock and process the reply
                filehandle.fdpObject.SetEventDriverRequestDataSet(filehandle.ctx, 0);
                filehandle.fdpObject.SetEventProxyIdle(filehandle.ctx, 1);

                while (true)
                {
                    // terminate if user set the termination flag
                    if (terminateProxy)
                    {
                        filehandle.myThreadWaitHandle.Set();
                        return;
                    }
                    // wait for driver event to occur
                    // When Driver sets RequestComplete Event, it completes the sequence of entire request.
                    WaitStatus = filehandle.fdpObject.WaitEventRequestComplete(filehandle.ctx, 10000);
                    if (WaitStatus == myContext.WAIT_OBJECT_0)
                    {
                        // success, the driver set the event flag, so break.
                        filehandle.fdpObject.SetEventRequestComplete(filehandle.ctx, 0);
                        break;
                    }
                    else if (WaitStatus == myContext.WAIT_TIMEOUT)
                    {
                        // the driver did not set the event flag, so continue
                        continue;
                    }
                    else
                    {
                        // error or exception, continue
                        continue;
                    }
                }
                // here, the request sequence completes, then loop reloops.
            }

            // phase 2 = completion and close
            // Server Thread completes and terminates here.
        }

        public static void CopyGlobalBuffers(IntPtr srcGlobalBuffer, long srcindex, IntPtr destGlobalBuffer, long destGlobalBufferSize, long destindex, long count)
        {
            IntPtr pSource0 = new IntPtr(srcGlobalBuffer.ToInt64() + srcindex);
            IntPtr pDestination0 = new IntPtr(destGlobalBuffer.ToInt64() + destindex);

            unsafe
            {
                void* pSource = pSource0.ToPointer();//srcGlobalBuffer.ToPointer();
                void* pDestination = pDestination0.ToPointer();
                //var pSource = (byte*)(srcGlobalBuffer.ToPointer()) + srcindex;
                //var pDestination = (byte*)(destGlobalBuffer.ToPointer()) + destindex;
                System.Buffer.MemoryCopy(pSource, pDestination, destGlobalBufferSize, count);
            }
        }

        public static void CopyFromGlobalBuffer(IntPtr srcGlobalBuffer, int srcindex, byte[] dest, int destindex, int count)
        {
            IntPtr pSource = srcGlobalBuffer;
            pSource += srcindex;
            Marshal.Copy(pSource, dest, destindex, count);

        }
        //std::copy(src, src + count, dest);


        public static void CopyToGlobalBuffer(byte[] src, int srcindex, IntPtr destGlobalBuffer, int destindex, int count)
        {
            IntPtr pDestination = destGlobalBuffer;
            pDestination += destindex;
            Marshal.Copy(src, srcindex, pDestination, count);
            //std::copy(src, src + count, dest);
        }

        private void cmdLoadFile_Click(object sender, EventArgs e)
        {
            CreateOrOpenFile(false);

        }

        private void cmdUnloadFile_Click(object sender, EventArgs e)
        {
            if (txtContainerFile.Text.Length <= 0)
                return;

            // disable the form and stand by.

            this.Enabled = false;
            
            // delete entire session context and all objects
            filehandle.fdpObject.deregister_file(filehandle.ctx);
            filehandle.fdpObject.SetEventShutdown(filehandle.ctx, 1);
            filehandle.fdpObject.WaitEventShutdownComplete(filehandle.ctx, myContext.INFINITE);
            filehandle.fdpObject.delete_ctx(filehandle.ctx);
            filehandle.fdpObject.delete_objects(filehandle.ctx);

            // reset
            tssLabelVaultFile1.Text = "no vault file loaded.";
            txtContainerFile.Text = "";
            txtDrivePath.Text = "";

            // terminate server-client thread
            terminateProxy = true;
            filehandle.myThreadWaitHandle.WaitOne(Timeout.Infinite, false);
            filehandle.DisposeFinalizeAll();
            filehandle = null;
            this.Enabled = true;
            MessageBox.Show(this, "Successfully unloaded disk file! Click on OK to continue...", "file unload success.", MessageBoxButtons.OK, MessageBoxIcon.Information, MessageBoxDefaultButton.Button1, (MessageBoxOptions)0x40000);  // MB_TOPMOST
        }

        private void timerUpdateStatus_Tick(object sender, EventArgs e)
        {
            if (this.filehandle != null)
            {
                txtboxTotalDataRead.Text = String.Format("{0} GB", (((this.filehandle.totalDataRead_BigDecimal / 1024) / 1024) / 1024).ToPlainString());
                txtboxTotalDataWritten.Text = String.Format("{0} GB", (((this.filehandle.totalDataWrite_BigDecimal / 1024) / 1024) / 1024).ToPlainString());
                txtboxTotalDataReadMB.Text = String.Format("{0} MB", ((this.filehandle.totalDataRead_BigDecimal / 1024) / 1024).ToPlainString());
                txtboxTotalDataWrittenMB.Text = String.Format("{0} MB", ((this.filehandle.totalDataWrite_BigDecimal / 1024) / 1024).ToPlainString());

                // bandwidth
                tssLabelBandwidthReadMB.Text = String.Format("read: {0}/s", BytesConvertor.SizeSuffix(this.filehandle.bandwidthRead, 10));
                tssLabelBandwidthWriteMB.Text = String.Format("write: {0}/s", BytesConvertor.SizeSuffix(this.filehandle.bandwidthWrite, 10));//((this.filehandle.bandwidthWrite / 1024) / 1024));
                this.filehandle.bandwidthRead = this.filehandle.bandwidthWrite = 0;
                this.txtboxTotalRunningTime.Text = this.myStopWatchRunningTime.Elapsed.ToString("d' Days 'h' Hours 'm' Minutes 's' Seconds'");
            }
            else
            {
                tssLabelBandwidthReadMB.Text = String.Format("read: {0}/s", BytesConvertor.SizeSuffix(0, 10));
                tssLabelBandwidthWriteMB.Text = String.Format("write: {0}/s", BytesConvertor.SizeSuffix(0, 10));//((this.filehandle.bandwidthWrite / 1024) / 1024));
                timerUpdateStatus.Enabled = false;
                myStopWatchRunningTime.Stop();
                timerUpdateStatus.Stop();
            }

        }

        private void timerCheckContainer1Status_Tick(object sender, EventArgs e)
        {

        }

        private void cmdExploreVault_Click(object sender, EventArgs e)
        {
            if (txtContainerFile.Text.Length <= 0)
                return;

            commonMethods1.openFolder(filehandle.drivePath);

        }

        private void radioUseSocket_CheckedChanged(object sender, EventArgs e)
        {
            if (radioUseSocket.Checked)
                txtPort.Enabled = true;
            else
                txtPort.Enabled = false;
        }

        private void cmdClose_Click(object sender, EventArgs e)
        {
            if (filehandle != null)
            {
                MessageBox.Show(this, "error: you have loaded a file. please unload it before closing this form.", "cannot exit without unloading the file.", MessageBoxButtons.OK, MessageBoxIcon.Error, MessageBoxDefaultButton.Button1, (MessageBoxOptions)0x40000);  // MB_TOPMOST
                return;
            }

            this.properExit = true;
            GC.Collect();
            this.Close();
            this.Dispose();
        }
    }
}
