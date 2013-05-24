using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace FdkTest
{
    class TestLog
    {
        public static void Test()
        {
            Global.Log.Level = Fdk.LogLevel.Error;
            Global.Log.WriteFatal("{0}", "fuck", 1);
        }
    }    
}
