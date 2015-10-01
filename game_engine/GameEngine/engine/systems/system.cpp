#include "system.h"

const QString System::name = "system";

System::System()
{
}

const QString System::componentName() const
{
    return System::name;
}
