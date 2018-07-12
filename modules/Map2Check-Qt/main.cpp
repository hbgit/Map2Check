#include <QCoreApplication>
#include <QTimer>
#include <mainclass.hpp>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("Map2Check");
    QCoreApplication::setApplicationVersion("Fuzzy-7.0a");

    MainClass mainClass;

    QObject::connect(&mainClass, SIGNAL(finished()), &mainClass, SLOT(aboutToQuitApp()));
    QTimer::singleShot(10, &mainClass, SLOT(run()));
    return a.exec();
}
