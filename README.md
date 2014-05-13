WirelessProximityMonitor_474-2135_Pittner-Sirianni-Swerling
===========================================================

## **Description:**
The goal of this project is to create a monitor of Wi-Fi traffic.  Using this traffic we 
will attempt to track and analyze relative location of wireless devices to the monitor.  
To achieve this, the Wireless Proximity Monitor (WPM) tool will use the following information: 
- _MAC address_
- _Timestamp_
- _Signal Strength_
Ultimately, this information will be used to create a log of the devices that were a certain 
distance away from the monitor at a particular time.  This data may then be parsed via display
and capture filters to find information for various personal uses.

## **Disclaimer:**
This tool was created for the Unix Forensics course at Rochester Institute of Technology.  After the end of the semester this tool is unlikely to be receive further development.

### **Dependencies:**
* [libpcap-1.5.3](http://www.tcpdump.org/release/libpcap-1.5.3.tar.gz)
* [gtkmm-3.0-dev](http://www.gtkmm.org/en/download.shtml)

	
### **Installation:**
1.  Include all appropriate .cpp and .h files in the same directory.
2.  Compile all files using command: **_g++ main.cpp mainwindow.cpp capture.cpp filter.cpp ReadWriteFiles.cpp Jzon.cpp -lpcap -o program `pkg-config --cflags --libs gtkmm-3.0`_**
3.  Run the program using the command: **_./program_**
4.  A new window should appear

### **Getting Started:**
* To start a capture:
    1.  Go to **Capture** in the top left of the window.
    2.  Click on **Start** in the drop down menu and a new window should appear.
    3.  In the new window, choose the interface you wish to capture on and include a capture filter if you'd like.
    4.  Click **Ok** and the capture will start.
	  * To apply *Display Filters* enter desired filter(s) in the box and click **Apply**.
	  * To clear *Display Filters* click **Clear**.
* To stop a capture:
	1.  Go to **Capture** in the top left of the window.
	2.  Click on **Stop** in the drop down menu and the capture will stop
* To Save a Capture:
    1.  Go to **File** in the top left of the window.
	2.  Click **Save** in the drop down menu and a new window should appear.
	3.  In the new window, enter in the name that you would like to save the file as and then click **Save**.
	  * **Captures will be saved in Json format**.
* To Open an existing Capture:
    1.  Go to **File** in the top left of the window.
	2.  Click **Open** in the drop down menu and a new window should appear.
	3.  In the new window, enter in the name of the file you would like to open and click **Open**.
* **Filters:**
	* Date:  **date=[start_date]-[end_date]**
	* Time:  **time=[start_time]-[end_time]**
	* Day:  **days=[day(s)_of_week]**
	* Month:  **months=[month(s)_of_year]**
	* Signal Strength:  **dbm=[min_dmb]-[max_dbm]**
	* Unique MAC Addresses: **uniqMac=[true]**
	* Single MAC Address: **mac=[MAC_address]**
	* Black list MAC addresses: **blackList=[saved_text_file]**
	* White list MAC addresses: **whiteList=[saved_text_file]**

#### **Authors:**
* Joseph Sirianni
* Ross Swerling
* Cal Pittner

* Johannes Häggqvist - **Jzon.cpp and Jzon.h**:  [Jzon files](https://code.google.com/p/jzon/)


###### **License: _Apachev2_**
