#include "trianglecomponent.h"

std::string const TriangleComponent::name = "trianglecomponent";
Pool<TriangleComponent *> *TriangleComponent::pool = new Pool<TriangleComponent*>([] () {return new TriangleComponent();});

TriangleComponent::TriangleComponent()
{
}

const std::string TriangleComponent::componentName() const
{
    return TriangleComponent::name;
}

void TriangleComponent::release()
{
    TriangleComponent::pool->release(this);
}
