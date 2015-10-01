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
    game = Game::getInstance();
}

void GameWindow::initialize()
{

    timer.setInterval(framerate * 1000);
    timer.start();
    this->connect(&timer, SIGNAL(timeout()), this, SLOT(renderNow()));

}

void GameWindow::render()
{
    this->render(timer.interval());
}

void GameWindow::render(float delta)
{
    glClear(GL_COLOR_BUFFER_BIT);
    game->update(delta);
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
    }
    renderNow();
}
