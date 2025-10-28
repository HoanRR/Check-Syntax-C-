#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application style
    app.setStyle("Fusion");

    // Set application info
    app.setApplicationName("C Compiler IDE");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Your Organization");

    MainWindow window;
    window.show();

    return app.exec();
}