#include "mainwindow.h"
#include <iostream>
#include <iomanip>
#include <gtkmm.h>

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
		fileitem_open = Gtk::manage(new Gtk::MenuItem("_Open", true));
		fileitem_open->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::on_open_click));
		filemenu->append(*fileitem_open);
		fileitem_save = Gtk::manage(new Gtk::MenuItem("_Save", true));
		fileitem_save->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::on_save_click));
		filemenu->append(*fileitem_save);
		fileitem_exit = Gtk::manage(new Gtk::MenuItem("_Exit", true));
		fileitem_exit->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::on_quit_click));
		filemenu->append(*fileitem_exit);

	menuitem_capture = Gtk::manage(new Gtk::MenuItem("_Capture", true));
		menubar->append(*menuitem_capture);
		capturemenu = Gtk::manage(new Gtk::Menu());
		menuitem_capture->set_submenu(*capturemenu);
		captureitem_start = Gtk::manage(new Gtk::MenuItem("_Start", true));
		captureitem_stop = Gtk::manage(new Gtk::MenuItem("_Stop", true));
		captureitem_start->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::captureStartBtn));
		captureitem_stop->signal_activate().connect
			(sigc::mem_fun(*this, &MainWindow::captureStopBtn));
		capturemenu->append(*captureitem_start);
		capturemenu->append(*captureitem_stop);

    Gtk::Grid *grid = Gtk::manage(new Gtk::Grid);
    grid->set_border_width(10);
    grid->set_row_spacing(5);
    grid->set_column_spacing(5);
    vbox->add(*grid);

    Gtk::ScrolledWindow *scrollWindow = Gtk::manage(new Gtk::ScrolledWindow);
    scrollWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    grid->attach(*scrollWindow, 0, 1, 16, 8);

    label = Gtk::manage(new Gtk::Label);
    label->set_markup("Display Filter: ");
    grid->attach(*label, 0, 0, 1, 1);

    filter = Gtk::manage(new Gtk::Entry);
    filter->set_hexpand(true);
    grid->attach_next_to(*filter, *label, Gtk::POS_RIGHT, 13, 1);

    enable_filter = Gtk::manage(new Gtk::Button("Apply"));
    enable_filter->signal_clicked().connect
    	(sigc::mem_fun(*this, &MainWindow::toggle_filter));
    grid->attach_next_to(*enable_filter, *filter, Gtk::POS_RIGHT, 1, 1);

    clear_filter = Gtk::manage(new Gtk::Button("Clear"));
    clear_filter->signal_clicked().connect
    	(sigc::mem_fun(*this, &MainWindow::toggle_filter));
    grid->attach_next_to(*clear_filter, *enable_filter, Gtk::POS_RIGHT, 1, 1);

    treeview = Gtk::manage(new Gtk::TreeView);
    scrollWindow->add(*treeview);
    treeview->set_vscroll_policy(Gtk::SCROLL_NATURAL);
    treeview->set_hexpand(true);
    treeview->set_vexpand(true);
    vbox->pack_start(*scrollWindow);

    refTreeModel = Gtk::ListStore::create(columns);
    treeview->set_model(refTreeModel);
    treeview->append_column("Date", columns.col_date);
    treeview->append_column("Time", columns.col_time);
	treeview->append_column("MAC Address", columns.col_mac);
	treeview->append_column("dBm", columns.col_dbm);

    vbox->show_all();
}

MainWindow::~MainWindow()
{
	pthread_t tmp;
	while(!captureThreads.empty()) {
		tmp = captureThreads.front();
		pthread_exit( &tmp );
	}
}

void MainWindow::captureStartBtn(void) {
	pthread_t capture;
	pthread_create(&capture, NULL, &MainWindow::createCaptureThread, this);
	captureThreads.push_back(capture);
}

void MainWindow::captureStopBtn() {
	pthread_t tmp;
	while(!captureThreads.empty()) {
		tmp = captureThreads.front();
		captureThreads.pop_front();
		pthread_exit( &tmp );
	}
}

void * MainWindow::printCapture(void)
{
	struct packet_structure packet;
	Capture packet_capture;
	time_t epoch_time;
	struct tm * ptm;
	Gtk::TreeModel::Row row;
	Glib::ustring date;
	Glib::ustring gm_time;
	Glib::ustring mac;

	std::stringstream ss;
	ss << std::hex << std::setfill('0');


	for(;;) {//int i = 0; i <= 100; i++) {
		packet = packet_capture.CapturePacket();
		packetsFiltered.push_front(packet);

		//Convert epoch time
		epoch_time = packet.epoch_time;
		ptm = gmtime (&epoch_time);

		//Date in readable format
		date = static_cast<std::ostringstream*>
			( &(std::ostringstream() << ptm->tm_year+1900) )->str();
		date += "-";
		date += static_cast<std::ostringstream*>
			( &(std::ostringstream() << ptm->tm_mon+1) )->str();
		date += "-";
		date += static_cast<std::ostringstream*>
			( &(std::ostringstream() << ptm->tm_mday) )->str();
		
		//Time in readable format
		gm_time = static_cast<std::ostringstream*>
			( &(std::ostringstream() << ptm->tm_hour) )->str();
		gm_time += ":";
		gm_time += static_cast<std::ostringstream*>
			( &(std::ostringstream() << ptm->tm_min) )->str();
		gm_time += ":";
		gm_time += static_cast<std::ostringstream*>
			( &(std::ostringstream() << ptm->tm_sec) )->str();
		
		//MAC Address
		for(int i = 0; i <= 6; i++) {
			if(i!=0) {
				ss << ":";
			}
			ss << std::setw(2) << static_cast<unsigned>(packet.addr[i]);
		} mac = ss.str();

		//Output to GUI
		row = *(refTreeModel->append());
		row[columns.col_date] = date;
		row[columns.col_time] = gm_time;
		row[columns.col_mac] = mac;
		row[columns.col_dbm] = packet.dbm;
		treeview->queue_draw();
		ss.str(std::string());
		mac.clear();
		//usleep(1000 * 1000);
	}
	return 0;
}

void MainWindow::on_quit_click()
{
    hide();
}

void MainWindow::on_open_click() {
	Gtk::Window *window = new Gtk::Window();
	Gtk::Grid *grid = Gtk::manage(new Gtk::Grid);

	window->set_title ("Open Capture");
	window->set_border_width(10);
	window->set_default_size(300, 150);
	window->set_position(Gtk::WIN_POS_CENTER);

	Gtk::Label *fileNameLabel = new Gtk::Label();
	Gtk::Entry *fileNameEntry = new Gtk::Entry();
	Gtk::Button *open_btn = new Gtk::Button("Open");
	Gtk::Button *cancel_btn = new Gtk::Button("Cancel");

	grid->set_border_width(5);
    grid->set_row_spacing(5);
    grid->set_column_spacing(5);
	window->add(*grid);

	fileNameLabel->set_markup("Enter a file name:");
	fileNameEntry->set_hexpand(true);

    grid->attach(*fileNameLabel, 0, 0, 1, 1);
    grid->attach(*fileNameEntry, 0, 1, 5, 1);
    grid->attach(*cancel_btn, 1, 2, 1, 1);
    grid->attach(*open_btn, 3, 2, 1, 1);

    open_btn->signal_clicked().connect
		(sigc::bind<std::string>
		(sigc::ptr_fun(&ReadWrite::writeToFile), 
		(std::string)fileNameEntry->get_text()) );
	cancel_btn->signal_clicked().connect
		(sigc::mem_fun(*this, &MainWindow::on_quit_click));

	window->show_all();
}

void MainWindow::on_save_click() {
	Gtk::Window *window = new Gtk::Window();
	Gtk::Grid *grid = Gtk::manage(new Gtk::Grid);

	window->set_title ("Save Capture");
	window->set_border_width(10);
	window->set_default_size(300, 150);
	window->set_position(Gtk::WIN_POS_CENTER);

	Gtk::Label *fileNameLabel = new Gtk::Label();
	Gtk::Entry *fileNameEntry = new Gtk::Entry();
	Gtk::Button *save_btn = new Gtk::Button("Save");
	Gtk::Button *cancel_btn = new Gtk::Button("Cancel");

	grid->set_border_width(5);
    grid->set_row_spacing(5);
    grid->set_column_spacing(5);
	window->add(*grid);

	fileNameLabel->set_markup("Enter a file name:");
	fileNameEntry->set_hexpand(true);

    grid->attach(*fileNameLabel, 0, 0, 1, 1);
    grid->attach(*fileNameEntry, 0, 1, 5, 1);
    grid->attach(*cancel_btn, 1, 2, 1, 1);
    grid->attach(*save_btn, 3, 2, 1, 1);

    save_btn->signal_clicked().connect
		(sigc::bind<std::string, std::list<packet_structure> >
		(sigc::ptr_fun(&ReadWrite::writeToFile), 
		fileNameEntry->get_text(), packetsFiltered) );
	cancel_btn->signal_clicked().connect
		(sigc::mem_fun(*this, &MainWindow::on_quit_click));

	window->show_all();
}

void MainWindow::toggle_filter() {

}

void MainWindow::capture_window() {

	Gtk::Window *window = new Gtk::Window();
	Gtk::Grid *grid = Gtk::manage(new Gtk::Grid);

	window->set_title ("Capture Settings");
	window->set_border_width(10);
	window->set_default_size(500, 200);
	window->set_position(Gtk::WIN_POS_CENTER);

	Gtk::RadioButton *int_radio_btn = new Gtk::RadioButton("wlan0");
	Gtk::Label *capture_filter_label = new Gtk::Label();
	Gtk::Entry *capture_filter_entry = new Gtk::Entry();
	Gtk::Button *ok_btn = new Gtk::Button("Ok");
	Gtk::Button *cancel_btn = new Gtk::Button("Cancel");

	grid->set_border_width(5);
    grid->set_row_spacing(5);
    grid->set_column_spacing(5);
	window->add(*grid);

	capture_filter_label->set_markup("Capture Filter: ");
	capture_filter_entry->set_hexpand(true);

	//ok_btn->signal_clicked().connect
	//	(sigc::mem_fun(*this, &MainWindow::push_packet));

	grid->attach(*int_radio_btn, 1, 0, 3, 1);
    grid->attach(*capture_filter_label, 0, 1, 1, 1);
    grid->attach_next_to(*capture_filter_entry, 
    	*capture_filter_label, Gtk::POS_RIGHT, 4, 1);
    grid->attach(*cancel_btn, 1, 2, 1, 1);
    grid->attach(*ok_btn, 3, 2, 1, 1);

	window->show_all();

	gtk_main();
}


void capture_window_ok_btn(void) {
	gtk_main_quit();
}