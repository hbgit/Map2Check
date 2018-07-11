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

    // TODO: Add include file
    QStringList arguments;
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
            /* TODO: begin instrumentation */
            finished();
        }
    });
    process->start(clang, arguments);
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

