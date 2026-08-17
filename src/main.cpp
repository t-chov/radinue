#include "ui/MainWindow.h"

#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[]) {
    QApplication application(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("Radinue"));
    QCoreApplication::setApplicationVersion(QStringLiteral("0.1.0"));
    QCoreApplication::setOrganizationName(QStringLiteral("Radinue"));

    radinue::MainWindow mainWindow;
    const QStringList arguments = application.arguments();
    if (arguments.size() == 2) {
        mainWindow.openDirectory(arguments.at(1));
    }
    mainWindow.show();
    return application.exec();
}
