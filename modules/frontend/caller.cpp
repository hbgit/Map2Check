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

inline QString getHashedName(QString filename) {
  static QString output = "compiled";
  return output;
}

inline QString getApplicationPath() {
  QString path = QCoreApplication::applicationDirPath();
  return path;
}

static inline QString getClang() { return "clang"; }
static inline QString getLllvmLink() { return "llvm-link"; }
static inline QString getOpt() { return "opt"; }

inline void configureEnvironment() {
  QString lib_path = getApplicationPath();
  lib_path.append("/../lib:$LD_LIBRARY_PATH");

  qputenv("LD_LIBRARY_PATH", lib_path.toStdString().c_str());

  QString bin_path = getApplicationPath();
  bin_path.append(":$PATH");

  qputenv("PATH", bin_path.toStdString().c_str());

  qDebug() << "Path: " << qgetenv("PATH");
  qDebug() << "LD: " << qgetenv("LD_LIBRARY_PATH");
}

inline void warnAboutQProccessError(QProcess *p) {
  QObject::connect(
      p,
      static_cast<void (QProcess::*)(QProcess::ProcessError)>(&QProcess::error),
      [=](QProcess::ProcessError pError) { qWarning() << "error " << pError; });
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
  QString clang = getClang();

  // TODO: Add include folder on project
  QStringList arguments;
  // arguments << "-I ../include";
  arguments << "-Wno-everything";
  arguments << "-Winteger-overflow";
  arguments << "-c";
  arguments << "-emit-llvm";
  arguments << "-g";
  arguments << Caller::getPreOptimizationFlags();
  arguments << "-o" << output.append(".bc");
  arguments << filename;

  QProcess *process = new QProcess(this);
  // process->setProcessEnvironment(getEnv());
  QString clangOutput = getHashedName(program);
  clangOutput.append("-clangOutput");
  process->setStandardOutputFile(clangOutput);
  QObject::connect(
      process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
      [=](int, QProcess::ExitStatus) {

        QString CompilationErrors(process->readAllStandardError());
        if (!CompilationErrors.isEmpty()) {
          emit error(CompilationErrors.toStdString().c_str());
          emit finished();
        }
        instrumentPass();

      });
  warnAboutQProccessError(process);
  process->start(clang, arguments);
}

void Caller::instrumentPass() {
  qDebug() << "Starting instrumentation with opt";
  emit instrumentationUpdate(InstrumentationStatus::InstrumentationStart);
  // TODO: check other modes
  QString opt = getOpt();
  opt.append(" ");
  QString nonDetPass = getApplicationPath().append("/../lib/libNonDetPass.so");
  QString memoryTrackPass =
      getApplicationPath().append("/../lib/libMemoryTrackPass.so");
  QString map2checkPass =
      getApplicationPath().append("/../lib/libMap2CheckLibrary.so");

  opt.append("-load=").append(nonDetPass).append(" -non_det ");
  opt.append("-load=").append(memoryTrackPass).append(" -memory_track ");
  opt.append("-load=").append(map2checkPass).append(" -map2check ");

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
      [=](int, QProcess::ExitStatus) { linkLLVM(); });
  warnAboutQProccessError(process);
  process->start(opt);
  qDebug() << opt;
}

void Caller::linkLLVM() {
  qDebug() << "Starting linking";
  emit instrumentationUpdate(InstrumentationStatus::LinkStart);
  // TODO: check other modes
  QString llvm_link = getLllvmLink();
  QStringList arguments;

  QString entry_file = getHashedName(program);
  entry_file.append("-inst.bc");
  arguments << entry_file;

  arguments << getApplicationPath().append("/../lib/Map2CheckFunctions.bc");
  arguments << getApplicationPath().append("/../lib/AllocationLog.bc");
  arguments << getApplicationPath().append("/../lib/HeapLog.bc");
  arguments << getApplicationPath().append("/../lib/TrackBBLog.bc");
  arguments << getApplicationPath().append("/../lib/Container.bc");
  arguments << getApplicationPath().append("/../lib/KleeLog.bc");
  arguments << getApplicationPath().append("/../lib/ListLog.bc");
  arguments << getApplicationPath().append("/../lib/PropertyGenerator.bc");

  QProcess *process = new QProcess(this);
  process->setReadChannelMode(QProcess::SeparateChannels);

  QString output = getHashedName(program);
  output.append("-linked.bc");
  process->setStandardOutputFile(output);

  QObject::connect(
      process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
      [=](int, QProcess::ExitStatus) { instrumentFuzzer(); });

  warnAboutQProccessError(process);
  process->start(llvm_link, arguments);
  qDebug() << arguments;
}

void Caller::executeAnalysis() {}

void Caller::analyzeProgram(QString program, Map2CheckMode mode,
                            QString targetFunction) {
  configureEnvironment();
  this->program = program;
  this->mode = mode;
  this->targetFunction = targetFunction;
  emit instrumentationUpdate(InstrumentationStatus::Started);
  // Should execute after cleaning the file with regex
  compileCFile();
}

void Caller::prematureStop() { removeTemporaryFiles(); }

void Caller::removeTemporaryFiles() {}

void Caller::instrumentFuzzer() {
  qDebug() << "Starting fuzzer instrumentation";
  emit instrumentationUpdate(InstrumentationStatus::AFL_Instrumentation);
  QString clang = getClang();

  QString entry_file = getHashedName(program);
  entry_file.append("-linked.bc");

  QString output_file = "-o ";
  output_file.append(getHashedName(program).append(".out"));
  QStringList arguments;

  arguments << "-g";
  arguments << "-fsanitize=fuzzer";
  // arguments << output_file;
  arguments << entry_file;

  QProcess *process = new QProcess(this);
  process->setReadChannelMode(QProcess::SeparateChannels);
  QString clangOutput = getHashedName(program);
  clangOutput.append("-clangOutput");
  process->setStandardOutputFile(clangOutput);

  QObject::connect(
      process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
      [=](int, QProcess::ExitStatus) { stop(); });

  warnAboutQProccessError(process);
  process->start(clang, arguments);
}

void Caller::stop() { emit finished(); }
}  // namespace Map2Check
