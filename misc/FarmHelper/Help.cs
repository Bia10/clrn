using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace FarmHelper
{
    public class Help
    {
        public static void StrCopy(ref char[] szResult, string sSource)
        {
            szResult = new char[100];
            for (int i = 0; i < sSource.Length; ++i)
                szResult[i] = sSource[i];
            szResult[sSource.Length] = '\0';
        }

    }
}
