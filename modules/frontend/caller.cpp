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
    static QString output = "compiled";
    return output;
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
    QString clang = "./bin/clang";
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
    QString clangOutput = getHashedName(program);
    output.append("-clangOutput");
    process->setStandardOutputFile(clangOutput
                                   );
    QObject::connect(process,  QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                     [=](int, QProcess::ExitStatus exitStatus)
    {
        if(exitStatus != QProcess::NormalExit)
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
    qDebug() << "Starting instrumentation with opt";
    emit instrumentationUpdate(InstrumentationStatus::CompilationStart);
    // TODO: check other modes
    QString opt = "./bin/opt -load ./lib/libNonDetPass.so -map2check";


    QProcess *process = new QProcess(this);
    process->setReadChannelMode(QProcess::SeparateChannels);

    QString compiled = getHashedName(program);
    compiled.append(".bc");
    process->setStandardInputFile(compiled);

    QString output = getHashedName(program);
    output.append("-inst.bc");
    process->setStandardOutputFile(output);

    QObject::connect(process,  QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                     [=](int, QProcess::ExitStatus exitStatus)
    {
        qDebug() << "Finished opt";
        // TODO: link program
        finished();
    });



    process->start(opt);

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

