/*/
 * Project: Wireless Proximity Analyzer
 *
 * Repository: https://github.com/ForensicTools/WirelessProximityMonitor_474-2135_Pittner-Sirianni-Swerling
 *
 * Authors:
 *		Joe Sirianni
 *		Cal Pittner
 *		Ross Swerling
 * 
 * License: Apache v2
/*/


#ifndef GTKMM_MAINWINDOW_H
#define GTKMM_MAINWINDOW_H

#include <gtkmm.h>
#include <list>
#include <pcap.h>
#include <time.h>
#include <sstream>
#include <string>
#include <pthread.h>
#include "filter.h"
#include "capture.h"
#include "ReadWriteFiles.h"
#include "PacketStructure.h"

class MainWindow : public Gtk::Window
{
public:
	MainWindow();
	virtual ~MainWindow();

	//Begins the caputring thread
	//Used to get around requirement of threads 
		//needing a 'this' variable due to signal constraints
	static void *createCaptureThread(void *context) {
		return ((MainWindow *)context)->printCapture();
	}

	//Begins channel itteration
	static void* iterate_wifi_channels(void*) {
		while(1) {
			system("iwconfig wlan0 channel 1");
			usleep(1000 * 1000);
			system("iwconfig wlan0 channel 6");
			usleep(1000 * 1000);
			system("iwconfig wlan0 channel 11");
			usleep(1000 * 1000);
		}
	}

	//Main window widgets	
	Gtk::TreeView *treeview;
	Gtk::Label *label;
	Gtk::Entry *filter;
	Gtk::Button *enableFilterBtn;
	Gtk::Button *clearFilterBtn;
	Gtk::ScrolledWindow *scrollWindow;
	pthread_mutex_t mutex; //Mutex for locking GUI

	//List of packets not filtered
	std::list<packet_structure> packetsCaptured;
	std::list<packet_structure> packetsFiltered;

	//Filter set at capture time
	FILTER captureFilter;

	//Filter which affects packets displayed
	FILTER displayFilter;

	//Global caputring variable
	bool capturing;

	//List of devices
	//Required to get around signal constraints
	std::vector<Gtk::RadioButton *>  dev_radio_btn;

protected:
	void on_open_click(void);	//Creates new window for opening capture
	void on_open_click_init(Gtk::Entry *);	//Opens capture
	void on_open_cancel_click(void);	//Closes open window
	void on_save_click(void);	//Creates new window for saving capture
	void on_save_click_init(Gtk::Entry *); //Saves capture
	void on_save_cancel_click(void);	//Closes save window
	void on_main_quit_click(void);		//Closes main program
	void enable_filter(void);	//Enables display filter
	void clear_filter(void);	//Clears display filter
	void capture_window(void);	//Creates window for capture settings
	void capture_window_ok_btn(Gtk::Entry *);	//Initializes capture
	void capture_window_cancel_btn(void);	//Closes caputre window
	void error_window(std::string);	//Error window
	void error_window_btn(void);	//Closes error window
	void * printCapture(void);		//Captures packets
	void print_packet(struct packet_structure);	//Prints packets
	void captureStopBtn(void);	//stops capture
  
  	//Columns for the packets displayed
	class ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
		public:
			ModelColumns()
			{ add(col_date); add(col_time); add(col_mac); add(col_dbm); }

			Gtk::TreeModelColumn<Glib::ustring> col_date;
			Gtk::TreeModelColumn<Glib::ustring> col_time;
			Gtk::TreeModelColumn<Glib::ustring> col_mac;
			Gtk::TreeModelColumn<int> col_dbm;
	};
	
	ModelColumns columns;
	Glib::RefPtr<Gtk::ListStore> refTreeModel; //List of packets displayed on GUI

	//Additional windows 
	Gtk::Window *captureWindow;
	Gtk::Window *saveWindow;
	Gtk::Window *openWindow;
	Gtk::Window *errorWindow;

	//The device being captured on
	std::string captureDevice;

private:
	//MainWindow tool bar items
	Gtk::MenuItem *menuitem_file;
	Gtk::Menu *filemenu;
		Gtk::MenuItem *fileitem_exit;
		Gtk::MenuItem *fileitem_open;
		Gtk::MenuItem *fileitem_save;

	Gtk::MenuItem *menuitem_capture;
	Gtk::Menu *capturemenu;
		Gtk::MenuItem *captureitem_start;
		Gtk::MenuItem *captureitem_stop;
};

#endif //GTKMM_MAINWINDOW_H