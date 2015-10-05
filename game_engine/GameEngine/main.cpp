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

    Game::getInstance()->setScene(new Scene());

    return app.exec();
}



