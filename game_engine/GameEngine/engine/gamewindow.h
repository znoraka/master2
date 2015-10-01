#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include "openglwindow.h"
#include "QTimer"
#include "game.h"

class GameWindow : public OpenGLWindow
{
public:
    GameWindow();

    void initialize();
    void render();
    void render(float delta);
    bool event(QEvent *event);

    void keyPressEvent(QKeyEvent *event);

private:

    int m_frame;
    QImage m_image;
    Game *game;
    float framerate = 1.0f / 60.0f;

    QTimer timer;
};


#endif // GAMEWINDOW_H
