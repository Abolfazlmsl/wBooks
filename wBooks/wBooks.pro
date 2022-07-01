QT += core quick widgets svg xml

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RESOURCES += qml.qrc

include(vendor/vendor.pri)


# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

SOURCES += \
        $$files(DownloaderSource/*.cpp)\
        $$files(PdfHandlerSource/*.cpp)\
        epubcontainer.cpp \
        epubdocument.cpp \
        main.cpp \
        tree_item.cpp \
        tree_model.cpp \
        widget.cpp

HEADERS += \
    $$files(DownloaderHeader/*.h)\
    $$files(PdfHandlerHeader/*.h)\
    epubcontainer.h \
    epubdocument.h \
    tree_item.h \
    tree_model.h \
    widget.h

#unix: LIBS += -lpoppler-qt5
win32: LIBS += "G:/QtTest/wBooks/wbooks/build-wBooks-Desktop_Qt_5_15_2_MinGW_64_bit-Debug/debug/libpoppler-qt5-1.dll"
