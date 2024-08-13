#pragma once

#include <QObject>

class Presenter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList sources READ sources CONSTANT)

public:
    QStringList sources() const;
};