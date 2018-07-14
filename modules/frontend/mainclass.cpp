#include <QDebug>
#include <QFileInfo>
#include <caller.hpp>
#include <mainclass.hpp>

MainClass::MainClass(QObject* parent) : QObject(parent) {
  QObject::connect(&caller, SIGNAL(error(const char*)), this,
                   SLOT(error(const char*)));
  QObject::connect(&caller, SIGNAL(finished()), this, SLOT(aboutToQuitApp()));
  QObject::connect(this, SIGNAL(finished()), &caller, SLOT(prematureStop()));
  // get instance
  app = QCoreApplication::instance();
  parser.setApplicationDescription("Map2Check Usage");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addOptions(
      {{{"i", "input-file"},
        QCoreApplication::translate("main", "Analyzes program <input>."),
        QCoreApplication::translate("main", "input")}

      });

  parser.process(*app);
  // Should adapt for QVector in future
  if (!parser.isSet("input-file")) {
    parser.showHelp();
  } else {
    inputFile = parser.value("input-file");
  }

  QFileInfo info(inputFile);
  QString extension = info.suffix();

  if (!((extension == "c") || (extension == "i"))) {
    qFatal("unsuported type");
    quit();
  }
}

void MainClass::run() {
  // Analyze program
  Map2Check::Map2CheckMode mode = Map2Check::Map2CheckMode::MEMTRACK_MODE;
  caller.analyzeProgram(inputFile, mode);
}

void MainClass::error(const char* message) {
  qFatal("%s", message);
  quit();
}

void MainClass::quit() { emit finished(); }

void MainClass::aboutToQuitApp() { exit(0); }
