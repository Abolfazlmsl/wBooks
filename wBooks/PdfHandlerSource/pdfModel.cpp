#include "PdfHandlerHeader/pdfModel.h"
#include "PdfHandlerHeader/pageImageProvider.h"

PdfModel::PdfModel(QQuickItem *parent)
    : QQuickItem(parent), numPages(0), loaded(false), document(nullptr) {}

void PdfModel::setPath(QString &pathName) {
  if (pathName.isEmpty()) {
    return;
  }

  this->path = pathName;

  emit pathChanged(pathName);

  loaded = false;
  emit loadedChanged();

  if (!loadDocument(pathName)) {
    return;
  }

  loadProvider();

  loaded = true;
  emit loadedChanged();
}

int PdfModel::loadDocument(QString &pathName) {
  DEBUG << "Loading document...";

  if (pathName.isEmpty()) {
    qWarning() << "Can't load the document, path is empty.";
    return 0;
  }

  this->document = Poppler::Document::load(pathName);

  if (!document || document->isLocked()) {
    qWarning() << "ERROR : Can't open the document located at " + pathName;
    emit error("Can't open the document located at " + pathName);

    delete document;
    return 0;
  }

  DEBUG << "Document loaded successfully !";

  document->setRenderHint(Poppler::Document::Antialiasing, true);
  document->setRenderHint(Poppler::Document::TextAntialiasing, true);

  if (numPages != document->numPages())
    emit numPagesChanged(document->numPages());

  return 1;
}

int PdfModel::getNumPages() {
  if (!document) {
    return 0;
  }

  numPages = document->numPages();

  return numPages;
}

bool PdfModel::getLoaded() const { return loaded; }

int PdfModel::loadProvider() {
  DEBUG << "Loading image provider...";
  QQmlEngine *engine = QQmlEngine::contextForObject(this)->engine();

  engine->addImageProvider(QLatin1String("poppler"),
                           new PageImageProvider(document));

  DEBUG << "Image provider loaded successfully !";
  return 1;
}

PdfModel::~PdfModel() {
  if (document)
    delete document;
}
