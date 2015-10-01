#ifndef POOLABLE_H
#define POOLABLE_H

#include <QtGlobal>
#include <QStack>
#include <QDebug>
#include <functional>
#include <typeinfo>

template<typename T> class Pool
{
public:
    Pool(std::function<T ()> f);
    T obtain();
    void release(T object);

protected:

private:
    QStack<T> objects;
    std::function<T ()> f;
};

#endif // POOLABLE_H
