WirelessProximityMonitor_474-2135_Pittner-Sirianni-Swerling
===========================================================

The goal of this project is to create a monitor of both Bluetooth and Wi-Fi traffic. Using this traffic we will attempt to track and analyze relative location to the monitor.  To achieve this, MAC addresses, timestamp, and signal strength will be logged.  Ultimately, this information will be used to create a log of what mobile devices were what distance away from the monitor at what time.  This data will be formatted using a graphical interface to better user readability.

Compile: 
g++ main.cpp mainwindow.cpp capture.cpp ReadWriteFiles.cpp Jzon.cpp filter.cpp -o program `pkg-config --cflags --libs gtkmm-3.0`

Dependencies:
	libpcap-1.5.3 - http://www.tcpdump.org/release/libpcap-1.5.3.tar.gz
	libgtk-3.0-dev
	
	