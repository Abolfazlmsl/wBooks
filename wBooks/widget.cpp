#include "widget.h"

#include "epubdocument.h"
#include <QScrollArea>

Widget::Widget(QQuickItem *parent)
    : QQuickPaintedItem(parent),
      m_currentChapter(0)
{
    m_document = new EPubDocument(this);
    connect(m_document, &EPubDocument::loadCompleted, this, [&]() {
        update();
    });

    connect(m_document, &EPubDocument::loadContents, this, &Widget::setContents);

}

Widget::~Widget()
{

}

void Widget::setFont(QString font, int fontSize)
{
    m_document->setLoaded(false);
    m_font = font;
    m_fontSize = fontSize;
    QFont serifFont(m_font, m_fontSize);
    m_document->setDefaultFont(serifFont);
    if (m_document->getFiletype() == 1){
        m_document->setPageSize(m_document->pageSize().height()*m_document->docNewPage()/m_document->docPage());
    }
    update();
    if (m_document->getFiletype() == 0){
        m_document->setLoaded(true);
    }
    //    emit m_document->documentLayout()->update(rect);
}

void Widget::changeTheme(bool isLight)
{
    lightMode = isLight;
    update();
}

//bool Widget::loadFile()
//{
//    QSettings settings;
//    QString fileName = QFileDialog::getOpenFileName(this, tr("Open epub"), settings.value("lastFile").toString(), tr("EPUB files (*.epub)"));
//    if (fileName.isEmpty()) {
//        return false;
//    }

//    settings.setValue("lastFile", fileName);

//    return loadFile(fileName);
//}

bool Widget::loadFile(const QString &path)
{
    if (path.isEmpty()) {
        return false;
    }

    if (!QFile::exists(path)) {
        qWarning() << path << "doesn't exist";
        return false;
    }

    m_path = path;

    m_document->setLoaded(false);
    m_document->clear();
    m_document->clearCache();
    currentPage = 1;
    m_yOffset = 0;
    m_document->openDocument(path);
    m_document->setPageSize(size());
    QFont serifFont(m_font, m_fontSize);
    m_document->setDefaultFont(serifFont);

    return true;
}

void Widget::scroll(int amount)
{
    int offset = m_yOffset + amount;
    //    offset = qMin(int(m_document->size().height() - m_document->pageSize().height()), offset);
    m_yOffset = qMax(0, offset);
    update();
}


void Widget::scrollSlider(int amount)
{
    int offset = m_yOffset + amount;
    if (m_document->getFiletype() == 0){
        //    offset = qMin(int(m_document->size().height() - m_document->pageSize().height()), offset);
        if (offset > pageSize){
            if (currentPage==m_document->docPage()){m_yOffset = pageSize;}
            else{nextPage();}
        }else if (offset < 0){
            if (currentPage==1){m_yOffset = 0;}
            else{previousPage();}
        }else{m_yOffset = qMax(0, offset);}
        update();
    }else{
        if (offset > (currentPage-1) * (m_document->pageSize().height())){
            if (currentPage<m_document->docPage()) {nextPage();}
        }else{
            if (currentPage>1) {previousPage();}
        }
    }
}

void Widget::scrollPage(int amount)
{
//    int currentPage = m_yOffset / (m_document->size().height());
//    currentPage += amount;
//    int offset = currentPage * (m_document->size().height());
    //    offset = qMin(int(m_document->size().height() - m_document->pageSize().height()), offset);
//    m_yOffset = qMax(0, offset);
    m_yOffset = (amount-1) * (m_document->pageSize().height());
    update();
}

void Widget::paint(QPainter *painter)
{
    if (lightMode){
        painter->fillRect(boundingRect(), Qt::white);
    }else{
        painter->fillRect(boundingRect(), Qt::black);
    }
    painter->setRenderHint(QPainter::Antialiasing, true);
    if (!m_document->loaded()) {
        //        if (lightMode){
        //            painter->setPen(Qt::black);
        //            painter->drawText(boundingRect(), Qt::AlignCenter, "Please choose an .epub file to open");
        //        }else{
        //            painter->setPen(Qt::white);
        //            painter->drawText(boundingRect(), Qt::AlignCenter, "Please choose an .epub file to open");
        //        }
        return;
    }
    //    int page_number = qCeil(m_document->docSize().height()/m_document->pageSize().height());

    int page_number = m_document->docPage();
//    int page_new_number = m_document->docNewPage();
    pageSize = m_document->documentLayout()->documentSize().height() - m_document->pageSize().height();
    setPageNumber(page_number);
    setBlockNumber(m_document->blockCount());
    setSliderHeight(pageSize);
    setPageHeight(m_document->pageSize().height());

    QAbstractTextDocumentLayout::PaintContext paintContext;

    //    QRectF rect = QRectF(0,m_yOffset,m_document->pageSize().width(),m_yOffset+m_document->pageSize().height());
    rect = QRectF(0,0,m_document->pageSize().width(),m_document->pageSize().height());
    //    paintContext.clip = boundingRect();
    paintContext.clip = rect;
    paintContext.clip.translate(0,m_yOffset);

    for (int group = 0; group < 3; ++group) {
        if (lightMode){
            paintContext.palette.setColor(QPalette::ColorGroup(group), QPalette::WindowText, Qt::black);
            paintContext.palette.setColor(QPalette::ColorGroup(group), QPalette::Light, Qt::black);
            paintContext.palette.setColor(QPalette::ColorGroup(group), QPalette::Text, Qt::black);
            paintContext.palette.setColor(QPalette::ColorGroup(group), QPalette::Base, Qt::black);

            paintContext.palette.setColor(QPalette::ColorGroup(group), QPalette::Background, Qt::white);
            paintContext.palette.setColor(QPalette::ColorGroup(group), QPalette::Window, Qt::white);
            paintContext.palette.setColor(QPalette::ColorGroup(group), QPalette::Button, Qt::white);
        }else{
            paintContext.palette.setColor(QPalette::ColorGroup(group), QPalette::WindowText, Qt::white);
            paintContext.palette.setColor(QPalette::ColorGroup(group), QPalette::Light, Qt::white);
            paintContext.palette.setColor(QPalette::ColorGroup(group), QPalette::Text, Qt::white);
            paintContext.palette.setColor(QPalette::ColorGroup(group), QPalette::Base, Qt::white);

            paintContext.palette.setColor(QPalette::ColorGroup(group), QPalette::Background, Qt::black);
            paintContext.palette.setColor(QPalette::ColorGroup(group), QPalette::Window, Qt::black);
            paintContext.palette.setColor(QPalette::ColorGroup(group), QPalette::Button, Qt::black);
        }
    }

    painter->translate(0, -m_yOffset);
    painter->setClipRect(paintContext.clip);

    //    QTextCursor firstVisible = m_document->cursorForPosition(rect.topLeft());
    //    QTextCursor lastVisible = m_document->cursorForPosition(rect.bottomRight());

    //    m_document->drawContents(painter, rect);

    m_document->documentLayout()->draw(painter, paintContext);

    if (m_document->getFiletype() == 1 && !m_document->getpdfLoaded()){
        m_document->exportPdf();
        m_document->setpdfLoaded(true);
    }
}

//void Widget::keyPressEvent(QKeyEvent *event)
//{
//    if (event->key() == Qt::Key_Up) {
//        scroll(-20);
//    } else if (event->key() == Qt::Key_Down) {
//        scroll(20);
//    } else if (event->key() == Qt::Key_PageUp) {
//        scrollPage(-1);
//    } else if (event->key() == Qt::Key_PageDown) {
//        scrollPage(1);
//    } else if (event->key() == Qt::Key_End) {
//        m_yOffset = m_document->size().height() - m_document->pageSize().height();
//        update();
//    }
//}

//void Widget::wheelEvent(QWheelEvent *event)
//{
//    QPoint numDegrees = event->angleDelta();

//    if (!numDegrees.isNull()) {
//        scroll(-numDegrees.y());
//    }
//}

void Widget::resizeEvent()
{
    m_document->clearCache();
    m_document->setPageSize(size());
    update();
    //    emit m_document->documentLayout()->update(rect);
}

int Widget::findBlockNumber(int index)
{
    QVariant data = m_document->getModelData(index);
    QString text = data.toString();
    qDebug() << text;
    QTextCursor text_cursor = m_document->find(text);
    int block_num = text_cursor.blockNumber();
    return block_num;
}

void Widget::setSetting(QString font, int fontSize, bool mode)
{
    m_font = font;
    m_fontSize = fontSize;
    lightMode = mode;
}

QString Widget::copyBooktoDb(QString path, QString fileName)
{
    QString str = path.replace("\\", "/");
    QDir dir = str;
    if(!dir.exists("Files")){
        dir.mkdir("Files");
    }
    QString cPath = str+"/Files/"+fileName;
    bool state = QFile::copy(m_path, cPath);
    if (state){
        return cPath;
    }
    return "";
}

void Widget::previousPage()
{
//    m_document->clear();
//    m_document->clearCache();
//    currentPage--;
//    m_document->updateDocument(currentPage);
//    QFont serifFont(m_font, m_fontSize);
//    m_document->setDefaultFont(serifFont);
//    update();
//    m_yOffset = m_document->documentLayout()->documentSize().height();
    setCurrentPageNumber(currentPage-1);
}

void Widget::nextPage()
{
//    m_document->clear();
//    m_document->clearCache();
//    currentPage++;
//    m_document->updateDocument(currentPage);
//    QFont serifFont(m_font, m_fontSize);
//    m_document->setDefaultFont(serifFont);
//    update();
//    m_yOffset = 0;
    setCurrentPageNumber(currentPage+1);
}

void Widget::specificPage(int index)
{
    if (m_document->getFiletype() == 0){
        m_document->setLoaded(false);
        m_document->clear();
        m_document->clearCache();
        bool increase = true;
        if (index < currentPage){increase = false;}
        currentPage = index;
        m_document->updateDocument(currentPage);
//        QFont serifFont(m_font, m_fontSize);
//        m_document->setDefaultFont(serifFont);
//        update();
        emit m_document->documentLayout()->pageCountChanged(pageNumber());
        //    m_yOffset = 0;
        if (increase){m_yOffset = 0;}
        else{m_yOffset = m_document->documentLayout()->documentSize().height() - m_document->pageSize().height();}
    }else{
        if (m_document->loaded()){
            currentPage = index;
            scrollPage(currentPage);
        }
    }
}
