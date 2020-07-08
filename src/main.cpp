#include "mainwindow.hpp"

#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();
    QCommandLineOption debugOption(QStringList() << "debug",
                                   QApplication::translate("main", "Use debug miner mode."));
    parser.addOption(debugOption);
    parser.process(a);

    MainWindow w;
    w.debug_mode = parser.isSet(debugOption);

    w.show();
    return a.exec();
}
