#include "entity.h"

Pool<Entity *> *Entity::pool = new Pool<Entity*>([] () {return new Entity();});

Entity::Entity()
{

}

Entity *Entity::addComponent(Component *component)
{
    removeComponent(component);
    component->setEntity(this);
    components[component->componentName()] = component;
    return this;
}

void Entity::removeComponent(Component *component)
{
    Component* c = components[component->componentName()];
    if(c != nullptr) {
        c->release();
    }
}

Component *Entity::getComponent(const std::string componentId)
{
    return components[componentId];
}

void Entity::release()
{
    std::for_each(components.begin(), components.end(), [] (std::pair<const std::string, Component*> p) {
        p.second->release();
    });
    pool->release(this);
}
