TARGET = WPN114-jseffects
TEMPLATE = lib
CONFIG += c++11 dll
QT += quick

localmod: DESTDIR = $$QML_MODULE_DESTDIR/WPN114/Audio/JSEffects
else {
    DESTDIR = $$[QT_INSTALL_QML]/WPN114/Audio/JSEffects
    QML_MODULE_DESTDIR = $$[QT_INSTALL_QML]
}

QMLDIR_FILES += $$PWD/qml/qmldir
QMLDIR_FILES += $$PWD/qml/jseffects.qmltypes
OTHER_FILES = $$QMLDIR_FILES

for(FILE,QMLDIR_FILES) {
    QMAKE_POST_LINK += $$quote(cp $${FILE} $${DESTDIR}$$escape_expand(\n\t))
}

WPN114_AUDIO_REPOSITORY = ../WPN114-audio
INCLUDEPATH += $$WPN114_AUDIO_REPOSITORY
LIBS += -L$$QML_MODULE_DESTDIR/WPN114/Audio -lWPN114-audio
QMAKE_RPATHDIR += $$QML_MODULE_DESTDIR/WPN114/Audio

HEADERS += $$PWD/source/hlpf/filter.hpp
HEADERS += $$PWD/source/mangler/mangler.hpp
HEADERS += $$PWD/source/sharpen/sharpen.hpp
SOURCES += $$PWD/source/hlpf/filter.cpp
SOURCES += $$PWD/source/mangler/mangler.cpp
SOURCES += $$PWD/source/sharpen/sharpen.cpp

SOURCES += $$PWD/qml_plugin.cpp
HEADERS += $$PWD/qml_plugin.hpp
