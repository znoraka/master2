#ifndef SCENE_H
#define SCENE_H

#include <QSet>
#include "camera.h"
#include "entity.h"
#include "components/positioncomponent.h"
#include "game/trianglecomponent.h"

class Scene
{
public:
    Scene();
    void addEntity(Entity *entity);
    void removeEntity(Entity *entity);

    void update(float delta);

protected:
    Camera *camera;

private:
    QSet<Entity*> entities;
};

#endif // SCENE_H
