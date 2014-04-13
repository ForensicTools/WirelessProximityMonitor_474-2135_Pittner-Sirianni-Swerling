//g++ main.cpp mainwindow.cpp -o program `pkg-config --cflags --libs gtkmm-3.0`

#include "mainwindow.h"
#include <gtkmm/application.h>

int main(int argc, char *argv[])
{
  Glib::RefPtr<Gtk::Application> app =
    Gtk::Application::create(argc, argv,
      "org.gtkmm.examples.base");

  MainWindow window;

  return app->run(window);
}