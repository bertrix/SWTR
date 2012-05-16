/*
 * VehicleTimer.cs
 * 
 * This is a butchered class from the 'original' BAE drone tester.
 * Removed all references to the sim environment.
 * The class houses timers only.
 * 
 * Sorry Pete :P
 * 
 * (C) Joseph East 2011
 * 
 * This library is released under the GPL V2
 * 
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Timers;

namespace DroneTester
{
    public static class VehicleTimer
    {
        public static Timer s_timer;
        private static RCTX drone;
        static VehicleTimer()
        {
            s_timer = new System.Timers.Timer();
            s_timer.Elapsed += new ElapsedEventHandler(OnTimedEvent);
            // Set the Interval to 5 seconds. (Was 5) 
            s_timer.Interval = 5000;
            s_timer.Enabled = true;
        }

        public static void StartTimer(RCTX _drone)
        {
            // Set the Interval to 0.1 seconds. (was 1)
            s_timer.Interval = 100;
            s_timer.Enabled = true;
            drone = _drone;
        }

        // Specify what you want to happen when the Elapsed event is raised.
        private static void OnTimedEvent(object source, ElapsedEventArgs e)
        {
            //Send packets to the drone if connection established
            if (drone.isConnect())
                drone.send();
        }
    }
}
