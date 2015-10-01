#ifndef ENTITY_H
#define ENTITY_H

#include <map>
#include <string>

#include "engine/components/component.h"

class Entity
{
    friend class Pool<Entity *>;

public:
    Entity *addComponent(Component *component);
    void removeComponent(Component *component);
    Component *getComponent(const std::string componentId);
    static Pool<Entity *> *pool;
    void release();

protected:
    Entity();

private:
    std::map<const std::string, Component*> components;
};

#endif // ENTITY_H
