#ifndef CALLER_HPP
#define CALLER_HPP

#include <QObject>
#include <QProcess>
#include <QString>
#include <QVector>

namespace Map2Check {

/** Map2Check verification modes */
enum class Map2CheckMode {
  MEMTRACK_MODE,     /**< Check memory errors (memtrack, deref, free) */
  REACHABILITY_MODE, /**< Check if a target function can be executed */
  OVERFLOW_MODE      /**< Check for signed integer overflows */
};

enum class InstrumentationStatus {
  Started,
  CompilationStart,
  InstrumentationStart,
  LinkStart,
  AFL_Instrumentation,
  ExecutingAnalysis,
  CheckingResults,
  Finished
};

class Caller : public QObject {
  Q_OBJECT
 private:
  bool isRunning = false;
  QString program;
  Map2CheckMode mode;
  QString targetFunction = 0;
  /** Get optmization flags for original C file
   *  @return Flags for clang */
  static QString getPreOptimizationFlags();
  /** Get optmization flags for final bytecode
   *  @return Flags for opt */
  static QString getPostOptimizationFlags();
  void instrumentPass();

  /** Iterate over clang compilation messages (if any)
   *  and check for erors */
  void processClangOutput();

  /** Link functions called after executing the passes */
  void linkLLVM();
  void executeAnalysis();
  /** Remove generated files for verification */
  void removeTemporaryFiles();
  void callPass();

  void instrumentFuzzer();

 public:
  Caller() {}
  void stop();
 signals:
  void instrumentationUpdate(InstrumentationStatus status);
  void error(const char* message);
  void finished();

 private slots:
  /** @brief Function to compile original C file removing external memory
   * operations calls
   *  @param cprogam_path Path for the file */
  void compileCFile();
  // void
  // void errorOnInstrumentation();
  // void errorOnAnalysis();

 public slots:
  void analyzeProgram(QString program, Map2CheckMode mode,
                      QString targetFunction = 0);
  void prematureStop();
};
}  // namespace Map2Check

#endif
