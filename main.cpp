//g++ main.cpp mainwindow.cpp capture.cpp ReadWriteFiles.cpp Jzon.cpp -lpcap -o program `pkg-config --cflags --libs gtkmm-3.0`

#include <gtkmm/application.h>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	Glib::RefPtr<Gtk::Application> app =
	Gtk::Application::create(argc, argv,
	  "main.window.base");

	MainWindow window;
	
	return app->run(window);
}