import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import Qt.labs.platform 1.1
import Qt.labs.settings 1.1

import com.EpubWidget 1.0

import "./Modules/Panels"
import "./Modules/LocalDatabase"

import "./Fonts/Icon.js" as Icons

Window {
    id: win
    width: 800
    height: 600
    visibility: Qt.WindowFullScreen
    visible: true
    title: qsTr("wBooks")

    property int ratio: 1
    property bool contentEnable: false
    property bool fileUploaded: false
    property int pagesNumber: 1
    property int thisPageNumber: 1
    property int blocksNumber: 1
    property int sliderTotalHeight: 1
    property var contentModel
    property int content_panel_width: 500

    property var localdb: LocalDatabase{
        id: db
        visible: false
    }

    //-- save app setting --//
    Settings{
        id: setting

        fileName: "setting"

        //app properties
        property string cPath: ""
        property string openFileName: ""
        property bool lightMode: true
        property string font: "Times New Roman"
        property int fontCurrentIndex: 0
        property int fontSize: 15
        property real sliderValue: 1
        property int onepageHeight: 1
        property real stepSize: 1
    }


    MouseArea {
        id: mainArea
        anchors.fill: parent
        hoverEnabled: true
        propagateComposedEvents: true
        onClicked: {
            contentpanel.width = 0
            contentEnable = false
        }
    }

    FontLoader{ id: segoeUI; source: "qrc:/Fonts/segoeui.ttf"}                          // segoeUI FONT
    FontLoader{ id: iranSans; source: "qrc:/Fonts/IRANSansMobile.ttf"}                  // iransans FONT
    FontLoader{ id: iranSansFAnum; source: "qrc:/Fonts/IRANSansMobile(FaNum).ttf"}                  // iranSans FARSI number FONT
    FontLoader{ id: iranSansMedium; source: "qrc:/Fonts/IRANSans_Medium.ttf"}           // iransans Medium FONT
    FontLoader{ id: webfont; source: "qrc:/Fonts/materialdesignicons-webfont.ttf"}      //ICONS FONT

    //-- font metric for size porpose --//
    FontMetrics{
        id: fontMetric
        font.family: iranSans.name
        font.pixelSize: Qt.application.font.pixelSize
    }

    //-- Home --//
    Rectangle{
        anchors.fill: parent
        color: (setting.lightMode) ? "steelblue" : "#201918"
        ColumnLayout{
            anchors.fill: parent
            spacing: 0

            RowLayout{
                id: header
                Layout.fillWidth: true
                Layout.preferredHeight: parent.height * 0.10
                Layout.margins: 5
                spacing: 10

                Label{
                    id: contentButton
                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width * 0.05
                    rotation: 180
                    text: Icons.table_of_contents
                    font.family: webfont.name
                    font.pixelSize: Qt.application.font.pixelSize * 3

                    verticalAlignment: Qt.AlignVCenter
                    horizontalAlignment: Qt.AlignHCenter

                    color: (setting.lightMode) ? (contentEnable) ? "white":"black" : (contentEnable) ? "black":"white"
                    MouseArea{
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            contentEnable = !contentEnable
                            //                            contentpanel.visible = contentEnable
                            if (contentEnable){
                                contentpanel.width = content_panel_width*ratio
                            }else{
                                contentpanel.width = 0
                            }
                        }
                    }
                }

                Label{
                    id: downloadButton
                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width * 0.05 / 2
                    text: Icons.download
                    font.family: webfont.name
                    font.pixelSize: Qt.application.font.pixelSize * 3

                    verticalAlignment: Qt.AlignVCenter
                    horizontalAlignment: Qt.AlignHCenter

                    color: (setting.lightMode) ? "black":"white"
                    MouseArea{
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            downloadpanel.visible = true
                        }
                    }
                }

                Item{Layout.preferredWidth: 5}

                Rectangle{
                    Layout.preferredHeight: width
                    Layout.preferredWidth: 30
                    radius: width /2
                    color: "black"

                    MouseArea{
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            setting.lightMode = false
                            epub.changeTheme(setting.lightMode)
                        }
                    }
                }

                Rectangle{
                    Layout.preferredHeight: width
                    Layout.preferredWidth: 30
                    radius: width / 2
                    color: "white"

                    MouseArea{
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            setting.lightMode = true
                            epub.changeTheme(setting.lightMode)
                        }
                    }
                }

                Item{Layout.preferredWidth: 10}


                M_inputText{
                    id: input_search
                    Layout.preferredWidth: parent.width * 0.35
                    Layout.fillHeight: true
                    label: "Search page number"
                    icon: Icons.magnify
                    placeholder: "Search page number"

                    Keys.onEnterPressed: {
                        if (parseInt(input_search.inputText.text) > 0 && parseInt(input_search.inputText.text) <= pagesNumber){
                            epubslider.value = parseInt(input_search.inputText.text) * setting.onepageHeight * setting.stepSize
                        }
                    }
                }

                ComboBox{
                    id: fontButton
                    Layout.fillHeight: true
                    currentIndex: setting.fontCurrentIndex
                    enabled: fileUploaded
                    Layout.preferredWidth: parent.width * 0.13
                    clip: true
                    background: Rectangle {
                        radius: 5
                        color: "transparent"
                        border.color: (setting.lightMode) ? "black" : "white"
                    }

                    contentItem: Label{
                        text: fontButton.editText
                        font.pixelSize: Qt.application.font.pixelSize
                        color: (setting.lightMode) ? "black" : "white"

                        verticalAlignment: Qt.AlignVCenter
                        horizontalAlignment: Qt.AlignHCenter

                    }

                    model: ListModel {
                        id: model
                        ListElement { text: "Times New Roman" }
                        ListElement { text: "Calibri" }
                        ListElement { text: "Arial" }
                        ListElement { text: "Courier"}
                    }
                    onActivated: {
                        setting.font = fontButton.currentText
                        setting.fontCurrentIndex = fontButton.currentIndex
                        epub.setFont(fontButton.currentText, fontSizeButton.value)
                    }
                }

                SpinBox{
                    id: fontSizeButton
                    Layout.fillHeight: true
                    enabled: fileUploaded
                    scale: 1
                    Layout.preferredWidth: parent.width * 0.1
                    Layout.margins: 7
                    from: 1
                    to: 40
                    value: setting.fontSize
                    stepSize: 1


                    background: Rectangle{
                        radius: 5
                        color: (setting.lightMode) ? "transparent" : "#f7f7f7"
                    }

//                    contentItem: Label{
//                        text: fontSizeButton.displayText
//                        font.pixelSize: Qt.application.font.pixelSize * 1
//                        color: "black"
//                        clip: true

//                        verticalAlignment: Qt.AlignVCenter
//                        horizontalAlignment: Qt.AlignHCenter

//                    }

                    onValueModified: {
                        setting.fontSize = fontSizeButton.value
                        epub.setFont(fontButton.currentText, fontSizeButton.value)
                    }
                }

            }

            Row{
                Layout.fillWidth: true
                Layout.preferredHeight: parent.height * 0.75
                Layout.leftMargin: 10
                clip: true
                EpubWidget{
                    id: epub
                    clip: true
                    width: parent.width * 0.95
                    height: parent.height

                    onWidthChanged: {
                        epub.resizeEvent()
                    }

                    onHeightChanged: {
                        epub.resizeEvent()
                    }

                    onContentsChanged: {
                        contentModel = contents
                        epubslider.value = epubslider.stepSize
                        thisPageNumber = 1
                    }

                    onSliderHeightChanged: {
                        epubslider.stepSize = epubslider.to / sliderHeight
                        setting.stepSize = epubslider.stepSize
//                        epubslider.to = sliderHeight
                        sliderTotalHeight = sliderHeight
                    }

                    onPageHeightChanged: {
                        setting.onepageHeight = pageHeight
                    }

                    onPageNumberChanged: {
                        pagesNumber = pageNumber
                    }

                    onBlockNumberChanged: {
                        blocksNumber = blockNumber
                    }

                    MouseArea{
                        id: epubArea
                        anchors.fill: parent
                        propagateComposedEvents: true
                        onWheel: {
                            //                            epub.scroll(-wheel.angleDelta.y)
                            epubslider.value = epubslider.value-wheel.angleDelta.y*setting.stepSize
                        }
                    }

                }
                Slider{
                    id: epubslider
                    enabled: fileUploaded
                    clip: true
                    from: stepSize
                    value: stepSize
                    to: 100
                    width: parent.width * 0.05
                    height: parent.height
                    orientation: Qt.Vertical
                    rotation: 180
                    onValueChanged: {
                        setting.sliderValue = value
                        epub.scrollSlider(value/setting.stepSize)
                        thisPageNumber = Math.ceil(value / setting.onepageHeight/setting.stepSize)
                        update()
                    }
                }

            }

            Item{
                Layout.fillWidth: true
                Layout.preferredHeight: parent.height * 0.05

                RowLayout{
                    anchors.fill: parent
                    spacing: 0

                    Item{
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }

                    Rectangle{
                        Layout.preferredWidth: parent.width * 0.15
                        Layout.fillHeight: true
                        Layout.topMargin: 5
                        border.width: 2
                        border.color: setting.lightMode ? "#888":"black"
                        radius: 10
                        color: setting.lightMode ? "#808080":"black"
                        clip: true
                        Label{
                            anchors.fill: parent
                            text: thisPageNumber + "/" + pagesNumber
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter

                            minimumPointSize: 10
                            fontSizeMode: Text.Fit
                            font.pixelSize: Qt.application.font.pixelSize * 1.5

                            verticalAlignment: Qt.AlignVCenter
                            horizontalAlignment: Qt.AlignHCenter

                            color: setting.lightMode ? "black":"white"
                            clip: true
                            elide: Text.ElideRight
                        }
                    }

                    Item{
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }

            }

            RowLayout{
                id: browesLay
                Layout.fillWidth: true
                Layout.preferredHeight: parent.height * 0.1
                Layout.margins: 5
                spacing: 10
                TextField{
                    id: browseText
                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width * 0.8
                    font.pixelSize: Qt.application.font.pixelSize * 1.2
                    verticalAlignment: Qt.AlignVCenter
                    horizontalAlignment: Qt.AlignHCenter
                    selectByMouse: true
                    readOnly: true
                    text: ""
                    color: setting.lightMode ? "black":"white"
                }

                Button{
                    id: browseButton
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    text: "Browse"
                    highlighted: setting.lightMode ? false:true
                    background: Rectangle {

                        border.width: browseButton.activeFocus ? 2 : 1
                        border.color: setting.lightMode ? "#888":"black"
                        radius: 4
                        color: setting.lightMode ? "#808080":"black"
                    }
                    MouseArea{
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            fileDialog.open()
                        }
                    }
                }
            }
        }

        FilePanel{
            id: contentpanel
            anchors.left: parent.left
            y: header.height + 10
            //            visible: false
            Behavior on width {
                NumberAnimation { duration: 300 }
            }

            clip: true
            width: 0
            height: epub.height

            color: "#444444"
        }
    }


    //-- Folder dialog --//
    FileDialog {
        id: fileDialog
        visible: false
        title: "Please choose a file"
        nameFilters: ["Epub files (*.epub)", "Pdf files (*.pdf)"]

        onAccepted: {
            var path = fileDialog.file.toString()
            path = path.replace(/^(file:\/{3})/,"")
            var fileName = path.slice(path.lastIndexOf("/")+1)
            browseText.text = fileName
            var result = epub.loadFile(path)
            if (result){
                fileUploaded = true
                var cPath = epub.copyBooktoDb(offlineStoragePath, fileName)
                setting.cPath = cPath
                setting.openFileName = fileName
            }
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    DownloadPanel{
        id: downloadpanel
        visible: false
    }

    Component.onCompleted: {
        epub.setSetting(setting.font, setting.fontSize, setting.lightMode)
        if (setting.cPath !== ""){
            var result = epub.loadFile(setting.cPath)
            if (result){
                fileUploaded = true
                browseText.text = setting.openFileName
                epubslider.value = setting.sliderValue
            }
        }

        db.initDatabase();
    }

}
