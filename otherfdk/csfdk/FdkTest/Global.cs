using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace FdkTest
{
    class Global
    {
        public static Fdk.Log Log
        {
            get 
            {
                return Fdk.Log.Instance;
            }
        }

        public static Fdk.Framework Framework
        {
            get
            {
                return Fdk.Framework.Instance;
            }
        }
    }
}
