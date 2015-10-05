#ifndef POSITIONCOMPONENT_H
#define POSITIONCOMPONENT_H

#include "component.h"

class PositionComponent : public Component
{
public:
    PositionComponent();
    static Pool<PositionComponent *> *pool;
    const QString componentName() const;
    const QString systemName() const;
    void release() override;

    PositionComponent *init(float x, float y, float z);

    float getX() const;
    float getY() const;
    float getZ() const;

    static const QString name;

private:
    float x, y, z;
};

#endif // POSITIONCOMPONENT_H
