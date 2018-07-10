#ifndef CALLER_HPP
#define CALLER_HPP

#include <QString>
#include <QVector>
#include <QObject>

namespace Map2Check  {

/** Map2Check verification modes */
enum class Map2CheckMode {
  MEMTRACK_MODE, /**< Check memory errors (memtrack, deref, free) */
  REACHABILITY_MODE, /**< Check if a target function can be executed */
  OVERFLOW_MODE /**< Check for signed integer overflows */
};

class Caller: public QObject {
    Q_OBJECT
private:    
    bool isRunning = false;
    /** Get optmization flags for original C file
     *  @return Flags for clang */
    static QString getPreOptimizationFlags();
    /** Get optmization flags for final bytecode
     *  @return Flags for opt */
    static QString getPostOptimizationFlags();
    /** Iterate over clang compilation messages (if any)
     *  and check for erors */
    void processClangOutput();

    /** @brief Function to compile original C file removing external memory operations calls
     *  @param cprogam_path Path for the file */
    void compileCFile(QString program);
    /** Link functions called after executing the passes */
    void linkLLVM();
    void executeAnalysis();
    /** Remove generated files for verification */
    void removeTemporaryFiles();
    void callPass();
public:
    Caller() {}
    void stop();
signals:
    /* 0 - Started...
     * 0.2 - Compilation
     * 0.4 - Instrumentation
     * 0.5 - Linking
     * 0.7 - Running
     * 0.9 - Checking results
     * 1 - Exit */
    void instrumentationUpdate(float status);
    void errorOnCompilation();
    void errorOnInstrumentation();
    void errorOnAnalysis();
    void finished();

public slots:
    //void instrumentProgram(QString program);
    //void runAnalysis(Map2CheckMode mode);
};
} // namespace Map2Check

#endif
