#include "mainwindow.hpp"

#include <QApplication>
#include <QCommandLineParser>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator myTranslator;
    myTranslator.load(QString("translations/goldminer_") + QLocale::system().name());
    a.installTranslator(&myTranslator);

    QTranslator qtTranslator;
    qtTranslator.load(QString("translations/qt_") + QLocale::system().name());
    a.installTranslator(&qtTranslator);

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
