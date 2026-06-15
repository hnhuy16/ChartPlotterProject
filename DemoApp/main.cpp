#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    // Bỏ qua cách nạp module, trỏ thẳng vào đường dẫn file trong hệ thống tài nguyên (Resource System)
    const QUrl url(QStringLiteral("qrc:/qt/qml/DemoApp/main.qml"));
    engine.load(url);

    return app.exec();
}