#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include "openglwindow.h"
#include "QTimer"
#include "camera.h"

class GameWindow : public OpenGLWindow
{
public:
    GameWindow();

    void initialize();
    void render();
    void render(float delta);
    bool event(QEvent *event);

    void keyPressEvent(QKeyEvent *event);

    void displayTriangles();
    void displayLines();
    void displayTrianglesC();
    void displayPoints();
    void displayTrianglesTexture();

    void displayColor(float);

    void loadMap(QString localPath);

private:

    int m_frame;
    QImage m_image;
    Camera *camera;


    int carte=1;

    int etat = 0;
    float rotX = -45.0;
    float rotY = -45.0;
    float ss = 1.0f;
    float framerate = 1.0f / 60.0f;
    float deltaTime = 0;
    float elapsed = 0;
    float lastUpdate = 0;
    QTimer timer;
};


#endif // GAMEWINDOW_H
