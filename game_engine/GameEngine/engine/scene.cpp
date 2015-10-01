#include "scene.h"

Scene::Scene()
{
    camera = new Camera();
    this->addEntity(
                Entity::pool->obtain()->
                addComponent(PositionComponent::pool->obtain()->init(0, 0, 0))->
                addComponent(TriangleComponent::pool->obtain()));
}

void Scene::addEntity(Entity *entity)
{
    this->entities.insert(entity);
}

void Scene::removeEntity(Entity *entity)
{
    entity->release();
    this->entities.remove(entity);
}

void Scene::update(float delta)
{

}
