#include "mangler.hpp"

#include <qmath.h>


void Mangler::setBitPattern( quint16 index,
qint8 b1, qint8 b2, qint8 b3, qint8 b4, qint8 b5, qint8 b6, qint8 b7, qint8 b8 )
{
    qint8* ptr = &lut[ index ];
    *ptr++ = b1; *ptr++ = b2; *ptr++ = b3; *ptr++ = b4;
    *ptr++ = b5; *ptr++ = b6; *ptr++ = b6; *ptr++ = b8;
}

inline float fOR(float lhs, float rhs)
{
    return (float)((int) lhs | (int)rhs);
}

inline float fAND(float lhs, float rhs)
{
    return (float)((int)lhs & (int)rhs);
}

void Mangler::initialize(qint64)
{    
    // initialize bit patterns

}
