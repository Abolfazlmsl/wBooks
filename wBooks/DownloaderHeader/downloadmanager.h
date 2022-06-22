#ifndef DOWNLOADMANAGERHTTP_H
#define DOWNLOADMANAGERHTTP_H

#include <QtGlobal>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QTimer>

class DownloadManager : public QObject
{
  Q_OBJECT

public:
  explicit DownloadManager(QObject *parent = 0);
  virtual ~DownloadManager();

signals:
  void addLine(QString qsLine);
  void downloadComplete();
  void totalSize(int totalValue);
  void received(int receivedValue);
  void progress(int nPercentage);
  void errorHappend();
  void filename(QString name);

public slots:
  void download(QUrl url, QString path);
  void pause();
  void resume();

private slots:
  void download();
  void finishedHead();
  void finished();
  void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
  void error(QNetworkReply::NetworkError code);
  void timeout();

private:
  QUrl _URL;
  QString _qsFileName;
  QNetworkAccessManager *_pManager;
  QNetworkRequest _CurrentRequest;
  QNetworkReply *_pCurrentReply;
  QFile *_pFile;
  int _nDownloadTotal;
  bool _bAcceptRanges;
  int _nDownloadSize;
  int _nDownloadSizeAtPause;
  QTimer _Timer;
};

#endif // DOWNLOADMANAGERHTTP_H
