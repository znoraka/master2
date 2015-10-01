#include "pool.h"

template<typename T>
Pool<T>::Pool(std::function<T ()> f)
{
    this->f = f;
}

template<typename T>
T Pool<T>::obtain()
{
    if(objects.size() == 0) {
        qDebug() << "created an object of type" << typeid(T).name();
        objects.push(f());
    } else {
        qDebug() << "obtained an object fo type" << typeid(T).name();
    }
    return objects.pop();
}

template<typename T>
void Pool<T>::release(T object)
{
    if(!objects.contains(object)) {
        qDebug() << "released object of type" << typeid(T).name();
        objects.push(object);
    }
}


