using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Windows.Forms;
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

namespace FilediskProxyNet
{
    public static class commonMethods1
    {
        public static String GetDrivePathByFolderPath(String path)
        {
            FileInfo f = new FileInfo(path);
            return Path.GetPathRoot(f.FullName);
        }
        public static String AddPathSep(String strpathstring)
        {
            String strPath = System.IO.Path.GetFullPath(strpathstring);
            char pathsep = strPath.ElementAt(strPath.Length - 1);
            if (pathsep != System.IO.Path.DirectorySeparatorChar)
            {
                strPath = strPath + System.IO.Path.DirectorySeparatorChar;
            }

            return strPath;
        }
        public static String getFullPathFromPathFile(String strPathFile)
        {
            if (!File.Exists(strPathFile))
                return "";

            try
            {
                return new FileInfo(strPathFile).DirectoryName;
            }
            catch
            {
                return "";
            }
        }

        public static String GetFullAppStartupPath()
        {
            String strPath = System.IO.Path.GetFullPath(Application.StartupPath);
            strPath = AddPathSep(strPath);
            return strPath;
        }
        public static bool DeleteFile(String strPathFile)
        {
            try
            {
                File.Delete(strPathFile); ;
            }
            catch
            {
                // Access denied or error while deleting the file.
                return false;

            }
            return true;
        }

        public static bool EraseFile(String strPath, Int64 length, Int64 iterations, bool deleteFile = false)
        {
            FileStream fs = null;
            RNGCryptoServiceProvider rng = new RNGCryptoServiceProvider();
            int sectorSize = 1048576; // 1 mb

            // this method erases the file beyond any recovery. data is completely destroyed. this is for security reasons.

            try
            {
                for (Int64 i = 0; i < iterations; i++)
                {
                    fs = new FileStream(strPath, FileMode.Open, FileAccess.ReadWrite, FileShare.None, sectorSize, FileOptions.RandomAccess);
                    fs.Seek(0, SeekOrigin.Begin);

                    long sectors = length / sectorSize;
                    byte[] sector = new byte[sectorSize];

                    // first erase sector by sector
                    for (long ctr = 0; ctr < sectors; ctr++)
                    {
                        // erase 3 times every sector. this overwrites beyond recovery.
                        long pos = fs.Position;
                        rng.GetBytes(sector);
                        fs.Write(sector, 0, sectorSize);
                        fs.Flush();
                        fs.Seek(pos, SeekOrigin.Begin);
                        rng.GetBytes(sector);
                        fs.Write(sector, 0, sectorSize);
                        fs.Flush();
                        fs.Seek(pos, SeekOrigin.Begin);
                        rng.GetBytes(sector);
                        fs.Write(sector, 0, sectorSize);
                        fs.Flush();
                    }

                    // then erase overwrite the remaining bytes 3 times so that the data is completely destroyed.
                    if (fs.Position != fs.Length)
                    {
                        long pos = fs.Position;
                        rng.GetBytes(sector);
                        fs.Write(sector, 0, (int)(fs.Length % sectorSize));
                        fs.Flush();
                        fs.Seek(pos, SeekOrigin.Begin);
                        rng.GetBytes(sector);
                        fs.Write(sector, 0, (int)(fs.Length % sectorSize));
                        fs.Flush();
                        fs.Seek(pos, SeekOrigin.Begin);
                        rng.GetBytes(sector);
                        fs.Write(sector, 0, (int)(fs.Length % sectorSize));
                        fs.Flush();
                    }

                    fs.Flush();
                    fs.Close();
                    fs.Dispose();
                    fs = null;
                }

                // iterations erasure completed, now finally delete the file if required.
                if (deleteFile)
                    DeleteFile(strPath);
            }
            catch
            {
                rng.Dispose();
                if (fs != null)
                {
                    fs.Close();
                    fs.Dispose();
                    fs = null;
                }
                return false;
            }
            rng.Dispose();
            return true;
        }

        public static bool initializeFile(String strPath, Int64 length)
        {
            FileStream fs = null;
            int sectorSize = 1048576; // 1 mb

            try
            {
                // initialize a new file
                fs = new FileStream(strPath, FileMode.Create, FileAccess.ReadWrite, FileShare.None, sectorSize, FileOptions.RandomAccess);
                fs.Seek(0, SeekOrigin.Begin);

                long sectors = length / sectorSize;
                byte[] sector = new byte[sectorSize];

                // write all sectors with 0s
                for (long ctr = 0; ctr < sectors; ctr++)
                {
                    fs.Write(sector, 0, sectorSize);
                    fs.Flush();
                }

                // create the last remaining length in the file
                if (fs.Position != length)
                {
                    fs.Write(sector, 0, (int)(length % sectorSize));
                    fs.Flush();
                }

                fs.Flush();
                fs.Close();
                fs.Dispose();
                fs = null;
            }
            catch
            {
                if (fs != null)
                {
                    fs.Close();
                    fs.Dispose();
                    fs = null;
                }
                return false;
            }
            return true;
        }

        public static FileStream CreateInitializeFile(String strPath, Int64 length, bool overwrite = true, bool returnHandle = true)
        {
            FileStream fs = null;
            int sectorSize = 1048576; // 1 mb

            if (File.Exists(strPath))
            {
                // file already exists.

                if (overwrite)
                {
                    // overwrite and create and initialize a new file
                    if (!initializeFile(strPath, length))
                        return null; // error
                }
                else
                {
                    // open the existing file later.
                }
            }
            else
            {
                // file does not exists. so create and initialize a new file
                if (!initializeFile(strPath, length))
                    return null; // error
            }

            if (returnHandle)
            {
                // file created or existed, so simply load the file and return it's handle
                fs = new FileStream(strPath, FileMode.Open, FileAccess.ReadWrite, FileShare.None, sectorSize, FileOptions.RandomAccess);
                fs.Seek(0, SeekOrigin.Begin);
                return fs;
            }
            else
            {
                // user does not wants us to load the file and return handle, so return with null without opening the file.
                return null;
            }
        }

        public static Boolean openFolder(String strFolderPath)
        {
            if(Directory.Exists(strFolderPath))
            {
                try
                {
                    System.Diagnostics.Process.Start("explorer.exe", strFolderPath);
                    return true;
                }
                catch
                {
                }
            }
            return false;
        }

        public static String GenerateTimeDateFromString(String strInputDate, String strInputTime, out DateTime output)
        {
            String[] strsplitDateArray = strInputDate.Split('-');
            String[] strsplitTimeArray = strInputTime.Split(':');
            String[] strsplitSecondsArray = strsplitTimeArray[2].Split('.');
            DateTime objDateTime = new DateTime(int.Parse(strsplitDateArray[2]), int.Parse(strsplitDateArray[1]), int.Parse(strsplitDateArray[0]), int.Parse(strsplitTimeArray[0]), int.Parse(strsplitTimeArray[1]), int.Parse(strsplitSecondsArray[0]), int.Parse(strsplitSecondsArray[1]), DateTimeKind.Utc);
            String strString = objDateTime.ToString("dddd, d MMMM, yyyy, hh-mm-ss tt");
            output = objDateTime;
            return strString;
        }
    }
}
