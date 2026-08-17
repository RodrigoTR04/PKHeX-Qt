#pragma once

#include <QString>

#ifndef PKHEXQT_VERSION
#define PKHEXQT_VERSION "0.1.0"
#endif

#ifndef PKHEXQT_ORACLE_DATE
#define PKHEXQT_ORACLE_DATE "26.07.07"
#endif

inline QString pkhexQtVersion()
{
    return QStringLiteral(PKHEXQT_VERSION);
}

inline QString pkhexOracleDate()
{
    return QStringLiteral(PKHEXQT_ORACLE_DATE);
}
