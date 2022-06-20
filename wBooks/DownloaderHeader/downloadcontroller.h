#ifndef DOWNLOADCONTROLLER_H
#define DOWNLOADCONTROLLER_H

#include <QObject>
#include "downloadmanager.h"

class downloadcontroller : public QObject
{
    Q_OBJECT
public:
    explicit downloadcontroller(QObject *parent = nullptr);

private slots:
  void addLine(QString qsLine);
  void progress(int nPercentage);
  void finished();

  void downloadBtn_clicked();
  void pauseBtn_clicked();
  void resumeBtn_clicked();

private:
  DownloadManager *mManager;

};

#endif // DOWNLOADCONTROLLER_H
