#include "engine/gamewindow.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>

#include <QtCore/qmath.h>
#include <QMouseEvent>
#include <QKeyEvent>
#include <time.h>
#include <sys/time.h>
#include <iostream>

#include <QtCore>
#include <QtGui>
#include "engine/camera.h"
#include "engine/components/component.h"
#include "engine/components/positioncomponent.h"
#include "engine/entity.h"
#include "game/trianglecomponent.h"

using namespace std;


void createWindow(Camera* camera, float framerate) {

}


int main(int argc, char **argv)
{
    srand(time(NULL));
    QGuiApplication app(argc, argv);

    QSurfaceFormat format;
    format.setSamples(16);

    GameWindow *w = new GameWindow();
    w->setFormat(format);
    w->resize(640, 480);
    w->show();
    w->setAnimating(true);

    Entity *e = Entity::pool->obtain();
    e->
            addComponent(Component::pool->obtain())->
            addComponent(PositionComponent::pool->obtain()->init(0, 0, 0))->
            addComponent(TriangleComponent::pool->obtain())->
            addComponent(PositionComponent::pool->obtain());

    e->getComponent(PositionComponent::name);
    e->release();

    return app.exec();
}



