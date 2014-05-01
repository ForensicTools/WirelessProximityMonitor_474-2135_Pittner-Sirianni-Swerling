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


#include "mainwindow.h"
#include <iostream>
#include <iomanip>
#include <gtkmm.h>

MainWindow::MainWindow()
{
	//Set window properties
	set_title ("Wireless Proximity Analyzer");
	set_border_width(10);
	set_default_size(800, 500);
	set_position(Gtk::WIN_POS_CENTER);

	//Initilaize capture bool
	capturing = false;

	//Container for widgets of the window
	Gtk::Box *vbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 0));
	add(*vbox);

	//Menu bar at top of window
	Gtk::MenuBar *menubar = Gtk::manage(new Gtk::MenuBar());
	vbox->pack_start(*menubar, Gtk::PACK_SHRINK, 0);

	/*/ 
	 *	Members of the memu bar
	 *	  File
	 *		Open
	 *		Save
	 *		Exit
	/*/
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

	/*/ 
	 *	Members of menu bar
	 *	  Capture
	 *		Start
	 *		Stop
	/*/
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

	//Organizes widgets
    Gtk::Grid *grid = Gtk::manage(new Gtk::Grid);
    grid->set_border_width(10);
    grid->set_row_spacing(5);
    grid->set_column_spacing(5);
    vbox->add(*grid);

    //Scroll window for list of packets
    scrollWindow = Gtk::manage(new Gtk::ScrolledWindow);
    scrollWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    grid->attach(*scrollWindow, 0, 1, 16, 8);

    //Labels filter entry
    label = Gtk::manage(new Gtk::Label);
    label->set_markup("Display Filter: ");
    grid->attach(*label, 0, 0, 1, 1);

    //User input for display filter
    filter = Gtk::manage(new Gtk::Entry);
    filter->set_hexpand(true);
    grid->attach_next_to(*filter, *label, Gtk::POS_RIGHT, 13, 1);

    //Enables the display filter
    enableFilterBtn = Gtk::manage(new Gtk::Button("Apply"));
    enableFilterBtn->signal_clicked().connect
    	(sigc::mem_fun(*this, &MainWindow::enable_filter));
    grid->attach_next_to(*enableFilterBtn, *filter, Gtk::POS_RIGHT, 1, 1);

    //Clears the display filter
    //Reapplys full capture
    clearFilterBtn = Gtk::manage(new Gtk::Button("Clear"));
    clearFilterBtn->signal_clicked().connect
    	(sigc::mem_fun(*this, &MainWindow::clear_filter));
    grid->attach_next_to(*clearFilterBtn, *enableFilterBtn, Gtk::POS_RIGHT, 1, 1);

    //Add packet list inside scroll window & apply settings
    treeview = Gtk::manage(new Gtk::TreeView);
    scrollWindow->add(*treeview);
    treeview->set_vscroll_policy(Gtk::SCROLL_NATURAL);
    treeview->set_hexpand(true);
    treeview->set_vexpand(true);

    //Create and label columns for packet list
    refTreeModel = Gtk::ListStore::create(columns);
    treeview->set_model(refTreeModel);
    treeview->append_column("Date", columns.col_date);
    treeview->append_column("Time", columns.col_time);
	treeview->append_column("MAC Address", columns.col_mac);
	treeview->append_column("dBm", columns.col_dbm);

	//Display the main window
    show_all();
}

MainWindow::~MainWindow() {
	//Set global variable
	//Threads will end soon after
	capturing = false;
	//Wait for threads to exit
	usleep(2000);

	hide();
}

void MainWindow::captureStopBtn() {
	//Set global variable for caputring
	//Threads will end soon after
	capturing = false;
}

void MainWindow::enable_filter(void) {
	//Lock GUI writing mutex
	pthread_mutex_lock(&mutex);
	//Take in string from entry
	std::string filterStr = filter->get_text();

	//Ensure filter was set
	if( displayFilter.setFilter(filterStr.c_str()) ) {
		//Clear GUI & List of displayed packets
		refTreeModel->clear();
		packetsFiltered.clear();

		//Goes through all packets captured
		for(std::list<packet_structure>::iterator it = packetsCaptured.begin(); 
			it != packetsCaptured.end(); it++) {

			//Checks them against display filter
			if(!displayFilter.filterPacket(*it, packetsFiltered))
			{
				//Push & print
				packetsFiltered.push_front(*it);
				print_packet(*it);
			}
		}
	}
	else //Prints error if filter had issue with entry
		error_window("Incorrect Filter Syntax");

	//Unlock GUI
	pthread_mutex_unlock(&mutex);
}

void MainWindow::clear_filter(void) {
	//Lock GUI mutex
	pthread_mutex_lock(&mutex);

	//Clear GUI & List of displayed packets
	//Null display filter & Entry text
	refTreeModel->clear();
	packetsFiltered.clear();
	displayFilter.clearFilter();
	filter->set_text("");

	//Prints all packets captured
	for(std::list<packet_structure>::iterator it = packetsCaptured.begin(); 
			it != packetsCaptured.end(); it++) {
		print_packet(*it);
	}

	//Unlock GUI
	pthread_mutex_unlock(&mutex);
}

void * MainWindow::printCapture(void)
{
	//Lock GUI
	//Clear GUI and lists
	pthread_mutex_lock(&mutex);
	packetsCaptured.clear();
	packetsFiltered.clear();
	refTreeModel->clear();
	pthread_mutex_unlock(&mutex);

	//Global capturing variable
	capturing = true;

	//Structure for each packet
	struct packet_structure packet;

	//Initiate capture for the device selected
	Capture packet_capture (captureDevice);
	std::string errorStr = packet_capture.captureError();

	//Checks for sucessful capturing
	//Errors most likely due to not selecting WiFi dev
	//Cannot call error window here
	if( errorStr == "") {	//Check global capturing variable
		//Begin itterating through channels 1, 6, 11
		pthread_t chanThread;
		pthread_create(&chanThread, NULL, &MainWindow::iterate_wifi_channels, this);

		while(capturing == true) {
			//Grab next packet
			packet = packet_capture.CapturePacket();

			//Lock GUI
			pthread_mutex_lock(&mutex);

			//Check capture filter
			if(!captureFilter.filterPacket(packet, packetsCaptured)) {
				packetsCaptured.push_back(packet); //Add to list

				//Check display filter
				if(!displayFilter.filterPacket(packet, packetsFiltered))
				{
					packetsFiltered.push_front(packet); //Add to list
					print_packet(packet); //Print to GUI
					usleep(1000 * 200);
				}
			}
			
			//Unlock GUI
			pthread_mutex_unlock(&mutex);
		}
		//Stop channel itterator thread
		pthread_exit(&chanThread);
	}
	
	return 0;
}

void MainWindow::print_packet(struct packet_structure packet) {
	time_t epoch_time = packet.epoch_time; //Holds packet time
	struct tm *ptm = gmtime (&epoch_time); //Convert to time struct
	std::stringstream timeSs; //Holds formatted time
	std::stringstream dateSs; //Holds formatted date
	Gtk::TreeModel::Row row;  //Row entry for GUI

	//0's for non filled fixed entires
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
	//row[columns.col_dbm] = packetsFiltered.size();
	row[columns.col_dbm] = packet.dbm;
	treeview->queue_draw();
}

void MainWindow::on_main_quit_click()
{
	//Set global variable
	//Threads will end soon after
	capturing = false;
	//Wait for threads to exit
	usleep(2000);

    hide();
}

void MainWindow::on_open_click() {
	//Window for opening capture file
	openWindow = new Gtk::Window();
	Gtk::Grid *grid = Gtk::manage(new Gtk::Grid);

	//Window settings
	openWindow->set_title ("Open Capture");
	openWindow->set_border_width(10);
	openWindow->set_default_size(300, 150);
	openWindow->set_position(Gtk::WIN_POS_CENTER);

	//Wigets displayed
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

	//Position widgets
    grid->attach(*fileNameLabel, 0, 0, 1, 1);
    grid->attach(*fileNameEntry, 0, 1, 5, 1);
    grid->attach(*cancel_btn, 1, 2, 1, 1);
    grid->attach(*open_btn, 3, 2, 1, 1);

    //Connect buttons to funcitons
    open_btn->signal_clicked().connect
		(sigc::bind<Gtk::Entry *>
		(sigc::mem_fun(*this, &MainWindow::on_open_click_init), 
		fileNameEntry) );
	cancel_btn->signal_clicked().connect
		(sigc::mem_fun(*this, &MainWindow::on_open_cancel_click));

	//Display created window
	openWindow->show_all();
}

void MainWindow::on_open_click_init(Gtk::Entry *fileNameEntry){
	ReadWrite open;
	std::string fileName = fileNameEntry->get_text();
	pthread_mutex_lock(&mutex);
	refTreeModel->clear();
	packetsCaptured.clear();
	packetsFiltered.clear();
	open.readFromFile(fileName.c_str(), &packetsCaptured);

	for(std::list<packet_structure>::iterator it = packetsCaptured.begin(); it != packetsCaptured.end(); it++)
	{
		print_packet (*it);
	}
	pthread_mutex_unlock(&mutex);
	openWindow->hide();
}

void MainWindow::on_open_cancel_click() {
	openWindow->hide();
}

void MainWindow::on_save_click() {
	//Create window for saving capture
	saveWindow = new Gtk::Window();
	Gtk::Grid *grid = Gtk::manage(new Gtk::Grid);

	//Window settings
	saveWindow->set_title ("Save Capture");
	saveWindow->set_border_width(10);
	saveWindow->set_default_size(300, 150);
	saveWindow->set_position(Gtk::WIN_POS_CENTER);

	//Widgets within window
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

	//Widget positioning
    grid->attach(*fileNameLabel, 0, 0, 1, 1);
    grid->attach(*fileNameEntry, 0, 1, 5, 1);
    grid->attach(*cancel_btn, 1, 2, 1, 1);
    grid->attach(*save_btn, 3, 2, 1, 1);

    //Pair buttons with functions
    save_btn->signal_clicked().connect
		(sigc::bind<Gtk::Entry *>
		(sigc::mem_fun(*this, &MainWindow::on_save_click_init), 
		fileNameEntry) );
	cancel_btn->signal_clicked().connect
		(sigc::mem_fun(*this, &MainWindow::on_save_cancel_click));

	//Dispaly window
	saveWindow->show_all();
}

void MainWindow::on_save_click_init(Gtk::Entry *fileNameEntry){
	//Create saving class
	ReadWrite save;
	//File to be saved to
	std::string fileName = fileNameEntry->get_text();

	save.writeToFile(fileName.c_str(), packetsFiltered);
	saveWindow->hide();
}

void MainWindow::on_save_cancel_click() {
	saveWindow->hide();
}

void MainWindow::capture_window() {
	//Window for setting caputre options
	char errbuff[PCAP_ERRBUF_SIZE]; //Error buffer
	pcap_if_t *d;			//Holds single device
	pcap_if_t *allDevs;		//Holds all devices
	dev_radio_btn.clear();	//Clears widget list of devices

	captureWindow = new Gtk::Window();
	Gtk::Grid *grid = Gtk::manage(new Gtk::Grid);

	//Window settings
	captureWindow->set_title ("Capture Settings");
	captureWindow->set_border_width(10);
	captureWindow->set_default_size(500, 200);
	captureWindow->set_position(Gtk::WIN_POS_CENTER);

	//Widgets within window
	Gtk::Label *capture_filter_label = new Gtk::Label();
	Gtk::Entry *capture_filter_entry = new Gtk::Entry();
	Gtk::Button *ok_btn = new Gtk::Button("Ok");
	Gtk::Button *cancel_btn = new Gtk::Button("Cancel");
	Gtk::RadioButton::Group rbGroup;
	std::vector<Gtk::Label *> dev_desc_label;

	//Checks if devices were found
	if(pcap_findalldevs(&allDevs, errbuff) == -1){
		exit(1);
	}
	
	//Create widgets for all devices
	for(d = allDevs; d != NULL; d = d->next) {
		dev_radio_btn.push_back(new Gtk::RadioButton(rbGroup, d->name));
		if(!d->description)
			dev_desc_label.push_back(new Gtk::Label("No Description"));
		else
			dev_desc_label.push_back(new Gtk::Label(d->description));
	}
	//Release devices found
	pcap_freealldevs(allDevs);

	grid->set_border_width(5);
    grid->set_row_spacing(5);
    grid->set_column_spacing(5);
	captureWindow->add(*grid);

	capture_filter_label->set_markup("Capture Filter: ");
	capture_filter_entry->set_hexpand(true);

	//Position list of devices on the window
	//Lists their desciption too
	int i = 0;
	while(i <= (dev_radio_btn.size() - 1)) {
		grid->attach(*(dev_radio_btn[i]), 1, i, 1, 1);
		grid->attach_next_to(*(dev_desc_label[i]),
							*(dev_radio_btn[i]),
							 Gtk::POS_RIGHT, 2, 1);
		i++;
	}

	//Position remaining widgets
    grid->attach(*capture_filter_label, 0, ++i, 1, 1);
    grid->attach_next_to(*capture_filter_entry, 
    	*capture_filter_label, Gtk::POS_RIGHT, 4, 1);
    grid->attach(*cancel_btn, 2, ++i, 1, 1);
    grid->attach_next_to(*ok_btn, *cancel_btn, Gtk::POS_RIGHT, 1, 1);

    //Connect buttons to buttons
    ok_btn->signal_clicked().connect
    	(sigc::bind<Gtk::Entry *>
		(sigc::mem_fun(*this, &MainWindow::capture_window_ok_btn),
		capture_filter_entry) );

	cancel_btn->signal_clicked().connect
		(sigc::mem_fun(*this, &MainWindow::capture_window_cancel_btn));

	//Display window
	captureWindow->show_all();
}

void MainWindow::capture_window_ok_btn(Gtk::Entry *filterEntry) {
	bool threadStart = false;

	//Gets text from window entry
	std::string filterString = filterEntry->get_text();

	//Checks for the selected device
	for(int i = 0; i <= (dev_radio_btn.size() - 1); i++)
		if(dev_radio_btn[i]->get_active() == true) {
			//Sets variable to selected device
			captureDevice = dev_radio_btn[i]->get_label();

			//Checks filter syntax
			if( !captureFilter.setFilter(filterString.c_str()) ) {
				error_window("Incorrect Filter Syntax");
				break;
			}

			//Begin capturing
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
	//Dynamic window for any errors encountered
	errorWindow = new Gtk::Window();
	Gtk::Grid *grid = Gtk::manage(new Gtk::Grid);

	//Window settings
	errorWindow->set_title ("Error");
	errorWindow->set_border_width(10);
	errorWindow->set_default_size(200, 100);
	errorWindow->set_position(Gtk::WIN_POS_CENTER);

	//Widgets on window
	Gtk::Label *message = new Gtk::Label();
	Gtk::Button *ok_btn = new Gtk::Button("Ok");

	//Widget settings
	message->set_markup(error);
	message->set_hexpand(true);
	ok_btn->set_hexpand(true);

	//Position widgets
	errorWindow->add(*grid);
	grid->attach(*message, 0, 0, 3, 1);
	grid->attach(*ok_btn, 0, 1, 1, 1);

	//Connect button to function
	ok_btn->signal_clicked().connect
		(sigc::mem_fun(*this, &MainWindow::error_window_btn) );

	//Show error window
	errorWindow->show_all();
}

void MainWindow::error_window_btn(void) {
	errorWindow->hide(); //Close error window
}
