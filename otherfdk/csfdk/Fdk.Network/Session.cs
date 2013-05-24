using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Fdk.Network
{
    public abstract class Session
    {
        public void SendMessage() {}
        public virtual void OnOpened() { }
        public virtual void OnClosing() { }
    }
}
