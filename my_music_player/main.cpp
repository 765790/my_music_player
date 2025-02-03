#include <qapplication.h>
#include "my_window.h"

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	MyWindow win(QSize(800,600));
	win.setWindowName("Alexander's music player");
	win.show();
	return app.exec();
}