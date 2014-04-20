//g++ main.cpp mainwindow.cpp -o program `pkg-config --cflags --libs gtkmm-3.0`

#include <gtkmm/application.h>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	pthread_t chanThread;			//Thread for rotating WiFi channels

	Glib::RefPtr<Gtk::Application> app =
	Gtk::Application::create(argc, argv,
	  "org.gtkmm.examples.base");

	MainWindow window;

	return app->run(window);
}