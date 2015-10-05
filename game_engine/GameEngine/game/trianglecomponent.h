#ifndef TRIANGLECOMPONENT_H
#define TRIANGLECOMPONENT_H

#include "engine/components/component.h"
#include "engine/systems/trianglesystem.h"

class TriangleComponent : public Component
{
public:
    TriangleComponent();
    static Pool<TriangleComponent *> *pool;
    const QString componentName() const override;
    const QString systemName() const override;
    virtual System *instantiateSystem();

    void release() override;

    virtual TriangleComponent *init(float width, float height);

    float getWidth() const;
    float getHeight() const;

    static const QString name;
private:
    float width, height;
};

#endif // TRIANGLECOMPONENT_H
