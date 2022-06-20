#include <QtQml/QQmlProperty>

#include "DownloaderHeader/downloadcontroller.h"

downloadcontroller::downloadcontroller(QObject *parent) : QObject(parent)
{
    QObject *button = parent->findChild<QObject *>("button");
    if (button)
    {
        QObject::connect(button, SIGNAL(clicked()), this, SLOT(on_downloadBtn_clicked()));
    }

    mManager = new DownloadManager(this);
    connect(mManager, &DownloadManager::addLine, this, &downloadcontroller::addLine);
    connect(mManager, &DownloadManager::downloadComplete, this, &downloadcontroller::finished);
    connect(mManager, &DownloadManager::progress, this, &downloadcontroller::progress);
}

void downloadcontroller::downloadBtn_clicked()
{
    QObject *textfield = this->parent()->findChild<QObject *>("textfield");
    QUrl url(QQmlProperty::read(textfield, "text").toString());
    mManager->download(url);
}

void downloadcontroller::pauseBtn_clicked()
{
    mManager->pause();
}

void downloadcontroller::resumeBtn_clicked()
{
    mManager->resume();
}

void downloadcontroller::addLine(QString qsLine)
{

}

void downloadcontroller::progress(int nPercentage)
{

}

void downloadcontroller::finished()
{

}
