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