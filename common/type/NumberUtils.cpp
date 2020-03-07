#include "NumberUtils.h"
#include <math.h>
NumberUtils::NumberUtils(/* args */)
{
}

NumberUtils::~NumberUtils()
{
}

bool NumberUtils::float_equal(float a, float b)
{
    if (fabs(a - b) < 1e-6)
        return true;
    else
        return false;
}
