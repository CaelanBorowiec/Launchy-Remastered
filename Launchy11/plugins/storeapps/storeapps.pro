TEMPLATE = lib
QT += widgets core5compat
CONFIG += plugin \
    release
VPATH += ../../src/
INCLUDEPATH += ../../src/
PRECOMPILED_HEADER = precompiled.h
HEADERS = plugin_interface.h \
    storeapps.h \
    precompiled.h
SOURCES = plugin_interface.cpp \
    storeapps.cpp
TARGET = storeapps
win32 {
    CONFIG -= embed_manifest_dll
	LIBS += shell32.lib ole32.lib uuid.lib user32.lib
	QMAKE_CXXFLAGS_RELEASE += /Zi
	QMAKE_LFLAGS_RELEASE += /DEBUG
}
if(!debug_and_release|build_pass):CONFIG(debug, debug|release):DESTDIR = ../../debug/plugins
if(!debug_and_release|build_pass):CONFIG(release, debug|release):DESTDIR = ../../release/plugins
