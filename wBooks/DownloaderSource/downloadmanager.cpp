#include "DownloaderHeader/downloadmanager.h"

#include <QFileInfo>
#include <QDateTime>
#include <QDebug>

DownloadManager::DownloadManager(QObject *parent) : QObject(parent),
                                                    _pManager(NULL),
                                                    _pCurrentReply(NULL),
                                                    _pFile(NULL),
                                                    _nDownloadTotal(0),
                                                    _bAcceptRanges(false),
                                                    _nDownloadSize(0),
                                                    _nDownloadSizeAtPause(0)
{
}

DownloadManager::~DownloadManager()
{
    if (_pCurrentReply != NULL)
    {
        pause();
    }
}

void DownloadManager::download(QUrl url)
{
    qDebug() << "download: URL=" << url.toString();

    _URL = url;
    _qsFileName = "test";
    _nDownloadSize = 0;
    _nDownloadSizeAtPause = 0;

    _pManager = new QNetworkAccessManager(this);
    _CurrentRequest = QNetworkRequest(url);

    _pCurrentReply = _pManager->head(_CurrentRequest);

    _Timer.setInterval(5000);
    _Timer.setSingleShot(true);
    connect(&_Timer, &QTimer::timeout, this, &DownloadManager::timeout);
    _Timer.start();

    connect(_pCurrentReply, &QNetworkReply::finished, this, &DownloadManager::finished);
    connect(_pCurrentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
}

void DownloadManager::pause()
{
    qDebug() << "pause() = " << _nDownloadSize;
    if (_pCurrentReply == NULL)
    {
        return;
    }
    _Timer.stop();
    disconnect(&_Timer, &QTimer::timeout, this, &DownloadManager::timeout);
    disconnect(_pCurrentReply, &QNetworkReply::finished, this, &DownloadManager::finished);
    disconnect(_pCurrentReply, &QNetworkReply::downloadProgress, this, &DownloadManager::downloadProgress);
    disconnect(_pCurrentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));

    _pCurrentReply->abort();
    //    _pFile->write( _pCurrentReply->readAll());
    _pFile->flush();
    _pCurrentReply = 0;
    _nDownloadSizeAtPause = _nDownloadSize;
    _nDownloadSize = 0;
}

void DownloadManager::resume()
{
    qDebug() << "resume() = " << _nDownloadSizeAtPause;

    download();
}

void DownloadManager::download()
{
    qDebug() << "download()";

    if (_bAcceptRanges)
    {
        QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(_nDownloadSizeAtPause) + "-";
        if (_nDownloadTotal > 0)
        {
            rangeHeaderValue += QByteArray::number(_nDownloadTotal);
        }
        _CurrentRequest.setRawHeader("Range", rangeHeaderValue);
    }

    _pCurrentReply = _pManager->get(_CurrentRequest);

    _Timer.setInterval(5000);
    _Timer.setSingleShot(true);
    connect(&_Timer, &QTimer::timeout, this, &DownloadManager::timeout);
    _Timer.start();

    connect(_pCurrentReply, &QNetworkReply::finished, this, &DownloadManager::finished);
    connect(_pCurrentReply, &QNetworkReply::downloadProgress, this, &DownloadManager::downloadProgress);
    connect(_pCurrentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
}

void DownloadManager::finishedHead()
{
    _Timer.stop();
    _bAcceptRanges = false;

    QList<QByteArray> list = _pCurrentReply->rawHeaderList();
    foreach (QByteArray header, list)
    {
        QString qsLine = QString(header) + " = " + _pCurrentReply->rawHeader(header);
        addLine(qsLine);
    }

    if (_pCurrentReply->hasRawHeader("Accept-Ranges"))
    {
        QString qstrAcceptRanges = _pCurrentReply->rawHeader("Accept-Ranges");
        _bAcceptRanges = (qstrAcceptRanges.compare("bytes", Qt::CaseInsensitive) == 0);
        qDebug() << "Accept-Ranges = " << qstrAcceptRanges << _bAcceptRanges;
    }

    _nDownloadTotal = _pCurrentReply->header(QNetworkRequest::ContentLengthHeader).toInt();

    //    _CurrentRequest = QNetworkRequest(url);
    _CurrentRequest.setRawHeader("Connection", "Keep-Alive");
    _CurrentRequest.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
    _pFile = new QFile(_qsFileName + ".part");
    if (!_bAcceptRanges)
    {
        _pFile->remove();
    }
    _pFile->open(QIODevice::ReadWrite | QIODevice::Append);

    _nDownloadSizeAtPause = _pFile->size();
    download();
}

void DownloadManager::finished()
{
    qDebug() << __FUNCTION__;

    _Timer.stop();
    _pFile->close();
    QFile::remove(_qsFileName);
    _pFile->rename(_qsFileName + ".part", _qsFileName);
    _pFile = NULL;
    _pCurrentReply = 0;
    emit downloadComplete();
}

void DownloadManager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    _Timer.stop();
    _nDownloadSize = _nDownloadSizeAtPause + bytesReceived;
    qDebug() << "Download Progress: Received=" << _nDownloadSize << ": Total=" << _nDownloadSizeAtPause + bytesTotal;

    _pFile->write(_pCurrentReply->readAll());
    int nPercentage = static_cast<int>((static_cast<float>(_nDownloadSizeAtPause + bytesReceived) * 100.0) / static_cast<float>(_nDownloadSizeAtPause + bytesTotal));
    qDebug() << nPercentage;
    emit progress(nPercentage);

    _Timer.start(5000);
}

void DownloadManager::error(QNetworkReply::NetworkError code)
{
    qDebug() << __FUNCTION__ << "(" << code << ")";
}

void DownloadManager::timeout()
{
    qDebug() << __FUNCTION__;
}
