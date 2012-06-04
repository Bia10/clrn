using System;
using System.IO;
using System.Net;
using System.Text;
using System.Reflection;
using System.Windows.Forms;

namespace FarmHelper_beta
{
    public class Updater
    {
        public String UpdaterSource, CurrentVersion, AvailableVersion;
        public Updater()
        {
            UpdaterSource = "http://code.google.com/p/farmhelper/downloads/list";
        }
        public bool CheckForUpdate()
        {
            CurrentVersion = Assembly.GetExecutingAssembly().GetName().Version.ToString(); //Этот метод получает версию текущей сборки. Т.е. версию вашей программы (Допустим она 1.0.0.0)
            StringBuilder sb = new StringBuilder();
            byte[] buf = new byte[8192];
            WebRequest request = (HttpWebRequest)
            WebRequest.Create(UpdaterSource);
            HttpWebResponse response = (HttpWebResponse)
                    request.GetResponse();
            Stream resStream = response.GetResponseStream();
            string tempString = null;
            int count = 0;
            do
            {
                // fill the buffer with data
                count = resStream.Read(buf, 0, buf.Length);
                // make sure we read some data
                if (count != 0)
                {
                    // translate from bytes to ASCII text
                    tempString = Encoding.ASCII.GetString(buf, 0, count);
                    // continue building the string
                    sb.Append(tempString);
                }
            }
            while (count > 0); // any more data to read?
            // print out page source
            String Str = sb.ToString();
            AvailableVersion = FindLastVersion(Str, "FarmHelper");
            if (AvailableVersion != CurrentVersion)
                return true;
            else return false;
        }
        public String FindLastVersion(String SourceStr, String FindStr)
        {
            int Start = 0;
            int End = 0;
            for (int i = 0; i < SourceStr.Length; i++)
            {
                if (SourceStr.Substring(i, 10) == FindStr)
                {
                    Start = i + FindStr.Length;
                    for (int n = Start; n < SourceStr.Length; n++)
                        if (SourceStr.Substring(n, 4) == ".exe")
                        {
                            End = n;
                            String Version = SourceStr.Substring(Start, End - Start);
                            return Version;
                        }
                }
            }
                return CurrentVersion;
        }
        public void DownloadNewVersion()
        {
            try
            {
                WebClient WC = new WebClient();
                WC.DownloadFile("http://farmhelper.googlecode.com/files/FarmHelper" + AvailableVersion + ".exe", Application.StartupPath + "\\Newversion.exe");
                WC.Dispose();
            }
            catch (Exception E) { WowControl.UpdateStatus(E.Message); }
        }
    }
}
