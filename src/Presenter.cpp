#include "Presenter.h"

QStringList Presenter::sources() const
{
    return QStringList{} << "Cone"
        << "Sphere"
        << "Capsule";
}