#ifndef SYSTEM_H
#define SYSTEM_H

#include <QString>
#include <QSet>

class Component;

#include <engine/components/component.h>

class System
{
public:
    System();
    virtual void update(float delta);
    void addComponent(Component *entity);
    void removeComponent(Component *entity);

    static const QString name;
    virtual const QString systemName() const;

protected:
    QSet<Component*> components;

private:

};

#endif // SYSTEM_H
