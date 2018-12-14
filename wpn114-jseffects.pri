WPN114_AUDIO_REPOSITORY = ../WPN114-audio
INCLUDEPATH += $$WPN114_AUDIO_REPOSITORY
LIBS += -L$$[QT_INSTALL_QML]/WPN114/Audio -lWPN114-audio

HEADERS += $$PWD/source/hlpf/filter.hpp
HEADERS += $$PWD/source/mangler/mangler.hpp
HEADERS += $$PWD/source/sharpen/sharpen.hpp
SOURCES += $$PWD/source/hlpf/filter.cpp
SOURCES += $$PWD/source/mangler/mangler.cpp
SOURCES += $$PWD/source/sharpen/sharpen.cpp

SOURCES += $$PWD/qml_plugin.cpp
HEADERS += $$PWD/qml_plugin.hpp
