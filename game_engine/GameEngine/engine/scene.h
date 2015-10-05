#ifndef SCENE_H
#define SCENE_H

#include <QSet>
#include <QList>
#include <QHash>

#include "camera.h"
#include "entity.h"
#include "components/positioncomponent.h"
#include "systems/system.h"
#include "game/trianglecomponent.h"

class Scene
{
public:
    Scene();
    void addEntity(Entity *entity);
    void removeEntity(Entity *entity);
    bool isReady() const;
    void update(float delta);

protected:
    Camera *camera;

private:
    bool containsSystem(const QString s) const;
    void addSystem(System* system);

    QSet<Entity*> entities;
    QList<System*> systems;
    QHash<QString, System*> systemsHash;
    bool ready = false;

};

#endif // SCENE_H
