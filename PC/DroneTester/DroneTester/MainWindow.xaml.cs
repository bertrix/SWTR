/*
 * MainWindow.xaml.cs
 * 
 * The guts of the program
 * 
 * Fairly self-explanatory I think
 * Give a yell on the wiki if there's anything that isn't
 * https://github.com/bertrix/SWTR/
 * 
 * (C) Joseph East 2012
 * 
 * Code released under GPLV2
 * 
 * All brand and product names and associated logos contained within this program belong to their respective owners and are protected by copyright. 
 * Under no circumstance may any of these be reproduced outside this program without the prior written agreement of their owner. 
 * 
 */


using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Sockets;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;
using System.Diagnostics;

namespace DroneTester
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    /// 
    
    public partial class MainWindow : Window
    {
        /// <summary>
        /// Set aside an instance of the drone object here, as this form basically governs its operation
        /// </summary>
        private RCTX drone;

        /// <summary>
        /// The inertia timer slowly moves throttle back to 0. 
        /// We use a DispatcherTimer instead of a System.Timers.Timer as DispatchTimer runs in the same thread as the UI, IE no access issues with regards to updating UI objects.
        /// The code was based around a mishmash of samples, mainly these pages
        /// In addition s_refreshTimer is dedicated to GUI objects while inertia timer is for drone specific objects
        /// http://stackoverflow.com/questions/949458/how-to-have-a-wpf-binding-update-every-second
        /// http://www.codeproject.com/KB/IP/publishsubscribe.aspx
        /// http://www.dijksterhuis.org/using-timers-in-c/
        /// http://msdn.microsoft.com/en-us/library/system.windows.threading.dispatchertimer.aspx
        /// http://www.dotnetperls.com/timer
        /// </summary>
        private DispatcherTimer inertia;
        public static DispatcherTimer s_refreshTimer;

        public MainWindow()
        {
            InitializeComponent();
            // drone
            drone = new RCTX();
            // GUI timer
            StartRefreshTimer();
            // Servo timer
            inertia = new DispatcherTimer();
            inertia.Tick += new EventHandler(inertia_Elapsed);
            inertia.Interval = new TimeSpan(0, 0, 0, 0, 50); //set the timer to call its callback function every 50ms
            // drone timer
            VehicleTimer.StartTimer(drone); //initialise the packet sender
        }

        private void StartRefreshTimer()
        {
            s_refreshTimer = new DispatcherTimer();
            s_refreshTimer.Tick += new EventHandler(OnTimedEvent);
            // Set the Interval to 30mS.
            s_refreshTimer.Interval = new TimeSpan(0, 0, 0, 0, 30);
            s_refreshTimer.Start();
        }

        private void OnTimedEvent(object source, EventArgs e)
        {
            // Update the signal strength
            if (drone.isConnect())
            {
                lSignal.Content = drone.sStrength();
                lHMCBearing.Content = drone.HMCBearing();
                lAnalog.Content = drone.analogRead();
            }
        }

        private void inertia_Elapsed(object source, EventArgs e)
        {
           
            // We'll keep the one intertia timer for all vehicles
            // 99% of usage scanarios involve a single vehicle at a time
            // No point wasting resources on extra timers

            /*
            // This is for boat
            if (sl_engineLeft.Value != 0 || sl_engineRight.Value != 0)
            {
                if (sl_engineLeft.Value > 0) sl_engineLeft.Value -= 1;
                else sl_engineLeft.Value += 1;
                if (sl_engineRight.Value > 0) sl_engineRight.Value -= 1;
                else sl_engineRight.Value += 1;
            }

            // This is for the hovercraft
            // unimplemented!

            // This is for the car. No inertia for the turning
            if (sl_turnDirection.Value != 90 || sl_carSpeed.Value != 90)
            {

                if (sl_carSpeed.Value > 90) sl_carSpeed.Value -= 1;
                else if (sl_carSpeed.Value < 90) sl_carSpeed.Value += 1;
            }*/
        }

        private void btnConnect_Click(object sender, RoutedEventArgs e)
        {
            string inputIP; // used to store IP address inputted by textbox on the form
            IPAddress outputIP; // used to sanity check the IP
            inputIP = txtIP.Text;

            // Now sanity check the input IP

            try
            {
                outputIP = IPAddress.Parse(inputIP);
            }
            catch (FormatException)
            {
                MessageBox.Show("IP address out of range");
                txtIP.Text = "";
                return;
            }

            // Assign the IP address to the RCTX class

            drone.init(outputIP);

            // Set the button configuration

           
            btnDisconnect.IsEnabled = true;
            btnConnect.IsEnabled = false;


            // Initialise a connection to the RCRX routine

            drone.connect();

            // Change the indicator icon
            imgStatus.Source = new BitmapImage(new Uri(@"pack://application:,,,/DroneTester;component/img/green.png"));

            // Immediate error check, we may have connected to a valid TCP/IP device, but not a drone!
            if (drone.isError())
            {
                MessageBox.Show("There was a comms error");
                resetGUI();
            }
        }

        private void btnDisconnect_Click(object sender, RoutedEventArgs e)
        {
            resetGUI();
        }

        public void resetGUI()
        {
            drone.disconnect();
            lSignal.Content = "0";
            lHMCBearing.Content = "0";
            lAnalog.Content = "0";
            btnDisconnect.IsEnabled = false;
            btnConnect.IsEnabled = true;
            imgStatus.Source = new BitmapImage(new Uri(@"pack://application:,,,/DroneTester;component/img/red.png"));
            inertia.Stop();
        }

        private void slPin3_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (drone != null)
            {
                Byte[] a = new Byte[2];
                drone.getServos().CopyTo(a, 0);
                a[0] = (byte)slPin3.Value;
                drone.setServos(a);
            }
        }

        private void slPin5_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (drone != null)
            {
                Byte[] a = new Byte[2];
                drone.getServos().CopyTo(a, 0);
                a[1] = (byte)slPin5.Value;
                drone.setServos(a);
            }
        }
        
    }
}
