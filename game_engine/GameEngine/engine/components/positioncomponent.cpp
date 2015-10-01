#include "positioncomponent.h"

std::string const PositionComponent::name = "positioncomponent";
Pool<PositionComponent *> *PositionComponent::pool = new Pool<PositionComponent*>([] () {return new PositionComponent();});

PositionComponent::PositionComponent()
{
}

const std::string PositionComponent::componentName() const
{
    return PositionComponent::name;
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