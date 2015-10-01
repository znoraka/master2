#ifndef CAMERA_H
#define CAMERA_H

#include <QtGui/QWindow>
#include <QtGui/QOpenGLFunctions>

class Camera
{
public:
    Camera();


    virtual void initialize(qreal ratio, qreal width, qreal height, qreal near, qreal far);
    void rotate(float x, float y, float z);
    void scale(float scaleX, float scaleY, float scaleZ);
    void update(float delta);
    void setAnimated(bool b);

    float getRotationX() const;
    float getRotationY() const;
    float getRotationZ() const;

    float getScaleX() const;
    float getScaleY() const;
    float geScaleZ() const;

    bool isAnimated() const;

private:
    int etat = 0;
    float rotX = -45.0;
    float rotY = -45.0;
    float ss = 1.0f;
    bool animated = false;
};

#endif // CAMERA_H
