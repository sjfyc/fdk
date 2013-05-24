using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace FdkTest
{  
    class TestFramework
    {
        private class MyModule : Fdk.Module
        {
            public override bool CheckStart()
            {
                return MyModule2.Instance.IsStarted;
            }
            public override bool Start()
            {
                Console.WriteLine("MyModule start");
                return true;
            }
            public override bool Routine()
            {
                Thread.Sleep(1000);
                Console.WriteLine("MyModule.Routine(q to exit)");
                string s = Console.ReadLine();
                if (s == "q")
                {
                    return false;
                }
                return true;
            }
            public override void Stop()
            {
                Console.WriteLine("MyModule stop");
            }
            public static MyModule Instance
            {
                get
                {
                    return s_instance;
                }
            }
            private static readonly MyModule s_instance = new MyModule();
        }

        private class MyModule2 : Fdk.Module
        {
            public override bool CheckStart()
            {
                return MyModule.Instance.IsStarted;
            }
            public override bool Start()
            {
                Console.WriteLine("MyModule2 start");
                return true;
            }
            public override bool Routine()
            {
                Console.WriteLine("MyModule2 routine");
                return true;
            }
            public override void Stop()
            {
                Console.WriteLine("MyModule2 stop");
            }
            public static MyModule2 Instance
            {
                get
                {
                    return s_instance;
                }
            }
            private static readonly MyModule2 s_instance = new MyModule2();
        }

        public static void Test()
        {
            Global.Framework.RegisterModule(MyModule.Instance);
            Global.Framework.RegisterModule(MyModule2.Instance);
            try
            {
                Global.Framework.Start();
                while (true)
                {
                    Global.Framework.Routine();
                }
            }
            catch (Fdk.Framework.ModuleStartError e)
            {
                Console.WriteLine("{0} start error", e.Module.Name);                
            }
            catch (Fdk.Framework.ModuleRoutineError e)
            {
                Console.WriteLine("{0} routine error", e.Module.Name);
                Global.Framework.Stop();
            }
            catch (Fdk.Framework.ModulesCycleDependError e)
            {
                Console.WriteLine("ModulesCycleDependError error");
                foreach (Fdk.Module module in e.Modules)
                {
                    Console.WriteLine("{0}", module.Name);
                }
            }
        }
    }
}
