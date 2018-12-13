#include "qml_plugin.hpp"

#include <source/hlpf/filter.hpp>
#include <source/mangler/mangler.hpp>
#include <source/sharpen/sharpen.hpp>

#include <QQmlEngine>
#include <qqml.h>

void qml_plugin::registerTypes(const char *uri)
{
    Q_UNUSED    ( uri );

    qmlRegisterType<Sharpen, 1> ( "WPN114.Audio.JSEffects", 1, 0, "Sharpen" );
    qmlRegisterType<Mangler, 1> ( "WPN114.Audio.JSEffects", 1, 0, "Mangler" );
    qmlRegisterType<Filter, 1>  ( "WPN114.Audio.JSEffects", 1, 0, "HLPFilter" );
}
