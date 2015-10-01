#ifndef POSITIONCOMPONENT_H
#define POSITIONCOMPONENT_H

#include "component.h"

class PositionComponent : public Component
{
public:
    PositionComponent();
    static Pool<PositionComponent *> *pool;
    const std::string componentName() const;
    void release() override;

    PositionComponent *init(float x, float y, float z);

    static const std::string name;
private:

    float x, y, z;
};

#endif // POSITIONCOMPONENT_H
