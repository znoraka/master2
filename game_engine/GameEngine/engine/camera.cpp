#include "camera.h"

Camera::Camera()
{
}

void Camera::initialize(qreal ratio, qreal width, qreal height, qreal near, qreal far)
{
    glViewport(0, 0, width * ratio, height * ratio);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, near, far);
}

void Camera::rotate(float x, float y, float z)
{
    this->rotX += x;
    this->rotY += y;
}

void Camera::scale(float scaleX, float scaleY, float scaleZ)
{
    this->ss += scaleX;
}

void Camera::update(float delta)
{
    glLoadIdentity();
    glScalef(ss,ss,ss);
    glRotatef(rotX,1.0f,0.0f,0.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    if(this->animated) {
        this->rotate(0, 1, 0);
    }
}

void Camera::setAnimated(bool b)
{
    this->animated = b;
}

float Camera::getRotationX() const
{
    return this->rotX;
}

float Camera::getRotationY() const
{
    return this->rotY;
}

float Camera::getRotationZ() const
{
    return 0;
}

float Camera::getScaleX() const
{
    return this->ss;
}

float Camera::getScaleY() const
{
    return this->ss;
}

bool Camera::isAnimated() const
{
    return this->animated;
}



