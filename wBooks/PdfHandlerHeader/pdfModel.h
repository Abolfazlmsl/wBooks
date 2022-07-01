#ifndef PDFMODEL_H
#define PDFMODEL_H

#include <QQuickItem>
#include <poppler/qt5/poppler-qt5.h>
#include <QDebug>
#include <QQmlContext>
#include <QQmlEngine>

#define DEBUG                                                                  \
  if (qgetenv("POPPLERPLUGIN_DEBUG") == "1")                                   \
  qDebug() << "Poppler plugin:"

class PdfModel : public QQuickItem {
  Q_OBJECT
  Q_DISABLE_COPY(PdfModel)

public:
  explicit PdfModel(QQuickItem *parent = nullptr);
  virtual ~PdfModel();

  Q_PROPERTY(QString path READ getPath WRITE setPath NOTIFY pathChanged)
  Q_PROPERTY(int numPages READ getNumPages NOTIFY numPagesChanged)
  Q_PROPERTY(bool loaded READ getLoaded NOTIFY loadedChanged)

  void setPath(QString &pathName);
  QString getPath() const { return path; }
  int getNumPages();
  bool getLoaded() const;

  QString path;
  int numPages;
  bool loaded;

private:
  int loadDocument(QString &pathNAme);
  int loadProvider();

  Poppler::Document *document;

signals:
  void pathChanged(const QString &newPath);
  void numPagesChanged(int numPages);
  void loadedChanged();
  void error(const QString &errorMessage);
};

QML_DECLARE_TYPE(PdfModel)

#endif // PDFMODEL_H
