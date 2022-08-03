#ifndef EPUBDOCUMENT_H
#define EPUBDOCUMENT_H

#include "epubcontainer.h"
#include <QObject>
#include <QTextDocument>
#include <QImage>
#include <QWidget>

#include "epubcontainer.h"
#include <QIODevice>
#include <QDebug>
#include <QDir>
#include <QTextCursor>
#include <QDomDocument>
#include <QSvgRenderer>
#include <QPainter>
#include <QTextBlock>
#include <QRegularExpression>
#include <QFontDatabase>
#include <QTextDocumentFragment>
#include <QImageReader>
#include <QAbstractTextDocumentLayout>
#include <QList>
#include <QAbstractItemModel>
#include <QElapsedTimer>
#include <qmath.h>
#include <QPrinter>

#include "tree_model.h"

#ifdef DEBUG_CSS
#include <private/qcssparser_p.h>
#endif


class EPubContainer;
class treeModel;

class EPubDocument : public QTextDocument
{
    Q_OBJECT

public:
    explicit EPubDocument(QObject *parent = nullptr);
    virtual ~EPubDocument();

    enum FileType { epub1, epub2 };
    bool loaded() { return m_loaded; }
    void setLoaded(bool isLoad) { m_loaded = isLoad; }

    QSizeF docSize() {return m_docSize;}
    int docNewPage() {return m_newpage;}
    int docPage() {return m_page;}
    FileType getFiletype() {return filetype;}
    bool getpdfLoaded() { return m_pdfLoaded; }
    void setpdfLoaded(bool isload) {m_pdfLoaded = isload;}

    QStringList getItemsPath() {return itemsPath;}
    QStringList getItemsSource() {return itemsSource;}

    void openDocument(const QString &path);
    int itemSpacing = 1;

    void readContents();
    void updateDocument(int page);

    void clearCache() {m_renderedSvgs.clear();}
    void exportOnePagePdf();
    void exportPdf();

    QString getModelSource(QModelIndex index);
    QVariant getModelData(QModelIndex index);
    QList<int> getContentBlocks() {return contentBlocks;}


signals:
    void loadCompleted();
    void loadPdfFile();
    void loadContents(TreeModel *model);

protected:
    virtual QVariant loadResource(int type, const QUrl &url) override;

private slots:
    void loadDocument();

private:
    void fixImages(QDomDocument &newDocument);
    const QImage &getSvgImage(const QString &id);

    QHash<QString, QByteArray> m_svgs;
    QHash<QString, QImage> m_renderedSvgs;
    QString readContentText(QDomNodeList list, int counter);

    QString m_documentPath;
    EPubContainer *m_container;
    EpubItem m_currentItem;
    QList<int> m_loadedFonts;

    QSizeF m_docSize;
    int m_page;
    int m_newpage;
    bool m_loaded;
    QStringList items;
    QStringList itemsPath;
    QStringList itemsSource;
    QList<int> contentBlocks;
    int blockInEachIterate = 0;
    QString cover;
    FileType filetype = epub1;
    bool m_pdfLoaded = false;

    TreeModel *tModel = new TreeModel();
};

#endif // EPUBDOCUMENT_H
