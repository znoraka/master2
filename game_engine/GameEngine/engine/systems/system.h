#ifndef SYSTEM_H
#define SYSTEM_H

#include <QString>

class System
{
public:
    System();
    void update();
    static const QString name;
    virtual const QString componentName() const;
};

#endif // SYSTEM_H
