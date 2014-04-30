===========================================================
WirelessProximityMonitor_474-2135_Pittner-Sirianni-Swerling
===========================================================


Description
-----------
This project captures Wi-Fi traffic and parses out the source MAC, time, and signal strength. Using these three data points traffic the flow of traffic may be monitored and analyzed over the desired time period.  Both display and capture filters may be applied to remove unwanted data for more tailored uses.  


Creators
--------
Joe Sirianni
	Email: jws1353@rit.edu
	
Ross Swerling
	Email: rxs7360@rit.edu
	
Cal Pittner
	Email: cdp6985@rit.edu

	
Installation
------------
g++ main.cpp mainwindow.cpp capture.cpp ReadWriteFiles.cpp Jzon.cpp -o program `pkg-config --cflags --libs gtkmm-3.0`


Dependencies
------------
libpcap-1.5.3 - http://www.tcpdump.org/release/libpcap-1.5.3.tar.gz
libgtk-3.0-dev


License
-------
Apache v2


Features
--------
