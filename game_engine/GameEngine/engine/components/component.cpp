#include "component.h"

QString const Component::name = "component";
Pool<Component *> *Component::pool = new Pool<Component*>([] () {return new Component();});

Component::Component()
{
}

const QString Component::componentName() const
{
    return Component::name;
}

Component::~Component()
{

}

void Component::setEntity(Entity *entity)
{
    this->entity = entity;
}

Entity *Component::getEntity() const
{
    return this->entity;
}

void Component::release()
{
    Component::pool->release(this);
}

void Component::init()
{

}
