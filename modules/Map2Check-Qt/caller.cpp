#include "caller.hpp"

#include <QDebug>
#include <QSysInfo>
#include <QStringList>
#include <QProcess>
#include <QFile>

namespace {
inline QString getOSSuffix()
{
    return QSysInfo::kernelType() == "winnt" ? ".exe" : "";
}

inline QString getHashedName(QString filename)
{
    return "compiled";
}
}

namespace Map2Check {

inline QString Caller::getPostOptimizationFlags()
{
    return "-O3";
}

inline QString Caller::getPreOptimizationFlags()
{
    return "-O0";
}


void Caller::compileCFile()
{
    QString filename = program;
    QString output = getHashedName(program);
    qDebug() << "Started compilation";

    emit instrumentationUpdate(InstrumentationStatus::CompilationStart);
    QString clang = "clang";
    clang.append(getOSSuffix());

    // TODO: Add include folder on project
    QStringList arguments;
    arguments << "-I include";
    arguments << "-Wno-everything";
    arguments << "-Winteger-overflow";
    arguments << "-c";
    arguments << "-emit-llvm";
    arguments << "-g";
    arguments << Caller::getPreOptimizationFlags();
    arguments << "-o" << output.append(".bc");
    arguments << filename;

    QProcess *process = new QProcess(this);
    QObject::connect(process,  QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                     [=](int, QProcess::ExitStatus exitStatus)
    {
        if(exitStatus == QProcess::CrashExit)
        {
            emit error("Error executing clang");
            finished();
        }
        else
        {
            qDebug() << "Messages from compilation";
            QString CompilationErrors(process->readAllStandardError());
            if(!CompilationErrors.isEmpty())
            {
                emit error(CompilationErrors.toStdString().c_str());
                finished();
            }
            instrumentPass();
        }
    });
    process->start(clang, arguments);
}

void Caller::instrumentPass()
{
    // TODO: check other modes
     QString opt = "opt";
     QStringList arguments;
     // Should fix for all OS (.so is for *nix)
     arguments << "-load ${MAP2CHECK_PATH}/lib/libNonDetPass.so";

     QProcess *process = new QProcess(this);
     QObject::connect(process,  QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                      [=](int, QProcess::ExitStatus exitStatus)
     {
         // TODO: implement
         qDebug() << "opt executed";
         finished();
     });
     QFile file(output);
     if (!file.open(QIODevice::ReadOnly))
     {
         // Error opening file
         emit error("Could not open compiled .bc file");
         finished();
     }

     while(!file.atEnd())
     {

     }

}

void Caller::analyzeProgram(QString program, Map2CheckMode mode, QString targetFunction)
{
    this->program = program;
    this->mode = mode;
    this->targetFunction = targetFunction;
    emit instrumentationUpdate(InstrumentationStatus::Started);
    // Should execute after cleaning the file with regex
    compileCFile();
}

void Caller::prematureStop()
{
    removeTemporaryFiles();
}

void Caller::removeTemporaryFiles()
{

}
} // namespace Map2Check

