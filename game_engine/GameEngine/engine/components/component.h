#ifndef COMPONENT_H
#define COMPONENT_H

#include <QString>
#include "../tools/pool.cpp"

class Entity;

class Component
{
    friend class Entity;
    friend class Pool<Component *>;
public:
    virtual const QString componentName() const;
    virtual ~Component();
    static Pool<Component *> *pool;

    Entity *getEntity() const;

    virtual void release();
    virtual void init();

    static const QString name;

protected:
    Component();

private:
    void setEntity(Entity *entity);
    Entity *entity;
};

#endif // COMPONENT_H
