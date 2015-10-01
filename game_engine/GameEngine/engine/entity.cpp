#include "entity.h"

Pool<Entity *> *Entity::pool = new Pool<Entity*>([] () {return new Entity();});

Entity::Entity()
{

}

Entity *Entity::addComponent(Component *component)
{
    removeComponent(component->componentName());
    component->setEntity(this);
    components[component->componentName()] = component;
    return this;
}

void Entity::removeComponent(const QString componentName)
{
    QHash<QString, Component*>::iterator i = components.find(componentName);
    if(i != components.end()) {
        i.value()->release();
    }
}

Component *Entity::getComponent(const QString componentId)
{
    return components[componentId];
}

void Entity::release()
{
    foreach (Component *var, components) {
        var->release();
    }
    pool->release(this);
}
