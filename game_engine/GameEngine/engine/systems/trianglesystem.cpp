#include "trianglesystem.h"

const QString TriangleSystem::name = "trianglesystem";

TriangleSystem::TriangleSystem()
{
}

void TriangleSystem::update(float delta)
{
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glColor3f(1, 0, 0);

    glBegin(GL_TRIANGLES);

    foreach (Component *c, components) {
        Entity *entity = c->getEntity();
        PositionComponent *pc = static_cast<PositionComponent*> (entity->getComponent(PositionComponent::name));
        TriangleComponent *tc = static_cast<TriangleComponent*> (entity->getComponent(TriangleComponent::name));
        float x = pc->getX();
        float y = pc->getY();
        glVertex3f(x, y, .0f);
        glVertex3f(x + tc->getWidth(), y, .0f);
        glVertex3f(x + tc->getWidth() * 0.5f, y + tc->getHeight(), .0f);
    }

    glEnd();
}

const QString TriangleSystem::systemName() const
{
    return TriangleSystem::name;
}
