/*
 * StateObject.cs
 *
 * State tracker required for Asynchronous sockets
 * 
 * Taken from MSDN
 * http://msdn.microsoft.com/en-us/library/bbx2eya8.aspx
 * 
 * With Modifications by Joseph East 2011
 * 
 * (C) Microsoft 2010
 * Licensed under MS-PL
 * http://msdn.microsoft.com/en-us/cc300389
 * 
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Sockets;

namespace DroneTester
{
    class StateObject
    {
        // Client socket.
        public Socket workSocket = null;
        // Size of receive buffer.
        public const int BufferSize = 256;
        // Receive buffer.
        public byte[] buffer = new byte[BufferSize];
        // Received data string.
        public StringBuilder sb = new StringBuilder();
    }
}
