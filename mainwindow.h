#ifndef GTKMM_MAINWINDOW_H
#define GTKMM_MAINWINDOW_H

#include <gtkmm.h>
#include <list>
#include <time.h>
#include <sstream>
#include <string>
#include <pthread.h>
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
	
	Gtk::TreeView *treeview;
	Gtk::Label *label;
	Gtk::Entry *filter;
	Gtk::Button *enable_filter;
	Gtk::Button *clear_filter;
	std::list<pthread_t> captureThreads;
	std::list<packet_structure> packetsCaptured;
	std::list<packet_structure> packetsFiltered;

protected:
	void on_open_click();
	void on_save_click();
	void on_quit_click();
	void toggle_filter();
	void capture_window();
	void capture_window_ok_btn(void);
	void * printCapture(void);
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