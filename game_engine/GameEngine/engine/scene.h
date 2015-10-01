#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "camera.h"
#include "entity.h"

class Scene
{
public:
    Scene();
    void addEntity(Entity *entity);
    void removeEntity(Entity *entity);
    void removeEntity(int index);

protected:
    Camera *camera;

private:
    std::vector<Entity*> entities;
};

#endif // SCENE_H
