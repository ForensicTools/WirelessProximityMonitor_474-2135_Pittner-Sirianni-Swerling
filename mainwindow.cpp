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

	capturing = false;

	Gtk::Box *vbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 0));
	add(*vbox);

	Gtk::MenuBar *menubar = Gtk::manage(new Gtk::MenuBar());
	vbox->pack_start(*menubar, Gtk::PACK_SHRINK, 0);

	menuitem_file = Gtk::manage(new Gtk::MenuItem("_File", true));
		menubar->append(*menuitem_file);
		filemenu = Gtk::manage(new Gtk::Menu());
		menuitem_file->set_submenu(*filemenu);
		fileitem_open = Gtk::manage(new Gtk::MenuItem("_Open", true));
		fileitem_open->signal_activate().connect(sigc::mem_fun
			(*this, &MainWindow::on_open_click));
		filemenu->append(*fileitem_open);
		fileitem_save = Gtk::manage(new Gtk::MenuItem("_Save", true));
		fileitem_save->signal_activate().connect(sigc::mem_fun
			(*this, &MainWindow::on_save_click));
		filemenu->append(*fileitem_save);
		fileitem_exit = Gtk::manage(new Gtk::MenuItem("_Exit", true));
		fileitem_exit->signal_activate().connect(sigc::mem_fun
			(*this, &MainWindow::on_main_quit_click));
		filemenu->append(*fileitem_exit);

	menuitem_capture = Gtk::manage(new Gtk::MenuItem("_Capture", true));
		menubar->append(*menuitem_capture);
		capturemenu = Gtk::manage(new Gtk::Menu());
		menuitem_capture->set_submenu(*capturemenu);
		captureitem_start = Gtk::manage(new Gtk::MenuItem("_Start", true));
		captureitem_stop = Gtk::manage(new Gtk::MenuItem("_Stop", true));
		captureitem_start->signal_activate().connect(sigc::mem_fun
			(*this, &MainWindow::capture_window));
		captureitem_stop->signal_activate().connect
			(sigc::mem_fun(*this, &MainWindow::captureStopBtn));
		capturemenu->append(*captureitem_start);
		capturemenu->append(*captureitem_stop);

    Gtk::Grid *grid = Gtk::manage(new Gtk::Grid);
    grid->set_border_width(10);
    grid->set_row_spacing(5);
    grid->set_column_spacing(5);
    vbox->add(*grid);

    scrollWindow = Gtk::manage(new Gtk::ScrolledWindow);
    scrollWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    grid->attach(*scrollWindow, 0, 1, 16, 8);

    label = Gtk::manage(new Gtk::Label);
    label->set_markup("Display Filter: ");
    grid->attach(*label, 0, 0, 1, 1);

    filter = Gtk::manage(new Gtk::Entry);
    filter->set_hexpand(true);
    grid->attach_next_to(*filter, *label, Gtk::POS_RIGHT, 13, 1);

    enableFilterBtn = Gtk::manage(new Gtk::Button("Apply"));
    enableFilterBtn->signal_clicked().connect
    	(sigc::mem_fun(*this, &MainWindow::enable_filter));
    grid->attach_next_to(*enableFilterBtn, *filter, Gtk::POS_RIGHT, 1, 1);

    clearFilterBtn = Gtk::manage(new Gtk::Button("Clear"));
    clearFilterBtn->signal_clicked().connect
    	(sigc::mem_fun(*this, &MainWindow::clear_filter));
    grid->attach_next_to(*clearFilterBtn, *enableFilterBtn, Gtk::POS_RIGHT, 1, 1);

    treeview = Gtk::manage(new Gtk::TreeView);
    scrollWindow->add(*treeview);
    treeview->set_vscroll_policy(Gtk::SCROLL_NATURAL);
    treeview->set_hexpand(true);
    treeview->set_vexpand(true);
    //vbox->add(*scrollWindow);

    refTreeModel = Gtk::ListStore::create(columns);
    treeview->set_model(refTreeModel);
    treeview->append_column("Date", columns.col_date);
    treeview->append_column("Time", columns.col_time);
	treeview->append_column("MAC Address", columns.col_mac);
	treeview->append_column("dBm", columns.col_dbm);

    show_all();
}

MainWindow::~MainWindow() {
	capturing = false;
}

void MainWindow::captureStartBtn(void) {
	pthread_t capture;
	pthread_create(&capture, NULL, &MainWindow::createCaptureThread, this);
	captureThreads.push_front(capture);
}

void MainWindow::captureStopBtn() {
	capturing = false;
}

void MainWindow::enable_filter(void) {
	pthread_mutex_lock(&mutex);
	std::string filterStr = filter->get_text();
	if( displayFilter.setFilter(filterStr.c_str()) ) {
		refTreeModel->clear();
		packetsFiltered.clear();

		for(std::list<packet_structure>::iterator it = packetsCaptured.begin(); 
			it != packetsCaptured.end(); it++) {

			if(!displayFilter.filterPacket(*it, packetsFiltered))
			{
				packetsFiltered.push_front(*it);
				print_packet(*it);
			}
		}
	}
	else
		error_window("Incorrect Filter Syntax");

	pthread_mutex_unlock(&mutex);
}

void MainWindow::clear_filter(void) {
	pthread_mutex_lock(&mutex);
	refTreeModel->clear();
	packetsFiltered.clear();
	displayFilter.clearFilter();
	filter->set_text("");
	for(std::list<packet_structure>::iterator it = packetsCaptured.begin(); 
			it != packetsCaptured.end(); it++) {
		print_packet(*it);
	}
	pthread_mutex_unlock(&mutex);
}

void * MainWindow::printCapture(void)
{
	capturing = true;
	struct packet_structure packet;
	Capture packet_capture (captureDevice);

	pthread_t chanThread;
	pthread_create(&chanThread, NULL, &MainWindow::iterate_wifi_channels, this);

	while(capturing == true) {
		packet = packet_capture.CapturePacket();
		pthread_mutex_lock(&mutex);

		if(!captureFilter.filterPacket(packet, packetsCaptured)) {
			packetsCaptured.push_back(packet);
			if(!displayFilter.filterPacket(packet, packetsFiltered))
			{
				packetsFiltered.push_front(packet);

				print_packet(packet);
			}
		}
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(&chanThread);
	return 0;
}

void MainWindow::print_packet(struct packet_structure packet) {
	time_t epoch_time = packet.epoch_time;
	struct tm *ptm = gmtime (&epoch_time);
	std::stringstream timeSs;
	std::stringstream dateSs;
	Gtk::TreeModel::Row row;
	dateSs << std::setfill('0');
	timeSs << std::setfill('0');

	//Date in readable format
	dateSs << std::setw(4) << static_cast<unsigned>(ptm->tm_year+1900);
	dateSs << "/";
	dateSs << std::setw(2) << static_cast<unsigned>(ptm->tm_mon+1);
	dateSs << "/";
	dateSs << std::setw(2) << static_cast<unsigned>(ptm->tm_mday);
	
	//Time in readable format
	timeSs << std::setw(2) << static_cast<unsigned>(ptm->tm_hour);
	timeSs << ":";
	timeSs << std::setw(2) << static_cast<unsigned>(ptm->tm_min);
	timeSs << ":";
	timeSs << std::setw(2) << static_cast<unsigned>(ptm->tm_sec);

	//Output to GUI
	row = *(refTreeModel->append());
	row[columns.col_date] = dateSs.str();
	row[columns.col_time] = timeSs.str();
	row[columns.col_mac] = packet.mac;
	row[columns.col_dbm] = packet.dbm;
	treeview->queue_draw();
}

void MainWindow::on_main_quit_click()
{
    hide();
}

void MainWindow::on_open_click() {
	openWindow = new Gtk::Window();
	Gtk::Grid *grid = Gtk::manage(new Gtk::Grid);

	openWindow->set_title ("Open Capture");
	openWindow->set_border_width(10);
	openWindow->set_default_size(300, 150);
	openWindow->set_position(Gtk::WIN_POS_CENTER);

	Gtk::Label *fileNameLabel = new Gtk::Label();
	Gtk::Entry *fileNameEntry = new Gtk::Entry();
	Gtk::Button *open_btn = new Gtk::Button("Open");
	Gtk::Button *cancel_btn = new Gtk::Button("Cancel");

	grid->set_border_width(5);
    grid->set_row_spacing(5);
    grid->set_column_spacing(5);
	openWindow->add(*grid);

	fileNameLabel->set_markup("Enter a file name:");
	fileNameEntry->set_hexpand(true);

    grid->attach(*fileNameLabel, 0, 0, 1, 1);
    grid->attach(*fileNameEntry, 0, 1, 5, 1);
    grid->attach(*cancel_btn, 1, 2, 1, 1);
    grid->attach(*open_btn, 3, 2, 1, 1);

    open_btn->signal_clicked().connect
		(sigc::bind<Gtk::Entry *>
		(sigc::mem_fun(*this, &MainWindow::on_open_click_init), 
		fileNameEntry) );
	cancel_btn->signal_clicked().connect
		(sigc::mem_fun(*this, &MainWindow::on_open_cancel_click));

	openWindow->show_all();
}

void MainWindow::on_open_click_init(Gtk::Entry *fileNameEntry){
	ReadWrite open;
	std::string fileName = fileNameEntry->get_text();

	open.readFromFile(fileName.c_str());
	openWindow->hide();
}

void MainWindow::on_open_cancel_click() {
	openWindow->hide();
}

void MainWindow::on_save_click() {
	saveWindow = new Gtk::Window();
	Gtk::Grid *grid = Gtk::manage(new Gtk::Grid);

	saveWindow->set_title ("Save Capture");
	saveWindow->set_border_width(10);
	saveWindow->set_default_size(300, 150);
	saveWindow->set_position(Gtk::WIN_POS_CENTER);

	Gtk::Label *fileNameLabel = new Gtk::Label();
	Gtk::Entry *fileNameEntry = new Gtk::Entry();
	Gtk::Button *save_btn = new Gtk::Button("Save");
	Gtk::Button *cancel_btn = new Gtk::Button("Cancel");

	grid->set_border_width(5);
    grid->set_row_spacing(5);
    grid->set_column_spacing(5);
	saveWindow->add(*grid);

	fileNameLabel->set_markup("Enter a file name:");
	fileNameEntry->set_hexpand(true);

    grid->attach(*fileNameLabel, 0, 0, 1, 1);
    grid->attach(*fileNameEntry, 0, 1, 5, 1);
    grid->attach(*cancel_btn, 1, 2, 1, 1);
    grid->attach(*save_btn, 3, 2, 1, 1);

    save_btn->signal_clicked().connect
		(sigc::bind<Gtk::Entry *, std::list<packet_structure> >
		(sigc::mem_fun(*this, &MainWindow::on_save_click_init), 
		fileNameEntry, packetsFiltered) );
	cancel_btn->signal_clicked().connect
		(sigc::mem_fun(*this, &MainWindow::on_save_cancel_click));

	saveWindow->show_all();
}

void MainWindow::on_save_click_init(Gtk::Entry *fileNameEntry, std::list<packet_structure> saveList){
	ReadWrite save;
	std::string fileName = fileNameEntry->get_text();

	save.writeToFile(fileName.c_str(), saveList);
	saveWindow->hide();
}

void MainWindow::on_save_cancel_click() {
	saveWindow->hide();
}

void MainWindow::capture_window() {
	char errbuff[PCAP_ERRBUF_SIZE];
	pcap_if_t *d;
	pcap_if_t *allDevs;
	dev_radio_btn.clear();

	captureWindow = new Gtk::Window();
	Gtk::Grid *grid = Gtk::manage(new Gtk::Grid);

	captureWindow->set_title ("Capture Settings");
	captureWindow->set_border_width(10);
	captureWindow->set_default_size(500, 200);
	captureWindow->set_position(Gtk::WIN_POS_CENTER);

	Gtk::Label *capture_filter_label = new Gtk::Label();
	Gtk::Entry *capture_filter_entry = new Gtk::Entry();
	Gtk::Button *ok_btn = new Gtk::Button("Ok");
	Gtk::Button *cancel_btn = new Gtk::Button("Cancel");
	Gtk::RadioButton::Group rbGroup;
	std::vector<Gtk::Label *> dev_desc_label;

	if(pcap_findalldevs(&allDevs, errbuff) == -1){
		exit(1);
	}
	
	for(d = allDevs; d != NULL; d = d->next) {
		dev_radio_btn.push_back(new Gtk::RadioButton(rbGroup, d->name));
		if(!d->description)
			dev_desc_label.push_back(new Gtk::Label("No Description"));
		else
			dev_desc_label.push_back(new Gtk::Label(d->description));
	}

	pcap_freealldevs(allDevs);

	grid->set_border_width(5);
    grid->set_row_spacing(5);
    grid->set_column_spacing(5);
	captureWindow->add(*grid);

	capture_filter_label->set_markup("Capture Filter: ");
	capture_filter_entry->set_hexpand(true);

	int i = 0;
	while(i <= (dev_radio_btn.size() - 1)) {
		grid->attach(*(dev_radio_btn[i]), 1, i, 1, 1);
		grid->attach_next_to(*(dev_desc_label[i]),
							*(dev_radio_btn[i]),
							 Gtk::POS_RIGHT, 2, 1);
		i++;
	}

    grid->attach(*capture_filter_label, 0, ++i, 1, 1);
    grid->attach_next_to(*capture_filter_entry, 
    	*capture_filter_label, Gtk::POS_RIGHT, 4, 1);
    grid->attach(*cancel_btn, 2, ++i, 1, 1);
    grid->attach_next_to(*ok_btn, *cancel_btn, Gtk::POS_RIGHT, 1, 1);

    ok_btn->signal_clicked().connect
    	(sigc::bind<Gtk::Entry *>
		(sigc::mem_fun(*this, &MainWindow::capture_window_ok_btn),
		capture_filter_entry) );

	cancel_btn->signal_clicked().connect
		(sigc::mem_fun(*this, &MainWindow::capture_window_cancel_btn));

	captureWindow->show_all();
}

void MainWindow::capture_window_ok_btn(Gtk::Entry *filterEntry) {
	bool threadStart = false;
	std::string filterString = filterEntry->get_text();
	for(int i = 0; i <= (dev_radio_btn.size() - 1); i++)
		if(dev_radio_btn[i]->get_active() == true) {
			captureDevice = dev_radio_btn[i]->get_label();
			captureFilter.setFilter(filterString.c_str());
			pthread_t capture;
			pthread_create(&capture, NULL, &MainWindow::createCaptureThread, this);
			threadStart = true;
			break;
		}
	captureWindow->hide();
}

void MainWindow::capture_window_cancel_btn(void) {
	captureWindow->hide();
}

void MainWindow::error_window(std::string error) {
	errorWindow = new Gtk::Window();
	Gtk::Grid *grid = Gtk::manage(new Gtk::Grid);

	errorWindow->set_title ("Error");
	errorWindow->set_border_width(10);
	errorWindow->set_default_size(200, 100);
	errorWindow->set_position(Gtk::WIN_POS_CENTER);

	Gtk::Label *message = new Gtk::Label();
	Gtk::Button *ok_btn = new Gtk::Button("Ok");

	message->set_markup(error);
	message->set_hexpand(true);
	ok_btn->set_hexpand(true);

	errorWindow->add(*grid);
	grid->attach(*message, 0, 0, 3, 1);
	grid->attach(*ok_btn, 0, 1, 1, 1);

	ok_btn->signal_clicked().connect
		(sigc::mem_fun(*this, &MainWindow::error_window_btn) );

	errorWindow->show_all();
}

void MainWindow::error_window_btn(void) {
	errorWindow->hide();
}
