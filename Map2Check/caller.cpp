#include "caller.hpp"

#include <QDebug>
#include <QSysInfo>
#include <QStringList>
#include <QProcess>

namespace {
inline QString getOSSuffix()
{
    return QSysInfo::kernelType() == "winnt" ? ".exe" : "";
}
}

namespace Map2Check {

QString Caller::getPostOptimizationFlags()
{
    return "-O3";
}

QString Caller::getPreOptimizationFlags()
{
    return "-O0";
}

void Caller::compileCFile(QString program)
{
    qDebug() << "Started compilation";
    emit instrumentationUpdate(0);
    QString program = "clang";
    program << getOSSuffix();

    QStringList arguments;
    arguments << "-Wno-everything";
    arguments << "-Winteger-overflow";
    arguments << "-c";
    arguments << "-emit-llvm";
    arguments << "-g";
    arguments << Caller::getPreOptimizationFlags();
    arguments << "-o compiled.bc";
    arguments << program;

    QProcess *process = new QProcess(this);
    process->start(program, arguments);
    //QObject::connect(process, SIGNAL(finished()), &*this, SLOT(aboutToQuitApp()));
}
} // namespace Map2Check

