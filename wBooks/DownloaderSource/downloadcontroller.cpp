#include <QtQml/QQmlProperty>

#include "DownloaderHeader/downloadcontroller.h"

downloadcontroller::downloadcontroller(QObject *parent) : QObject(parent)
{

    mManager = new DownloadManager(this);
    connect(mManager, &DownloadManager::addLine, this, &downloadcontroller::addLine);
    connect(mManager, &DownloadManager::downloadComplete, this, &downloadcontroller::finished);
    connect(mManager, &DownloadManager::totalSize, this, &downloadcontroller::getTotalSize);
    connect(mManager, &DownloadManager::received, this, &downloadcontroller::received);
    connect(mManager, &DownloadManager::progress, this, &downloadcontroller::progress);
    connect(mManager, &DownloadManager::errorHappend, this, &downloadcontroller::getError);
    connect(mManager, &DownloadManager::filename, this, &downloadcontroller::getFileName);
}

void downloadcontroller::downloadBtn_clicked(QString address, QString path)
{
    QString str = path.replace("\\", "/");
    QDir dir = str;
    if(!dir.exists("Files")){
        dir.mkdir("Files");
    }
    QString folderPath = str+"/Files/";
    QUrl url(address);
    mManager->download(url, folderPath);
}

void downloadcontroller::pauseBtn_clicked()
{
    mManager->pause();
}

void downloadcontroller::resumeBtn_clicked()
{
    mManager->resume();
}

void downloadcontroller::openFile()
{
    QDesktopServices::openUrl( QUrl::fromLocalFile(fPath) );
}

void downloadcontroller::addLine(QString qsLine)
{
    qDebug() << "addline: " << qsLine;
}

void downloadcontroller::getTotalSize(int totalValue)
{
    QString size = size_human(totalValue);
    emit sendTotalSize(size);
}

void downloadcontroller::received(int receivedValue)
{
    QString receive_size = size_human(receivedValue);
    emit receivedData(receive_size);
}

void downloadcontroller::progress(int nPercentage)
{
    emit percentage(nPercentage);
}

void downloadcontroller::finished()
{
    emit sendFinish();
}

void downloadcontroller::getError()
{
    emit sendError();
}

void downloadcontroller::getFileName(QString name)
{
    fPath = name;
}

QString downloadcontroller::size_human(float num)
{
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("bytes");

    while(num >= 1024.0 && i.hasNext())
     {
        unit = i.next();
        num /= 1024.0;
    }
    return QString().setNum(num,'f',2)+" "+unit;
}
