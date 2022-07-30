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

    bool loaded() { return m_loaded; }
    void setLoaded(bool isLoad) { m_loaded = isLoad; }

    QSizeF docSize() {return m_docSize;}
    int docNewPage() {return m_newpage;}
    int docPage() {return m_page;}

    void openDocument(const QString &path);
    int itemSpacing = 1;

    void readContents();
    void updateDocument(int page);

    void clearCache() {m_renderedSvgs.clear();}

    QVariant getModelData(int index);


signals:
    void loadCompleted();
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
    QString cover;

    TreeModel *tModel_content = new TreeModel();
};

#endif // EPUBDOCUMENT_H
