#ifndef ENTITY_H
#define ENTITY_H

#include <QHash>
#include <QString>

#include "engine/components/component.h"

class Entity
{
    friend class Pool<Entity *>;

public:
    Entity *addComponent(Component *component);
    void removeComponent(const QString componentName);
    Component *getComponent(const QString componentId);
    static Pool<Entity *> *pool;
    void release();

protected:
    Entity();

private:
    QHash<QString, Component*> components;
};

#endif // ENTITY_H
