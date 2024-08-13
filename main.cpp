#include "src/Presenter.h"
#include "src/MyVtkItem.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickVTKRenderWindow.h>
#include <QQuickWindow>

#include <QVTKRenderWindowAdapter.h>

#include <QQmlContext>
#include <QObject>

extern "C" {
    _declspec(dllexport) int NvOptimusEnablement = 1;
    _declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main(int argc, char* argv[])
{
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGLRhi);
    QSurfaceFormat::setDefaultFormat(QVTKRenderWindowAdapter::defaultFormat());

    QGuiApplication app(argc, argv);
    Presenter presenter;

    qmlRegisterType<MyVtkItem>("com.vtk.example", 1, 0, "MyVtkItem");
    qmlRegisterUncreatableType<Presenter>("com.vtk.example", 1, 0, "Presenter", "!!");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("presenter", &presenter);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
