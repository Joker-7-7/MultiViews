#pragma once

#include "QQuickVtkItem.h"

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCapsuleSource.h>
#include <vtkConeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSphereSource.h>
#include <vtkInteractorStyleTrackball.h>

struct MyVtkItem : QQuickVtkItem
{
    Q_OBJECT
        Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)

signals:
    void sourceChanged(QString);

    void clicked();
public:
    MyVtkItem();

    struct Data : vtkObject
    {
        static Data* New();
        vtkTypeMacro(Data, vtkObject);

        vtkNew<vtkActor> actor;
        vtkNew<vtkRenderer> renderer;
        vtkNew<vtkConeSource> cone;
        vtkNew<vtkSphereSource> sphere;
        vtkNew<vtkCapsuleSource> capsule;
        vtkNew<vtkPolyDataMapper> mapper;
        vtkNew<vtkInteractorStyleTrackballCamera> style;
    };

    vtkUserData initializeVTK(vtkRenderWindow* renderWindow) override;
    void destroyingVTK(vtkRenderWindow* renderWindow, vtkUserData userData);

    vtkNew<vtkCamera> _camera;

    void resetCamera();

    QString source() const;

    void setSource(QString v, bool forceVtk = false);
    QString _source;

    bool event(QEvent* ev) override;
    QScopedPointer<QMouseEvent> _click;
};