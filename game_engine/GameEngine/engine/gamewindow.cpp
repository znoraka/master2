#include "gamewindow.h"

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
using namespace std;


GameWindow::GameWindow()
{
    camera = new Camera();
}

void GameWindow::initialize()
{

    timer.setInterval(framerate * 1000);
    this->camera->initialize(devicePixelRatio(), width(), height(), -100.0, 100.0);
    timer.start();
    this->connect(&timer, SIGNAL(timeout()), this, SLOT(renderNow()));

}

void GameWindow::render()
{
//    this->elapsed = timer.elapsed();

//    if(this->elapsed > this->framerate) {
//        this->render(this->elapsed);
//        this->elapsed = 0;
//        timer.start();
//    }
    this->render(0);
}

void GameWindow::render(float delta)
{
    glClear(GL_COLOR_BUFFER_BIT);
    this->camera->update(delta);
//    this->camera->rotate(1, 0, 0);

    ++m_frame;
}

bool GameWindow::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::UpdateRequest:

        renderNow();
        return true;
    default:
        return QWindow::event(event);
    }
}

void GameWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Escape:
        qApp->exit();
        break;
    case 'Z':
        camera->scale(0.10f, 0.10f, 0);
        break;
    case 'S':
        camera->scale(-0.10f, -0.10f, 0);
        break;
    case 'A':
        camera->rotate(1.0f, 0, 0);
        break;
    case 'E':
        camera->rotate(-1.0f, 0, 0);
        break;
    case 'Q':
        camera->rotate(0, 1.0f, 0);
        break;
    case 'D':
        camera->rotate(0, -1.0f, 0);
        break;
   }
    renderNow();
}
