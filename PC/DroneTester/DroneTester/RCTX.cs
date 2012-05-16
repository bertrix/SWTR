/*
 * A RCoIP implementation in C# over TCP
 * 
 * Not a very good one, the structures need a redo but it works
 * 
 * (C) Joseph East 2011
 * 
 * The libary compliments the DroneTester program as part of the SWTR package
 * https://github.com/bertrix/SWTR/
 * 
 * Original RCoIP specification is (C) Mike McCauley 2010
 * http://www.open.com.au/mikem/arduino/RCKit/
 * 
 * This library is released under the GPL V2
 * 
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Sockets;
using System.Net;
using System.Threading;
using System.Windows;
using System.Diagnostics;
using System.Timers; 

// Check out this article for your failsafe routine
// http://msdn.microsoft.com/en-us/library/system.net.sockets.socket.connected.aspx

namespace DroneTester
{

    /// <summary>
    /// This class defines an instance of RCTx.
    /// Basically contains the socket bindings to the Arduino drone and functions to send and receive packets
    /// The network interface is as non-blocking as possible, but it isn't self repairing. Worst case you may have to reset the program
    /// </summary>
    public class RCTX
    {
        // Thread handling objects for the network stack
        static ManualResetEvent connectDone = new ManualResetEvent(false);
        static ManualResetEvent sendDone = new ManualResetEvent(false);
        static ManualResetEvent receiveDone = new ManualResetEvent(false);
        
        // Socket handlers
        private IPAddress ipAddress;
        private static Socket s;
        private IPEndPoint endPoint;
        private EndPoint endPointYup;
        private StateObject state;

        // Packet contents
        private Byte[] version; // This must always be 0x1 for all instances.
        private Byte[] motorSet; // A 6 element array defined as { in 1, in 2, speed A, in 3, in 4, speed B } See Arduino code for more info
        // Actually it's a 4 element array now due to the inverting motorcontroller. 1,A,2,B
        // Really there should be an abstraction layer like on the Arduino side, but the code works and I'd rather not touch it til I have more time.
        private Byte[] servoSet; // stores servo values if used. Going to make dynamic hardware configuration is going to be fun

        /*
         * The entire packet structure needs a redo  
         * 
         */

        // Receive buffer, used for storing the reply packets
        // We use a list of bytes as by the name, it is trivial to append to it
        // It is static as the callback functions from the network routines write directly into it and as there's only once instance of the drone it doesn't matter.
        private static List<byte> appender;

        // State flag, true when connected. Used for timer loop in VehicleTime to determine if it should establish a connection and send packets.
        private bool _connected;

       // Drone sensor data buffers
       // private static string NMEA; // GPS NMEA string, currently unimplemented
        private static byte WirelessSignal; // Wireless signal strength, this is actually updated from the drone but isn't implemented anywhere yet
        private static float[] YPR; // Yaw , Pitch, Roll
        private static Byte[] Analog; // Analog channels, currently unimplemented
        private ushort _HMCBearing;
        private ushort analogData;

        private static bool error = true; // A flag for the connecting program to check
        

        /// <summary>
        /// Generic constructor, just fills in the packet arrays
        /// </summary>
        public RCTX()
        { 
            version = new Byte[1] { 0x1 }; // This must always be 0x1, for any and all cases of version
            motorSet = new Byte[4] { 0x1, 0x0, 0x1, 0x0 }; // sets the motors for forwrad motion with 0 throttle
            servoSet = new Byte[2] { 0x5A, 0x5A }; // initialise the servo
            appender = new List<byte>(); // This list of bytes stores the received data
            _connected = false; // Prevent the timer from playing with the class until we're ready
            ipAddress = null; // obviously
            WirelessSignal = 0x0;
            YPR = new float[3];
        }

        // Externally initialise the IP address
        public void init(IPAddress ipIn)
        {
            ipAddress = ipIn;
        }

        // This function replaces the motorSet array with a modified one for transmission
        public void setMotors(byte[] settings)
        {
            if (settings.Length == motorSet.Length)
                settings.CopyTo(motorSet, 0);
        }

        // Likewise, but with servos
        public void setServos(byte[] settings)
        {
            if (settings.Length == servoSet.Length)
                settings.CopyTo(servoSet, 0);
        }

        // Some getters
        public Byte[] getMotors()
        {
            return motorSet;
        }

        public Byte[] getServos()
        {
            return servoSet;
        }

        // Return connection status
        public bool isConnect()
        {
            return _connected;
        }

        public byte sStrength()
        {
            return WirelessSignal;
        }

        public float[] direction()
        {
            return YPR;
        }

        public ushort HMCBearing()
        {
            return _HMCBearing;
        }

        // Data from analog sensors
        public ushort analogRead()
        {
            return analogData;
        }
    


        // This function zeros everything, good for disconnects
        public void resetMotors()
        {
            new Byte[4] { 0x1, 0x0, 0x1, 0x0 }.CopyTo(motorSet, 0);
            new Byte[2] { 0x0, 0x0 }.CopyTo(servoSet, 0);
        }

        // This function scrapes the data from the packer buffer, does some processing and puts it into the 'clean' data buffers
        // It will need redoing when I get around to inplementing a new packet structure
        private void scrapeData()
        {
            
            WirelessSignal = appender[1];
            //analogData = unendianShort(3);
            //YPR[0] = unendian(3);
            //YPR[1] = unendian(7);
            //YPR[2] = unendian(11);
            //_HMCBearing = unendianShort(3);           
        }

        // This function extracts IEEE 754 compatible floats from the packet buffer using a defined offset.
        private float unendian(int start)
        {
            float a;
            byte[] ender = new Byte[4];
            appender.CopyTo(start, ender, 0, 4);
            a = BitConverter.ToSingle(ender, 0);
            return a;
        }

        // This function converts 2 bytes to short
        private ushort unendianShort(int start)
        {
            ushort a;
            byte[] ender = new Byte[2];
            appender.CopyTo(start, ender, 0, 2);
            //Array.Reverse(ender);
            a = BitConverter.ToUInt16(ender, 0);
            return a;
        }

        /// <summary>
        /// Connection routine: Establishes a connection to the unit, sends out the data and receives data.
        /// Now based on a single socket instead of thousands
        /// The connect/send/receive routines look complicated becasue they are, non-blocking I/O does that.
        /// I recommend this article for more info
        /// http://msdn.microsoft.com/en-us/library/bbx2eya8.aspx
        /// The failover is a message box informing that something went wrong and then resetting the state flag, Hopefully without crashing the program. 
        /// </summary>
        public void connect()
        {
            try
            {
                // Close any previous connection, can't be careful enough
                if (s != null && s.Connected)
                {
                    s.Shutdown(SocketShutdown.Both);
                    System.Threading.Thread.Sleep(10);
                    s.Close();
                }

                // generate new end-points and sockets, unfortunately we can't reuse dead ones, or non-existant ones.
                endPoint = new IPEndPoint(ipAddress, 9048);
                endPointYup = (endPoint);
                s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

                //s.SendTimeout = 100;
               // s.ReceiveTimeout = 100;
                ///appender = new List<byte>();

                // Attempt to connect for 1 second, otherwise error out.
                s.BeginConnect(endPointYup, new AsyncCallback(ConnectCallback), s);
                connectDone.WaitOne(1000);

                if (s.Connected)
                    //Begin initial transmission
                    send();
                else { throw new System.ArgumentException("Couldn't connect"); }
               
                //Let main program know we've got a connection
                _connected = s.Connected;
               
            }
            catch (Exception)
            {
                _connected = false;
                connectDone.Reset();
                sendDone.Reset();
                receiveDone.Reset();
                error = true;
                //MessageBox.Show("The connection failed1");
                
            }
        }

        public static void ConnectCallback(IAsyncResult ar)
        {
            try
            {
                Socket s = (Socket)ar.AsyncState;
                s.EndConnect(ar);
                connectDone.Set();
            }
            catch (Exception e)
            {
                //failsafe();
                error = true;
                //MessageBox.Show("con fail");
                Console.WriteLine(e.ToString());
            }
        }

        public void send()
        {
            try
            {
                if (s.Connected)
                {
                    error = false;
                    // Construct the data packet, basically concatenate arrays.
                    // We could use just arrays and the copyto method, but meh

                    /*
                     * 
                     *             
                     *  var z = new int[x.length + y.length];
                     *  x.CopyTo(z, 0);
                     *  y.CopyTo(z, x.length);
                     *  
                     */

                    /// We need a lot of modiciations here for dynamic hardware
                    /// This commented section is for L298 models
                 /*   ArraySegment<byte> asd1 = new ArraySegment<byte>(version);
                    ArraySegment<byte> asd2 = new ArraySegment<byte>(motorSet);
                    ArraySegment<byte> asd3 = new ArraySegment<byte>(servoSet); */


                    /// This section is for servo models like the RC car
                    ArraySegment<byte> asd1 = new ArraySegment<byte>(version);
                    ArraySegment<byte> asd2 = new ArraySegment<byte>(servoSet); 

                    // Concatenate the lists
                    List<ArraySegment<byte>> list = new List<ArraySegment<byte>> { asd1, asd2 };

                    // Assign
                    appender = new List<byte>();

                    // Asynchronously send the packet
                    s.BeginSend(list, SocketFlags.None, new AsyncCallback(SendCallback), s);
                    
                    
                    // Asynchronously receive a response packet (this is a function call, not a native routine)
                    Receive(s);

                    // Give 200ms for the receive thread to do its thing, otherwise error out
                    receiveDone.WaitOne(200);
                   
                    //sendDone.Reset();
                    //receiveDone.Reset();
                  
                    // Start to pull apart the received data
                    scrapeData();

                    // Update conneceted status again
                    _connected = s.Connected;
                }
                else
                    throw new System.ArgumentException("Not connected, Jan"); 
            }
            catch (Exception e)
            {
                _connected = false;
                connectDone.Reset();
                sendDone.Reset();
                receiveDone.Reset();
                error = true;
                //MessageBox.Show(e.ToString());
            }
        }

        private static void SendCallback(IAsyncResult ar)
        {
            try
            {
                // Retrieve the socket from the state object.
                Socket s = (Socket)ar.AsyncState;

                // Complete sending the data to the remote device. Return number of bytes sent
                int bytesSent = s.EndSend(ar);
                ///Console.WriteLine("Sent {0} bytes to server.", bytesSent);

                // Signal that all bytes have been sent to the thread.
                sendDone.Set();
            }
            catch (Exception e)
            {
                //failsafe();
               
                MessageBox.Show("send fail");
                Console.WriteLine(e.ToString());
            }
        }

        private static void Receive(Socket client)
        {
            try
            {
                // Create the state object.
                StateObject state = new StateObject();
                state.workSocket = client;

                // Begin receiving the data from the remote device.

                client.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0,
                    new AsyncCallback(ReceiveCallback), state);

            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        private static void ReceiveCallback(IAsyncResult ar)
        {
            try
            {
                // Retrieve the state object and the client socket 
                // from the asynchronous state object.
                StateObject state = (StateObject)ar.AsyncState;
                Socket client = state.workSocket;
                // Read data from the remote device.
                int bytesRead = client.EndReceive(ar);
                //Debug.WriteLine(bytesRead);
                if (bytesRead > 0)
                {
                    // There might be more data, so store the data received so far
                    appender.AddRange(state.buffer);
                    //  Get the rest of the data.
                    client.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0,
                        new AsyncCallback(ReceiveCallback), state);
                }
                else
                {
                    // All the data has arrived; 
                    // Signal that all bytes have been received.
                    receiveDone.Set();
                }
            }
            catch (Exception e)
            {
                //failsafe();
               
                //MessageBox.Show("rcv fail");
                Console.WriteLine(e.ToString());
            }
        }

        // Zero the motors and reset the connection flag meaning network transactions stop.
        public void disconnect()
        {
            resetMotors();
            send();
            failsafe();           
        }

        // Failsafe routine
        public void failsafe()
        {
            _connected = false;
            try
            {
                s.Close();
                connectDone.Reset();
                sendDone.Reset();
                receiveDone.Reset();
            }
            catch
            {
                connectDone.Reset();
                sendDone.Reset();
                receiveDone.Reset();
                error = true;
                //MessageBox.Show("The connection failed3");
            }
        }

        // A connection diagnosis function suggested by MSDN in the link at the very top of the code
        // Sends a non-blocking blank packet to the drone, as it doesn't contain a correct ID bit it will be discarded at the endpoint
        // but if it actually sends then we know the connection is still good.
        // So might be a good idea to run this routine before you start tearing down connections when it just could be a flag issue.
        // It's so far unused as there is currently no need to use it.
        public bool diagnose()
        {
            bool blockingState = s.Blocking;
            bool igglepiggle;
            try
            {
                byte [] tmp = new byte[1];

                s.Blocking = false;
                s.Send(tmp, 0, 0);
                Console.WriteLine("Connected!");
                igglepiggle =  true;
            }
            catch (SocketException e) 
            {               
                // 10035 == WSAEWOULDBLOCK
                if (e.NativeErrorCode.Equals(10035))
                Console.WriteLine("Still Connected, but the Send would block");
                else
                {   
                    Console.WriteLine("Disconnected: error code {0}!", e.NativeErrorCode);
                }
                igglepiggle = false;
            }
            finally
            {
                s.Blocking = blockingState;
                
            }
           
            Console.WriteLine("Connected: {0}", s.Connected);
            return igglepiggle;
        }
           
        // A break button, doesn't always work.
        public void bugout()
        {
            _connected = false;
        }

        public bool isError()
        {
            return error;
        }
    }
}
