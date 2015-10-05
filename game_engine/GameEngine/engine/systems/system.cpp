#include "system.h"

const QString System::name = "system";

System::System()
{
}

void System::update(float delta)
{

}

void System::addComponent(Component *component)
{
    this->components.insert(component);
}

void System::removeComponent(Component *component)
{
    component->release();
    this->components.remove(component);
}

const QString System::systemName() const
{
    return System::name;
}
