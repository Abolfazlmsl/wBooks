#include "epubdocument.h"

EPubDocument::EPubDocument(QObject *parent) : QTextDocument(parent),
    m_container(nullptr),
    m_loaded(false)
{
    setUndoRedoEnabled(true);
//    connect(documentLayout(), &QAbstractTextDocumentLayout::documentSizeChanged, this, [=](const QSizeF &newSize) {
//        qDebug() << "doc size changed";
//        qDebug() << "doc size changed" << newSize;
//        qDebug() << "----------------------------";
//        m_docSize = newSize;
//    });

    connect(documentLayout(), &QAbstractTextDocumentLayout::pageCountChanged, this, [=](const int &newPage) {
        m_newpage = newPage;
        m_loaded = true;
        emit loadCompleted();
    });

}

EPubDocument::~EPubDocument()
{
    for (const int fontId : m_loadedFonts) {
        QFontDatabase::removeApplicationFont(fontId);
    }
}

void EPubDocument::openDocument(const QString &path)
{
    m_documentPath = path;
    loadDocument();
}

void EPubDocument::readContents()
{
    TreeModel *tModel = new TreeModel();

    QString cover = m_container->getStandardPage(EpubPageReference::TableOfContents);
    EpubItem contentItem = m_container->getEpubItem(cover);
    if (contentItem.path.isEmpty()){
        cover = "ncx";
        contentItem = m_container->getEpubItem(cover);
    }
    QSharedPointer<QIODevice> contentioDevice = m_container->getIoDevice(contentItem.path);
    QDomDocument document;
    document.setContent(contentioDevice.data(), true);


//    QDomNodeList metadataNodeList = document.elementsByTagName("content");
//    for (int i=0; i<metadataNodeList.count(); i++) {
//        QDomNode itemnode = metadataNodeList.at(i);

//        QDomElement itemElement = itemnode.toElement();
//        EpubItem toc_Item = m_container->getEpubItem(itemElement.attribute("src").split('.')[0]);
//        QSharedPointer<QIODevice> ioDevice = m_container->getIoDevice(toc_Item.path);
//        qDebug() << ioDevice.data()->readAll();

//    }

    QDomNodeList Points1 = document.elementsByTagName("navPoint");
    QDomNodeList Labels1 = document.elementsByTagName("navLabel");
    QDomNodeList content_NodeList1 = document.elementsByTagName("content");

    for (int i=0; i<Labels1.count(); i++) {
        QDomNode nodeLabels1 = Labels1.at(i);
        QDomElement ElementLabels1 = nodeLabels1.toElement();

        auto text_content = new TreeItem(readContentText(content_NodeList1, i));
        tModel_content->addTopLevelItem(text_content);

        auto text_main = new TreeItem(ElementLabels1.text());
        tModel->addTopLevelItem(text_main);

        QDomNode nodePoints1 = Points1.at(i);
        QDomElement ElementPoints1 = nodePoints1.toElement();
        QDomNodeList Labels2 = ElementPoints1.elementsByTagName("navLabel");
        QDomNodeList Points2 = ElementPoints1.elementsByTagName("navPoint");
        QDomNodeList content_NodeList2 = ElementPoints1.elementsByTagName("content");

        for (int j=1; j<Labels2.count();j++){
            QDomNode nodeLabels2 = Labels2.at(j);
            QDomElement ElementLabels2 = nodeLabels2.toElement();

            auto text_content2 = new TreeItem(readContentText(content_NodeList2, j));
            tModel_content->addItem(text_content, text_content2);

            auto text_main2 = new TreeItem(ElementLabels2.text());
            tModel->addItem(text_main, text_main2);

            QDomNode nodePoints2 = Points2.at(j-1);
            QDomElement ElementPoints2 = nodePoints2.toElement();
            QDomNodeList Labels3 = ElementPoints2.elementsByTagName("navLabel");
            QDomNodeList Points3 = ElementPoints2.elementsByTagName("navPoint");
            QDomNodeList content_NodeList3 = ElementPoints2.elementsByTagName("content");


            for (int k=1; k<Labels3.count();k++){
                QDomNode nodeLabels3 = Labels3.at(k);
                QDomElement ElementLabels3 = nodeLabels3.toElement();

                auto text_content3 = new TreeItem(readContentText(content_NodeList3, k));
                tModel_content->addItem(text_content2, text_content3);

                auto text_main3 = new TreeItem(ElementLabels3.text());
                tModel->addItem(text_main2, text_main3);

                QDomNode nodePoints3 = Points3.at(k-1);
                QDomElement ElementPoints3 = nodePoints3.toElement();
                QDomNodeList Labels4 = ElementPoints3.elementsByTagName("navLabel");
                QDomNodeList Points4 = ElementPoints3.elementsByTagName("navPoint");
                QDomNodeList content_NodeList4 = ElementPoints3.elementsByTagName("content");


                for (int f=1; f<Labels4.count();f++){
                    QDomNode nodeLabels4 = Labels4.at(f);
                    QDomElement ElementLabels4 = nodeLabels4.toElement();

                    auto text_content4 = new TreeItem(readContentText(content_NodeList4, f));
                    tModel_content->addItem(text_content3, text_content4);

                    auto text_main4 = new TreeItem(ElementLabels4.text());
                    tModel->addItem(text_main3, text_main4);

                    i++;
                    j++;
                    k++;
                }
                i++;
                j++;
            }
            i++;
        }
    }

    emit loadContents(tModel);
}

QVariant EPubDocument::getModelData(int index)
{
    const QModelIndex &idx = tModel_content->index(index, 0, tModel_content->rootIndex());
    return tModel_content->data(idx);
}

QString EPubDocument::readContentText(QDomNodeList list, int counter)
{
    //*******************************Content-Click*************************//
    QDomNode content_node = list.at(counter);
    QDomElement content_element = content_node.toElement();
    EpubItem toc_Item = m_container->getEpubItem(content_element.attribute("src").split('.')[0]);
    if (!toc_Item.path.isEmpty()){
        QSharedPointer<QIODevice> content_ioDevice = m_container->getIoDevice(toc_Item.path);

        QDomDocument content_itemDoc;
        content_itemDoc.setContent(content_ioDevice.data());
        QDomNodeList content_text = content_itemDoc.elementsByTagName("article");

        QDomNode content_itemnode = content_text.at(0);
        QDomElement content_itemelement = content_itemnode.toElement();


        return content_itemelement.text();
    }else{
        return "";
    }
    //***************************************************************//
}

void EPubDocument::loadDocument()
{
    QElapsedTimer timer;
    timer.start();
    setBaseUrl(QUrl());
    m_container = new EPubContainer(this);

//    connect(m_container, &EPubContainer::errorHappened, this, [](QString error) {
//        qWarning().noquote() << error;
//    });

    if (!m_container->openFile(m_documentPath)) {
        return;
    }

    qDebug() << "Time restarted in " << timer.restart() << "ms";

    items = m_container->getItems();

    cover = m_container->getStandardPage(EpubPageReference::CoverPage);
    if (!cover.isEmpty()) {
        items.prepend(cover);
    }

    m_page = qCeil(items.length() / 10);

    QDomDocument domDoc;
    QTextCursor textCursor(this);
    textCursor.beginEditBlock();
    textCursor.movePosition(QTextCursor::End);

    QTextBlockFormat pageBreak;
    pageBreak.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysBefore);

    int num = 0;
    for (int i=0; i<itemSpacing;i++){
        if (i>=items.length()){break;}
        const QString &chapter = items[i];
        m_currentItem = m_container->getEpubItem(chapter);
        if (m_currentItem.path.isEmpty()) {
            continue;
        }

        QSharedPointer<QIODevice> ioDevice = m_container->getIoDevice(m_currentItem.path);

        domDoc.setContent(ioDevice.data());

        if (!cover.isEmpty() && num==0){
            fixImages(domDoc);
        }
        textCursor.insertFragment(QTextDocumentFragment::fromHtml(domDoc.toString()));
        textCursor.insertBlock(pageBreak);
        num++;
    }

    setBaseUrl(QUrl(m_currentItem.path));
    qDebug() << "Base url:" << baseUrl();

    qDebug() << "Load done in" << timer.restart() << "ms";
    {
        QElapsedTimer timer;
        timer.start();
        QFont f = defaultFont();
        QFontMetrics fm(f);
        int mw =  fm.horizontalAdvance(QLatin1Char('x')) * 80;
        int w = mw;
        setTextWidth(w);
        qDebug() << "Text width set in" << timer.restart() << "ms";
        QSizeF size = m_docSize;
        if (size.width() != 0) {
            w = qSqrt((uint)(5 * size.height() * size.width() / 3));
            setTextWidth(qMin(w, mw));

            size = m_docSize;//documentLayout()->documentSize();
            if (w*3 < 5*size.height()) {
                w = qSqrt((uint)(2 * size.height() * size.width()));
                setTextWidth(qMin(w, mw));
            }
        }
        qDebug() << "Text width changed in" << timer.restart() << "ms";
//        w = idealWidth();
//        qDebug() << "Ideal width in" << timer.restart() << "ms";
//        setTextWidth(w);
//        qDebug() << "Final text width in" << timer.restart() << "ms";
    }
    qDebug() << "Adjust size done in" << timer.elapsed() << "ms";

    textCursor.endEditBlock();
    readContents();
}

void EPubDocument::updateDocument(int page)
{
    QDomDocument domDoc;
    QTextCursor textCursor(this);
    setBaseUrl(QUrl());
    textCursor.beginEditBlock();
    textCursor.movePosition(QTextCursor::End);

    QTextBlockFormat pageBreak;
    pageBreak.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysBefore);

    int num = (page-1);
    for (int i=(page-1)*itemSpacing; i<page*itemSpacing;i++){
        if (i>=items.length()){break;}
        const QString &chapter = items[i];
        m_currentItem = m_container->getEpubItem(chapter);
        if (m_currentItem.path.isEmpty()) {
            continue;
        }

        QSharedPointer<QIODevice> ioDevice = m_container->getIoDevice(m_currentItem.path);

        domDoc.setContent(ioDevice.data());

        if (!cover.isEmpty() && num==0){
            fixImages(domDoc);
        }
        textCursor.insertFragment(QTextDocumentFragment::fromHtml(domDoc.toString()));
        textCursor.insertBlock(pageBreak);
        num++;
    }

    setBaseUrl(QUrl(m_currentItem.path));
//    qDebug() << "Base url:" << baseUrl();
    textCursor.endEditBlock();

    emit loadCompleted();
}

void EPubDocument::fixImages(QDomDocument &newDocument)
{
    // TODO: FIXME: replace this with not smushing all HTML together in one document
//    { // Fix relative URLs, images are lazily loaded so the base URL might not
      // be correct when they are loaded
        QDomNodeList imageNodes = newDocument.elementsByTagName("img");
        for (int i=0; i<imageNodes.count(); i++) {
            QDomElement image = imageNodes.at(i).toElement();
            if (!image.hasAttribute("src")) {
                continue;
            }
            QUrl href = QUrl(image.attribute("src"));
            href = baseUrl().resolved(href);
            image.setAttribute("src", href.toString());
        }
//    }

//    { // QImage which QtSvg uses isn't able to read files from inside the archive, so embed image data inline
        imageNodes = newDocument.elementsByTagName("image"); // SVG images
        for (int i=0; i<imageNodes.count(); i++) {
            QDomElement image = imageNodes.at(i).toElement();
            if (!image.hasAttribute("xlink:href")) {
                continue;
            }
            QString path = image.attribute("xlink:href");
            QByteArray fileData = loadResource(0, QUrl(path)).toByteArray();
            QByteArray data = "data:image/jpeg;base64," +fileData.toBase64();
            image.setAttribute("xlink:href", QString::fromLatin1(data));
        }
//    }

    static int svgCounter = 0;

    // QTextDocument isn't fond of SVGs, so rip them out and store them separately, and give it <img> instead
    QDomNodeList svgNodes = newDocument.elementsByTagName("svg");
    for (int i=0; i<svgNodes.count(); i++) {
        QDomElement svgNode = svgNodes.at(i).toElement();

        // Serialize out the old SVG, store it
        QDomDocument tempDocument;
        tempDocument.appendChild(tempDocument.importNode(svgNode, true));
        QString svgId = QString::number(++svgCounter);
        m_svgs.insert(svgId, tempDocument.toByteArray());

        // Create <img> node pointing to our SVG image
        QDomElement imageElement = newDocument.createElement("img");
        imageElement.setAttribute("src", "svgcache:" + svgId);

        // Replace <svg> node with our <img> node
        QDomNode parent = svgNodes.at(i).parentNode();
        parent.replaceChild(imageElement, svgNode);
    }
}

const QImage &EPubDocument::getSvgImage(const QString &id)
{
    if (m_renderedSvgs.contains(id)) {
        return m_renderedSvgs[id];
    }
    if (!m_svgs.contains(id)) {
        qWarning() << "Couldn't find SVG" << id;
        static QImage nullImg;
        return nullImg;
    }

    QSize imageSize(pageSize().width() - documentMargin() * 4,
                    pageSize().height() - documentMargin() * 4);


    QSvgRenderer renderer(m_svgs.value(id));
    QSize svgSize(renderer.viewBox().size());

    if (svgSize.isValid()) {
        if (svgSize.scaled(imageSize, Qt::KeepAspectRatio).isValid()) {
            svgSize.scale(imageSize, Qt::KeepAspectRatio);
        }
    } else {
        svgSize = imageSize;
    }

    QImage rendered(svgSize, QImage::Format_ARGB32);
    QPainter painter(&rendered);
    if (!painter.isActive()) {
        qWarning() << "Unable to activate painter" << svgSize;
        static const QImage dummy = QImage();
        return dummy;
    }
    renderer.render(&painter);
    painter.end();

    m_renderedSvgs.insert(id, rendered);
    return m_renderedSvgs[id];
}


QVariant EPubDocument::loadResource(int type, const QUrl &url)
{
    Q_UNUSED(type);

    if (url.scheme() == "svgcache") {
        return getSvgImage(url.path());
    }

    if (url.scheme() == "data") {
        QByteArray data = url.path().toUtf8();
        const int start = data.indexOf(';');
        if (start == -1) {
            qWarning() << "unable to decode data:, no ;" << data.left(100);
            data = QByteArray();

            addResource(type, url, data);
            return data;
        }

        data = data.mid(start + 1);
        if (data.startsWith("base64,")) {
            data = QByteArray::fromBase64(data.mid(data.indexOf(',') + 1));
        } else {
            qWarning() << "unable to decode data:, unknown encoding" << data.left(100);
            data = QByteArray();
        }

        addResource(type, url, data);
        return data;
    }


    QSharedPointer<QIODevice> ioDevice = m_container->getIoDevice(url.path());
    if (!ioDevice) {
        qWarning() << "Unable to get io device for" << url.toString().left(100);
        qDebug() << url.scheme();
        return QVariant();
    }
    QByteArray data = ioDevice->readAll();

    if (type == QTextDocument::StyleSheetResource) {
        const QString cssData = QString::fromUtf8(data);

        // Extract embedded fonts
        static const QRegularExpression fontfaceRegex("@font-face\\s*{[^}]+}", QRegularExpression::MultilineOption);
        QRegularExpressionMatchIterator fontfaceIterator = fontfaceRegex.globalMatch(cssData);
        while (fontfaceIterator.hasNext()) {
            QString fontface = fontfaceIterator.next().captured();

            static const QRegularExpression urlExpression("url\\s*\\(([^\\)]+)\\)");
            QString fontPath = urlExpression.match(fontface).captured(1);
            // Resolve relative and whatnot shit
            fontPath = QDir::cleanPath(QFileInfo(baseUrl().path()).path() + '/' + fontPath);

            QSharedPointer<QIODevice> ioDevice = m_container->getIoDevice(fontPath);
            if (ioDevice) {
                m_loadedFonts.append(QFontDatabase::addApplicationFontFromData(ioDevice->readAll()));
                qDebug() << "Loaded font" << QFontDatabase::applicationFontFamilies(m_loadedFonts.last());
            } else {
                qWarning() << "Failed to load font from" << fontPath << baseUrl();
            }
        }

        data = cssData.toUtf8();

    }

    addResource(type, url, data);

    return data;
}

