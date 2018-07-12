#ifndef MAINCLASS_HPP
#define MAINCLASS_HPP

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QObject>
#include <caller.hpp>

class MainClass : public QObject
{
    Q_OBJECT
private:
    QCoreApplication *app;
    QCommandLineParser parser;
    QString inputFile;
    Map2Check::Caller caller;
public:
    explicit MainClass(QObject *parent = 0);
    /////////////////////////////////////////////////////////////
    /// Call this to quit application
    /////////////////////////////////////////////////////////////
    void quit();
signals:
    /////////////////////////////////////////////////////////////
    /// Signal to finish, this is connected to Application Quit
    /////////////////////////////////////////////////////////////
    void finished();

public slots:
    /////////////////////////////////////////////////////////////
    /// This is the slot that gets called from main to start everything
    /// but, everthing is set up in the Constructor
    /////////////////////////////////////////////////////////////
    void run();
    /////////////////////////////////////////////////////////////
    /// slot that get signal when that application is about to quit
    /////////////////////////////////////////////////////////////
    void aboutToQuitApp();

    void error(const char* message);
};

#endif // MAINCLASS_HPP
