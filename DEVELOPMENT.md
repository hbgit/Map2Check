# Environment

* Ubuntu (or derivatives)
* Qt5 (put in your path)
* build-essential `apt install build-essential`
* cmake `apt install cmake`
* curl `apt install curl`

1. Clone or save this repository
2. Run bootrap.sh script
3. Run make-release.sh script

# Using Qt Creator for development

We recommend the use of Qt Creator since it is a really good C/C++ IDE and it is easy to install

1. Open the main CMakeLists.txt file
2. In the project options, set the workspace to the release directory
3. Build it in debug mode

# Generating Release

## Recommended: use static Qt

1. Download and extract the Qt source code: https://download.qt.io/archive/qt/5.11/5.11.1/single/qt-everywhere-src-5.11.1.tar.xz
2. Configure it to compile statically: `./configure -static -prefix “~/QtStatic” -qt-zlib -qt-pcre -qt-xcb -qt-sql-sqlite -make libs -nomake tools -nomake examples -nomake tests`
3. Execute `make` and `make install`
4. Configure CMake or QtCreator to use it 

## Creating AppImage release

TODO: adapt map2check to work with https://github.com/probonopd/linuxdeployqt
