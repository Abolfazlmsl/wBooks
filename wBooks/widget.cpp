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
    QFont serifFont(font, fontSize);
    m_document->setDefaultFont(serifFont);
    update();
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

    QFont serifFont(m_font, m_fontSize);
    m_document->clear();
    m_document->clearCache();
    m_document->openDocument(path);
    m_document->setPageSize(size());
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
    //    int offset = m_yOffset + amount;
    //    offset = qMin(int(m_document->size().height() - m_document->pageSize().height()), offset);
    m_yOffset = qMax(0, amount);
    update();
}

void Widget::scrollPage(int amount)
{
    int currentPage = m_yOffset / m_document->pageSize().height();
    currentPage += amount;
    int offset = currentPage * m_document->pageSize().height();
    //    offset = qMin(int(m_document->size().height() - m_document->pageSize().height()), offset);
    m_yOffset = qMax(0, offset);
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
        if (lightMode){
            painter->setPen(Qt::black);
            painter->drawText(boundingRect(), Qt::AlignCenter, "Please choose an .epub file to open");
        }else{
            painter->setPen(Qt::white);
            painter->drawText(boundingRect(), Qt::AlignCenter, "Please choose an .epub file to open");
        }
        return;
    }

    int page_number = qCeil(m_document->docSize().height()/m_document->pageSize().height());
    setPageNumber(page_number);
    setBlockNumber(m_document->blockCount());
    setSliderHeight(m_document->docSize().height());
    setPageHeight(m_document->pageSize().height());

    QAbstractTextDocumentLayout::PaintContext paintContext;

    paintContext.clip = boundingRect();
    paintContext.clip.translate(0, m_yOffset);

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

    m_document->documentLayout()->draw(painter, paintContext);
    update();

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
