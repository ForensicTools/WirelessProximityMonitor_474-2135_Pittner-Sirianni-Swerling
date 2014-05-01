all:
	g++ main.cpp mainwindow.cpp capture.cpp ReadWriteFiles.cpp Jzon.cpp filter.cpp -lpcap -o program `pkg-config --cflags --libs gtkmm-3.0`
