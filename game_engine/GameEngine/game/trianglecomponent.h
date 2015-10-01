#ifndef TRIANGLECOMPONENT_H
#define TRIANGLECOMPONENT_H

#include "engine/components/component.h"

class TriangleComponent : public Component
{
public:
    TriangleComponent();
    static Pool<TriangleComponent *> *pool;
    const std::string componentName() const override;
    void release() override;

    static const std::string name;
private:
};

#endif // TRIANGLECOMPONENT_H
