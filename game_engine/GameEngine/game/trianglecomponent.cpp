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

const QString TriangleComponent::systemName() const
{
    return TriangleSystem::name;
}

System *TriangleComponent::instantiateSystem()
{
    return new TriangleSystem();
}

void TriangleComponent::release()
{
    TriangleComponent::pool->release(this);
}

TriangleComponent *TriangleComponent::init(float width, float height)
{
    this->width = width;
    this->height = height;
    return this;
}

float TriangleComponent::getWidth() const
{
    return width;
}

float TriangleComponent::getHeight() const
{
    return height;
}
