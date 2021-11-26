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

        public static FileStream CreateInitializeFile(String strPath, Int64 length, bool overwrite = true)
        {
            FileStream fs = null;
            try
            {
                if (overwrite)
                {
                    fs = new FileStream(strPath, FileMode.Create, FileAccess.ReadWrite, FileShare.None, 1048576, FileOptions.RandomAccess);
                    fs.SetLength(length);
                    fs.Flush();
                }
                else
                {
                    fs = new FileStream(strPath, FileMode.Open, FileAccess.ReadWrite, FileShare.None, 1048576, FileOptions.RandomAccess);
                }
            }
            catch
            {
                return null;
            }
            return fs;
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
