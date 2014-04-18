#ifndef GTKMM_MAINWINDOW_H
#define GTKMM_MAINWINDOW_H

#include <gtkmm.h>

class MainWindow : public Gtk::Window
{
public:
	MainWindow();
	virtual ~MainWindow();
	Gtk::TreeView *treeview;
	Gtk::Label *label;
	Gtk::Entry *filter;
	Gtk::Button *enable_filter;
	Gtk::Button *clear_filter;
	
protected:
	void on_quit_click();
	void push_packet();
	void toggle_filter();
	void capture_window();
  
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

	Gtk::MenuItem *menuitem_capture;
	Gtk::Menu *capturemenu;
		Gtk::MenuItem *captureitem_start;
		Gtk::MenuItem *captureitem_stop;
};

#endif //GTKMM_MAINWINDOW_H