#ifndef TRIANGLESYSTEM_H
#define TRIANGLESYSTEM_H

#include "system.h"
#include <QtGui/QOpenGLFunctions>
#include "engine/components/positioncomponent.h"
#include "engine/entity.h"
#include "game/trianglecomponent.h"

class TriangleSystem : public System
{
public:
    TriangleSystem();
    virtual void update(float delta);
    virtual const QString systemName() const;
    static const QString name;

private:

};

#endif // TRIANGLESYSTEM_H
