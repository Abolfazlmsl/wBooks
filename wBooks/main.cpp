#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <widget.h>
#include <DownloaderHeader/downloadcontroller.h>
#include <PdfHandlerHeader/pdfModel.h>

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    qputenv("QT_QUICK_CONTROLS_STYLE", "material");

    QQmlApplicationEngine engine;
    qmlRegisterType<Widget>("com.EpubWidget", 1, 0, "EpubWidget");
    qmlRegisterType<PdfModel>("org.pdfviewer.poppler", 1, 0, "Poppler");

    //-- QSetting configuration --//
    QCoreApplication::setOrganizationName("wBooks");
    QCoreApplication::setOrganizationDomain("");
    QCoreApplication::setApplicationName("wBooks");

    downloadcontroller downloader;
    engine.rootContext()->setContextProperty("downloader", &downloader);

    auto offlineStoragePath = engine.offlineStoragePath();
    engine.rootContext()->setContextProperty("offlineStoragePath", offlineStoragePath);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
