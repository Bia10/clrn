using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Management;
using System.IO;
using System.Windows.Forms;

namespace FarmHelper_beta
{
    public static class Defender
    {
        public static String GetSerial()
        {
            String SerialTxt = "";
            SelectQuery query = new SelectQuery("Win32_Processor");
            ManagementObjectSearcher searcher = new ManagementObjectSearcher(query);
            ManagementObjectCollection coll = searcher.Get();
            foreach (ManagementObject obj in coll)
                SerialTxt = obj.Properties["ProcessorId"].Value.ToString();
            char[] SerialChar = SerialTxt.ToCharArray();
            double result = 0;
            for (int i = 0; i < SerialChar.Length; i++)
                result += Math.Pow((int)SerialChar[i] * (int)SerialChar[i] - 30*(int)SerialChar[i], 3.64324698123764923d);
            result *= 12;
            return result.ToString();
        }
        public static bool CheckLicense(String LicenseKey)
        {
            StringBuilder SB = new StringBuilder();
            StringBuilder SB1 = new StringBuilder();
            String SerialID = GetSerial();
            char[] Chars = SerialID.ToString().ToCharArray();
            double R1 = 0;
            for (int i = 0; i < Chars.Length; i++)
            {
                int B = (int)Chars[i];
                int E = (int)Chars[Chars.Length - i - 1];
                double R = Math.Pow(B * E, 3);
                R1 = R1 + R * i;
                SB.Append(R1.ToString());
            }
            String Temp = SB.ToString();
            for (int i = 0; i < LicenseKey.Length; i++)
            {
                char C = LicenseKey.ToCharArray()[i];
                int C1 = Convert.ToInt16(C);
                if ((C1 >= 48) & (C1 <= 59))
                    SB1.Append((int)C1 - 48);
            }
            if (Temp == SB1.ToString())
                return true;
            else return false;
        }
        public static bool CheckLicense()
        {
            String LicenseKey = "";
            if (File.Exists(Application.StartupPath + "\\Data\\license.fh") == true)
                LicenseKey = File.ReadAllText(Application.StartupPath + "\\Data\\license.fh");
            StringBuilder SB = new StringBuilder();
            StringBuilder SB1 = new StringBuilder();
            String SerialID = GetSerial();
            char[] Chars = SerialID.ToString().ToCharArray();
            double R1 = 0;
            for (int i = 0; i < Chars.Length; i++)
            {
                int B = (int)Chars[i];
                int E = (int)Chars[Chars.Length - i - 1];
                double R = Math.Pow(B * E, 3);
                R1 = R1 + R * i;
                SB.Append(R1.ToString());
            }
            String Temp = SB.ToString();
            for (int i = 0; i < LicenseKey.Length; i++)
            {
                char C = LicenseKey.ToCharArray()[i];
                int C1 = Convert.ToInt16(C);
                if ((C1 >= 48) & (C1 <= 59))
                    SB1.Append((int)C1 - 48);
            }
            if (Temp == SB1.ToString())
                return true;
            else return false;
        }
    }
}
