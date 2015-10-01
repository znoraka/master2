#include "trianglecomponent.h"

QString const TriangleComponent::name = "trianglecomponent";
Pool<TriangleComponent *> *TriangleComponent::pool = new Pool<TriangleComponent*>([] () {return new TriangleComponent();});

TriangleComponent::TriangleComponent()
{
}

const QString TriangleComponent::componentName() const
{
    return TriangleComponent::name;
}

void TriangleComponent::release()
{
    TriangleComponent::pool->release(this);
}
