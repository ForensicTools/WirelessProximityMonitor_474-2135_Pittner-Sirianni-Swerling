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

	static void *createCaptureThread(void *context) {
		return ((MainWindow *)context)->printCapture();
	}

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
	
	Gtk::TreeView *treeview;
	Gtk::Label *label;
	Gtk::Entry *filter;
	Gtk::Button *enableFilterBtn;
	Gtk::Button *clearFilterBtn;
	Gtk::ScrolledWindow *scrollWindow;
	std::list<pthread_t> captureThreads;
	pthread_mutex_t mutex;
	std::list<packet_structure> packetsCaptured;
	std::list<packet_structure> packetsFiltered;
	FILTER captureFilter;
	FILTER displayFilter;
	bool capturing;
	std::vector<Gtk::RadioButton *>  dev_radio_btn;

protected:
	void on_open_click(void);
	void on_open_click_init(Gtk::Entry *);
	void on_open_cancel_click(void);
	void on_save_click(void);
	void on_save_click_init(Gtk::Entry *, std::list<packet_structure>);
	void on_save_cancel_click(void);
	void on_main_quit_click(void);
	void enable_filter(void);
	void clear_filter(void);
	void capture_window(void);
	void capture_window_ok_btn(Gtk::Entry *);
	void capture_window_cancel_btn(void);
	void error_window(std::string);
	void error_window_btn(void);
	void * printCapture(void);
	void print_packet(struct packet_structure);
	void captureStartBtn(void);
	void captureStopBtn(void);
  
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
	Glib::RefPtr<Gtk::ListStore> refTreeModel;

	Gtk::Window *captureWindow;
	Gtk::Window *saveWindow;
	Gtk::Window *openWindow;
	Gtk::Window *errorWindow;

	std::string captureDevice;

private:
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