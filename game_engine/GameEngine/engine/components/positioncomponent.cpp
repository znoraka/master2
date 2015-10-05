#include "positioncomponent.h"

QString const PositionComponent::name = "positioncomponent";
Pool<PositionComponent *> *PositionComponent::pool = new Pool<PositionComponent*>([] () {return new PositionComponent();});

PositionComponent::PositionComponent()
{
}

const QString PositionComponent::componentName() const
{
    return PositionComponent::name;
}

const QString PositionComponent::systemName() const
{
    return System::name;
}

void PositionComponent::release()
{
    PositionComponent::pool->release(this);
}

PositionComponent *PositionComponent::init(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
    return this;
}

float PositionComponent::getX() const
{
    return x;
}

float PositionComponent::getY() const
{
    return y;
}

float PositionComponent::getZ() const
{
    return z;
}
