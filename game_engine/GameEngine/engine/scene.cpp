#include "scene.h"

Scene::Scene()
{
    camera = new Camera();
    Entity *e = Entity::pool->obtain();
    e->
            addComponent(PositionComponent::pool->obtain()->init(0.2f, 0.2f, 0))->
            addComponent(TriangleComponent::pool->obtain()->init(0.4f, 0.4f));
    this->addEntity(e);

    e = Entity::pool->obtain();
        e->
                addComponent(PositionComponent::pool->obtain()->init(0.8f, 0.2f, 0))->
                addComponent(TriangleComponent::pool->obtain()->init(0.4f, 0.4f));
        this->addEntity(e);

    this->ready = true;
}

void Scene::addEntity(Entity *entity)
{
    foreach (Component *component, entity->getComponents()) {
        if(!this->containsSystem(component->systemName())) {
            qDebug() << "inserting system" << component->systemName();
            this->addSystem(component->instantiateSystem());
        }
        System *system = systemsHash.find(component->systemName()).value();
        system->addComponent(component);
    }
}

void Scene::removeEntity(Entity *entity)
{
    entity->release();
    this->entities.remove(entity);
}

bool Scene::isReady() const
{
    return this->ready;
}

void Scene::update(float delta)
{
    foreach (System *system, systemsHash) {
        system->update(delta);
    }
}

bool Scene::containsSystem(const QString s) const
{
    return systemsHash.find(s) != systemsHash.end();
}

void Scene::addSystem(System *system)
{
    systemsHash.insert(system->systemName(), system);
}
