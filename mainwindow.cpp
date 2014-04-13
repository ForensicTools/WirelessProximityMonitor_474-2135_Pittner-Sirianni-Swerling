#include "mainwindow.h"
#include <iostream>
#include <gtk/gtk.h>

MainWindow::MainWindow()
{
	set_title ("Wireless Proximity Analyzer");
	set_border_width(10);
	set_default_size(800, 500);
	set_position(Gtk::WIN_POS_CENTER);

	Gtk::Box *vbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 0));
	add(*vbox);

	Gtk::MenuBar *menubar = Gtk::manage(new Gtk::MenuBar());
	vbox->pack_start(*menubar, Gtk::PACK_SHRINK, 0);

	menuitem_file = Gtk::manage(new Gtk::MenuItem("_File", true));
		menubar->append(*menuitem_file);
		filemenu = Gtk::manage(new Gtk::Menu());
		menuitem_file->set_submenu(*filemenu);
		fileitem_exit = Gtk::manage(new Gtk::MenuItem("_Exit", true));
		fileitem_exit->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::on_quit_click));
		filemenu->append(*fileitem_exit);

	menuitem_capture = Gtk::manage(new Gtk::MenuItem("_Capture", true));
		menubar->append(*menuitem_capture);
		capturemenu = Gtk::manage(new Gtk::Menu());
		menuitem_capture->set_submenu(*capturemenu);
		captureitem_start = Gtk::manage(new Gtk::MenuItem("_Start", true));
		captureitem_stop = Gtk::manage(new Gtk::MenuItem("_Stop", true));
		captureitem_start->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::capture_window));
		captureitem_stop->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::push_packet));
		capturemenu->append(*captureitem_start);
		capturemenu->append(*captureitem_stop);

    Gtk::Grid *grid = Gtk::manage(new Gtk::Grid);
    grid->set_border_width(10);
    grid->set_row_spacing(5);
    grid->set_column_spacing(5);
    vbox->add(*grid);

    label = Gtk::manage(new Gtk::Label);
    label->set_markup("Display Filter: ");
    grid->attach(*label, 0, 0, 1, 1);

    filter = Gtk::manage(new Gtk::Entry);
    grid->attach_next_to(*filter, *label, Gtk::POS_RIGHT, 13, 1);

    enable_filter = Gtk::manage(new Gtk::Button("Apply"));
    enable_filter->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::toggle_filter));
    grid->attach_next_to(*enable_filter, *filter, Gtk::POS_RIGHT, 1, 1);

    clear_filter = Gtk::manage(new Gtk::Button("Clear"));
    clear_filter->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::toggle_filter));
    grid->attach_next_to(*clear_filter, *enable_filter, Gtk::POS_RIGHT, 1, 1);

    Gtk::TreeView *treeview = Gtk::manage(new Gtk::TreeView);
    treeview->set_hexpand(true);
    treeview->set_vexpand(true);
    grid->attach(*treeview, 0, 1, 16, 8);

    refTreeModel = Gtk::ListStore::create(columns);
    treeview->set_model(refTreeModel);
    treeview->append_column("Date", columns.col_date);
    treeview->append_column("Time", columns.col_time);
	treeview->append_column("MAC Address", columns.col_mac);
	treeview->append_column("dBm", columns.col_dbm);

	/*
    label = Gtk::manage(new Gtk::Label);
    label->set_markup("<b>Text To Add: </b>");
    grid->attach(*label, 0, 1, 1, 1);

    text = Gtk::manage(new Gtk::Entry);
    grid->attach(*text, 1, 1, 2, 1);

    Gtk::Button *button = Gtk::manage(new Gtk::Button("Add Text"));
    button->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_click));
    grid->attach(*button, 2, 2, 1, 1);
    */
    vbox->show_all();
}

MainWindow::~MainWindow()
{
}

void MainWindow::push_packet()
{
	Glib::ustring date = "packet->date;";
	Glib::ustring time = "packet->time;";
	Glib::ustring mac = "packet->mac";
	int dbm = -42;
	
	Gtk::TreeModel::Row row = *(refTreeModel->append());
	row[columns.col_date] = date;
	row[columns.col_time] = time;
	row[columns.col_mac] = mac;
	row[columns.col_dbm] = dbm;
}

void MainWindow::on_quit_click()
{
    hide();
}

void MainWindow::toggle_filter() {

}

void MainWindow::capture_window() {
	GtkWidget *window;
	GtkWidget *box;
	GtkWidget *grid;

	GtkWidget *int_check_btn;
	GtkWidget *capture_filter_label;
	GtkWidget *capture_filter_entry;
	GtkWidget *ok_btn;
	GtkWidget *cancel_btn;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Capture Settings");
	gtk_window_set_default_size(GTK_WINDOW(window), 500, 200);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_window_set_keep_above(GTK_WINDOW(window), true);
	//gtk_window_set_transient_for(GTK_WINDOW(window) )

	int_check_btn = gtk_check_button_new_with_label("wlan0");

	capture_filter_label = gtk_label_new("Capture Filter: ");
	capture_filter_entry = gtk_entry_new();

	ok_btn = gtk_button_new_with_label("Ok");
	cancel_btn = gtk_button_new_with_label("Cancel");

	grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
	gtk_container_add(GTK_CONTAINER(window), grid);

	

    gtk_grid_attach(GTK_GRID(grid), int_check_btn, 1, 0, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), capture_filter_label, 0, 1, 1, 1);
    gtk_grid_attach_next_to(GTK_GRID(grid), capture_filter_entry, 
    	capture_filter_label, GTK_POS_RIGHT, 9, 1);
    gtk_grid_attach(GTK_GRID(grid), cancel_btn, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ok_btn, 3, 2, 1, 1);

	gtk_widget_show_all(window);

	//gtk_main();
}