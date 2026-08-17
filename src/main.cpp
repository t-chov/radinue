#include "ui/MainWindow.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDebug>

#include <clocale>

int main(int argc, char *argv[]) {
    QApplication application(argc, argv);

    // libmpv's client API requires the process-wide numeric locale to remain "C".
    // GUI launches inherit the user's macOS/Windows locale, which can otherwise make
    // mpv_create() fail before a file is loaded.
    if (std::setlocale(LC_NUMERIC, "C") == nullptr) {
        qWarning() << "Could not set the numeric locale required by libmpv";
    }

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
