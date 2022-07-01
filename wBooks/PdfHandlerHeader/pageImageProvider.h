#ifndef PAGEIMAGEPROVIDER_H
#define PAGEIMAGEPROVIDER_H

#include <QDebug>
#include <QPaintDevice>
#include <QQuickImageProvider>
#include <poppler/qt5/poppler-qt5.h>

class PageImageProvider : public QQuickImageProvider {
public:
  PageImageProvider(Poppler::Document *pdfDocument);
  QImage requestImage(const QString &id, QSize *size,
                      const QSize &requestedSize);

private:
  Poppler::Document *document;
};

#endif // PAGEIMAGEPROVIDER_H
