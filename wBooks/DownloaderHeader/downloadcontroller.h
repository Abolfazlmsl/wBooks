#ifndef DOWNLOADCONTROLLER_H
#define DOWNLOADCONTROLLER_H

#include <QObject>
#include <QDir>
#include <QDesktopServices>
#include "downloadmanager.h"

class downloadcontroller : public QObject
{
    Q_OBJECT
public:
    explicit downloadcontroller(QObject *parent = nullptr);

public slots:
  void addLine(QString qsLine);
  void getTotalSize(int totalValue);
  void received(int receivedValue);
  void progress(int nPercentage);
  void finished();
  void getError();
  void getFileName(QString name);

  void downloadBtn_clicked(QString address, QString path);
  void pauseBtn_clicked();
  void resumeBtn_clicked();

  void openFile();

signals:
  void sendTotalSize(QString size);
  void receivedData(QString data);
  void percentage(int percent);
  void sendError();
  void sendFinish();

private:
  DownloadManager *mManager;
  QString size_human(float num);
  QString fPath;

};

#endif // DOWNLOADCONTROLLER_H
