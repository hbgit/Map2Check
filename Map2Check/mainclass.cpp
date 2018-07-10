#include <mainclass.hpp>
#include <QDebug>
#include <QFileInfo>

MainClass::MainClass(QObject *parent) : QObject(parent)
{
    // get instance
    app = QCoreApplication::instance();
    parser.setApplicationDescription("Map2Check Usage");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOptions({
        {{"i", "input-file"},
            QCoreApplication::translate("main", "Analyzes program <input>."),
            QCoreApplication::translate("main", "input")}

    });

    parser.process(*app);
    //Should adapt for QVector in future
    if(!parser.isSet("input-file")) {
       parser.showHelp();
    } else
    {
        inputFile = parser.value("input-file");
    }

    QFileInfo info(inputFile);
    QString extension = info.suffix();

    if(!((extension == "c") || (extension == "i"))) {
        qFatal(QCoreApplication::translate("error", "Unsuported input type."),);
        quit();
    }
}

void MainClass::run()
{
    // MAIN CODE GOES HERE
    qDebug() << "MainClass.run is executing";


    quit();
}

void MainClass::quit()
{
    emit finished();
}

void MainClass::aboutToQuitApp()
{
    //stop threads and remove objects
}
