#include "caller.hpp"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QStringList>
#include <QSysInfo>
#include <QtGlobal>

namespace {
inline QString getOSSuffix() {
  return QSysInfo::kernelType() == "winnt" ? ".exe" : "";
}

inline QString getHashedName(QString filename) {
  static QString output = "compiled";
  return output;
}

/* On DEBUG mode the workspace MUST be the release directory since it contains
 * all external files, on release mode, it should be the main binary file, so it
 * can be called from anywhere */
inline QString getApplicationPath() {
  QString path;
#ifdef QT_DEBUG
  QDir dir(".");
  path = dir.absolutePath();
#else
  path = QCoreApplication::applicationDirPath();
#endif
  return path;
}

inline QString getExternalToolsPathVar() {
  QString path;
  path.append(getApplicationPath());
  path.append("/bin:$PATH ");
  return path;
}
}  // namespace

namespace Map2Check {

inline QString Caller::getPostOptimizationFlags() { return "-O3"; }

inline QString Caller::getPreOptimizationFlags() { return "-O0"; }

void Caller::compileCFile() {
  QString filename = program;
  QString output = getHashedName(program);
  qDebug() << "Started compilation";

  emit instrumentationUpdate(InstrumentationStatus::CompilationStart);
  QString clang = getApplicationPath().append("/bin/clang");
  clang.append(getOSSuffix());
  qDebug() << clang;
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
  clangOutput.append("-clangOutput");
  process->setStandardOutputFile(clangOutput);
  QObject::connect(
      process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
      [=](int, QProcess::ExitStatus exitStatus) {
        if (exitStatus != QProcess::NormalExit) {
          emit error("Error executing clang");
          emit finished();
        } else {
          qDebug() << "Messages from compilation";
          QString CompilationErrors(process->readAllStandardError());
          if (!CompilationErrors.isEmpty()) {
            emit error(CompilationErrors.toStdString().c_str());
            emit finished();
          }
          instrumentPass();
        }
      });
  process->start(clang, arguments);
}

void Caller::instrumentPass() {
  qDebug() << "Starting instrumentation with opt";
  emit instrumentationUpdate(InstrumentationStatus::InstrumentationStart);
  // TODO: check other modes
  QString opt = getApplicationPath().append(
      "/bin/opt -load ./lib/libNonDetPass.so -non_det -load "
      "./lib/libMap2CheckLibrary.so -map2check ");

  QProcess *process = new QProcess(this);
  process->setReadChannelMode(QProcess::SeparateChannels);

  QString compiled = getHashedName(program);
  compiled.append(".bc");
  process->setStandardInputFile(compiled);

  QString output = getHashedName(program);
  output.append("-inst.bc");
  process->setStandardOutputFile(output);

  QObject::connect(
      process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
      [=](int, QProcess::ExitStatus exitStatus) { linkLLVM(); });

  process->start(opt);
}

void Caller::linkLLVM() {
  qDebug() << "Starting linking";
  emit instrumentationUpdate(InstrumentationStatus::LinkStart);
  // TODO: check other modes
  QString llvm_link = getApplicationPath().append("/bin/llvm-link");

  QStringList arguments;

  QString entry_file = getHashedName(program);
  entry_file.append("-inst.bc");
  arguments << entry_file;

  arguments << "./lib/Map2CheckFunctions.bc";
  arguments << "./lib/AllocationLog.bc";
  arguments << "./lib/HeapLog.bc";
  arguments << "./lib/TrackBBLog.bc";
  arguments << "./lib/Container.bc";
  arguments << "./lib/KleeLog.bc";
  arguments << "./lib/ListLog.bc";
  arguments << "./lib/PropertyGenerator.bc";

  QProcess *process = new QProcess(this);
  process->setReadChannelMode(QProcess::SeparateChannels);

  QString output = getHashedName(program);
  output.append("-linked.bc");
  process->setStandardOutputFile(output);

  QObject::connect(
      process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
      [=](int, QProcess::ExitStatus exitStatus) { instrumentAFL(); });

  process->start(llvm_link, arguments);
}

void Caller::analyzeProgram(QString program, Map2CheckMode mode,
                            QString targetFunction) {
  this->program = program;
  this->mode = mode;
  this->targetFunction = targetFunction;
  emit instrumentationUpdate(InstrumentationStatus::Started);
  // Should execute after cleaning the file with regex
  compileCFile();
}

void Caller::prematureStop() { removeTemporaryFiles(); }

void Caller::removeTemporaryFiles() {}

void Caller::instrumentAFL() {
  qputenv("PATH", getExternalToolsPathVar().toUtf8());

  qDebug() << "Starting AFL Instrumentation";
  emit instrumentationUpdate(InstrumentationStatus::AFL_Instrumentation);
  // TODO: check other modes
  QString afl;
  afl.append(getApplicationPath().append("/afl/afl-clang "));

  QString entry_file = getHashedName(program);
  entry_file.append("-linked.bc ");

  afl.append(entry_file);

  afl.append("-o ");
  QString output = getHashedName(program);
  output.append(".out");
  afl.append(output);

  QProcess *process = new QProcess(this);
  QObject::connect(
      process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
      [=](int, QProcess::ExitStatus exitStatus) { stop(); });

  process->start(afl);
}

void Caller::stop() { emit finished(); }
}  // namespace Map2Check
