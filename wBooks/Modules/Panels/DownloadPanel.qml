import QtQuick 2.14
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12
import QtGraphicalEffects 1.0

import "./../../Fonts/Icon.js" as Icons

ApplicationWindow{
    id: root_auth

    property bool success: true

    //-- Get the percentage of download from Qt --//
    Connections {
        target: downloader

        function onSendTotalSize(size){
            total_txt.text = size
        }

        function onReceivedData(data){
            progress_txt.text = data
        }

        function onPercentage(value){
            progressbar.value = value
        }

        function onSendFinish(){
            success = true
            popup.state = "downloaded"
            alarmSignupWin.msg = "دانلود با موفقیت به اتمام رسید"
        }

        function onSendError(){
            success = false
            popup.state = "initial"
            alarmSignupWin.msg = "دانلود با خطا مواجه شد"
        }
    }

    //-- when open LoginPage inputs most be Empty --//
    signal resetForm()
    onResetForm: {
        input_License.inputText.text = ""
        input_License.inputText.forceActiveFocus()
    }

    visible: true//false//
    minimumWidth: 700
    maximumWidth: 700
    minimumHeight: 500
    maximumHeight: 500
    title: " "
    objectName: "Auth"
    flags: Qt.Dialog //SplashScreen //Dialog //Widget,SubWindow //Sheet //CoverWindow

    MouseArea {
        //            anchors.fill: parent
        width: parent.width
        height: parent.height

        propagateComposedEvents: true
        property real lastMouseX: 0
        property real lastMouseY: 0
        acceptedButtons: Qt.LeftButton
        onMouseXChanged: root_auth.x += (mouseX - lastMouseX)
        onMouseYChanged: root_auth.y += (mouseY - lastMouseY)
        onPressed: {
            if(mouse.button == Qt.LeftButton){
                parent.forceActiveFocus()
                lastMouseX = mouseX
                lastMouseY = mouseY

                //-- seek clip --//
                //                    player.seek((player.duration*mouseX)/width)
            }
            //                mouse.accepted = false
        }
    }

    Pane {
        id: popup

        state: "initial"

        states: [
            State {
                name: "initial"
                PropertyChanges {
                    target: open_btn
                    enabled: false
                    color: "gray"
                }
                PropertyChanges {
                    target: start_btn
                    enabled: true
                }
                PropertyChanges {
                    target: pause_btn
                    enabled: false
                }
                PropertyChanges {
                    target: resume_btn
                    enabled: false
                }
            },
            State {
                name: "download"
                PropertyChanges {
                    target: open_btn
                    enabled: false
                    color: "gray"
                }
                PropertyChanges {
                    target: start_btn
                    enabled: false
                }
                PropertyChanges {
                    target: pause_btn
                    enabled: true
                }
                PropertyChanges {
                    target: resume_btn
                    enabled: false
                }
            },
            State {
                name: "pause"
                PropertyChanges {
                    target: open_btn
                    enabled: false
                    color: "gray"
                }
                PropertyChanges {
                    target: start_btn
                    enabled: true
                }
                PropertyChanges {
                    target: pause_btn
                    enabled: false
                }
                PropertyChanges {
                    target: resume_btn
                    enabled: true
                }
            },
            State {
                name: "resume"
                PropertyChanges {
                    target: open_btn
                    enabled: false
                    color: "gray"
                }
                PropertyChanges {
                    target: start_btn
                    enabled: false
                }
                PropertyChanges {
                    target: pause_btn
                    enabled: true
                }
                PropertyChanges {
                    target: resume_btn
                    enabled: false
                }
            },
            State {
                name: "downloaded"
                PropertyChanges {
                    target: open_btn
                    enabled: true
                    color: "#50FF4B"
                }
                PropertyChanges {
                    target: start_btn
                    enabled: true
                }
                PropertyChanges {
                    target: pause_btn
                    enabled: false
                }
                PropertyChanges {
                    target: resume_btn
                    enabled: false
                }
            }
        ]

        Rectangle{
            anchors.fill: parent; color: "white"
        }

        anchors.fill: parent
        anchors.margins: -11

        ColumnLayout{
            anchors.fill: parent
            spacing: 0

            //-- logo and intro -//
            Rectangle{
                Layout.fillWidth: true
                Layout.preferredHeight: parent.height * 0.2
                color: (setting.lightMode) ? "#00FF0000" : "#201918"

                Image {
                    id: img_intro
                    source: "./../../Images/logo1.png"
                    width: parent.width
                    height: parent.height
                    fillMode: Image.PreserveAspectFit

                }

            }

            //-- inputs for download -//
            Rectangle{
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: (setting.lightMode) ? "#00FF0000" : "#201918"

                ColumnLayout{
                    anchors.fill: parent
                    //                    //-- footer of left layout--//
                    //                    Rectangle {
                    //                        id: bar
                    //                        Layout.fillWidth: true
                    //                        Layout.preferredHeight: 50

                    //                        radius: 20
                    //                        Layout.leftMargin: 10
                    //                        Layout.rightMargin: 10
                    //                        color: (setting.lightMode) ? "#6c88b7":"black"

                    //                        Label{
                    //                            text: "wBooks downloader"
                    //                            font.pixelSize: Qt.application.font.pixelSize * 1.3
                    //                            color: (setting.lightMode) ? "black":"white"
                    //                            anchors.centerIn: parent
                    //                        }
                    //                    }

                    Rectangle{
                        id: licenseView
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: (setting.lightMode) ? "#00FF0000" : "#201918"
                        //                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        clip: true
                        ColumnLayout{
                            anchors.fill: parent
                            //-- spacer --//
                            Item{Layout.preferredHeight: 20}

                            //-- "Enter the url --//
                            Label{
                                Layout.fillWidth: true
                                Layout.preferredHeight: implicitHeight
                                text: "آدرس دانلود را وارد کنید"
                                color: (setting.lightMode) ? "darkblue" : "white"
                                font.family: iranSans.name
                                font.pixelSize: Qt.application.font.pixelSize * 1.3
                                horizontalAlignment: Qt.AlignHCenter

                            }

                            //-- spacer --//
                            Item{Layout.preferredHeight: 15}

                            //-- url --//
                            M_inputText{
                                id: input_address
                                label: "Download address"
                                icon: Icons.web
                                placeholder: "Download address"
                                Layout.leftMargin: 20
                                Layout.rightMargin: 20
                                border.color: (setting.lightMode) ? "transparent" : "white"

                            }

                            //-- spacer --//
                            Item{Layout.preferredHeight: 30 }

                            ProgressBar{
                                id: progressbar
                                Layout.fillWidth: true
                                implicitHeight: 0
                                visible: false
                                Behavior on implicitHeight {
                                    NumberAnimation { duration: 300 }
                                }
                                Layout.leftMargin: 20
                                Layout.rightMargin: 20
                                from: 0
                                to: 100
                                value: 0
                                background: Rectangle {
                                    implicitWidth: 200
                                    implicitHeight: 6
                                    color: "#e6e6e6"
                                    radius: 10
                                }

                                contentItem: Item {
                                    implicitWidth: 200
                                    implicitHeight: 4

                                    Rectangle {
                                        width: progressbar.visualPosition * parent.width
                                        height: parent.height
                                        radius: 10
                                        color: "#17a81a"
                                    }
                                }

                                Label{
                                    anchors.fill: parent
                                    horizontalAlignment: Qt.AlignHCenter
                                    verticalAlignment: Qt.AlignVCenter
                                    visible: progressbar.visible

                                    text: progressbar.value + " %"
                                    color: "black"
                                    z: 1
                                }
                            }


                            //-- spacer --//
                            Item{Layout.preferredHeight: 10 }

                            Item{
                                Layout.fillWidth: true
                                Layout.preferredHeight: 40
                                RowLayout{
                                    anchors.fill: parent
                                    Item{Layout.fillWidth: true}
                                    Label{
                                        id: progress_txt
                                        visible: progressbar.visible
                                        verticalAlignment: Qt.AlignVCenter
                                        horizontalAlignment: Qt.AlignHCenter
                                        color: (setting.lightMode) ? "black" : "white"
                                        text: ""
                                    }

                                    Label{
                                        visible: progressbar.visible
                                        verticalAlignment: Qt.AlignVCenter
                                        horizontalAlignment: Qt.AlignHCenter
                                        color: (setting.lightMode) ? "black" : "white"
                                        text: "/"
                                    }

                                    Label{
                                        id: total_txt
                                        visible: progressbar.visible
                                        verticalAlignment: Qt.AlignVCenter
                                        horizontalAlignment: Qt.AlignHCenter
                                        color: (setting.lightMode) ? "black" : "white"
                                        text: ""
                                    }
                                    Item{Layout.preferredWidth: parent.width * 0.5}
                                    //-- Button open --//
                                    Rectangle{
                                        id: open_btn
                                        Layout.fillHeight: true
                                        Layout.preferredWidth: parent.width * 0.2
                                        visible: progressbar.visible
                                        radius: width / 2


                                        Label{
                                            anchors.centerIn: parent
                                            text: "باز کردن فایل"
                                            font.family: iranSans.name
                                            font.pixelSize: Qt.application.font.pixelSize * 1.5
                                            color: "#ffffff"
                                        }

                                        MouseArea{
                                            anchors.fill: parent
                                            cursorShape: Qt.PointingHandCursor
                                            onClicked: {
                                                downloader.openFile()
                                            }
                                        }
                                    }

                                    Item{Layout.fillWidth: true}
                                }
                            }

                            //-- spacer --//
                            Item{Layout.preferredHeight: 20 }

                            Item{
                                Layout.fillWidth: true
                                Layout.preferredHeight: 38
                                RowLayout{
                                    anchors.fill: parent
                                    spacing: 10

                                    Item{Layout.fillWidth: true}


                                    //-- Button resume --//
                                    Rectangle{
                                        id: resume_btn
                                        Layout.fillHeight: true
                                        Layout.preferredWidth: parent.width * 0.3

                                        radius: height / 2

                                        color: "green"


                                        Label{
                                            anchors.centerIn: parent
                                            text: "ادامه دانلود"
                                            font.family: iranSans.name
                                            font.pixelSize: Qt.application.font.pixelSize * 1.5
                                            color: "#ffffff"
                                        }

                                        MouseArea{
                                            anchors.fill: parent
                                            cursorShape: Qt.PointingHandCursor
                                            onClicked: {
                                                popup.state = "resume"
                                                downloader.resumeBtn_clicked()
                                            }
                                        }
                                    }


                                    //-- Button pause --//
                                    Rectangle{
                                        id: pause_btn
                                        Layout.fillHeight: true
                                        Layout.preferredWidth: parent.width * 0.3

                                        radius: height / 2

                                        color: "red"


                                        Label{
                                            anchors.centerIn: parent
                                            text: "توقف دانلود"
                                            font.family: iranSans.name
                                            font.pixelSize: Qt.application.font.pixelSize * 1.5
                                            color: "#ffffff"
                                        }

                                        MouseArea{
                                            anchors.fill: parent
                                            cursorShape: Qt.PointingHandCursor
                                            onClicked: {
                                                popup.state = "pause"
                                                downloader.pauseBtn_clicked()
                                            }
                                        }
                                    }
                                    //-- Button download --//
                                    Rectangle{
                                        id: start_btn
                                        Layout.fillHeight: true
                                        Layout.preferredWidth: parent.width * 0.3

                                        radius: height / 2

                                        color: "darkblue"


                                        Label{
                                            anchors.centerIn: parent
                                            text: "شروع دانلود"
                                            font.family: iranSans.name
                                            font.pixelSize: Qt.application.font.pixelSize * 1.5
                                            color: "#ffffff"
                                        }

                                        MouseArea{
                                            anchors.fill: parent
                                            cursorShape: Qt.PointingHandCursor
                                            onClicked: {
                                                if (input_address.inputText.text != ""){
                                                    popup.state = "download"
                                                    alarmSignupWin.msg = ""
                                                    progressbar.visible = true
                                                    progressbar.implicitHeight = 30
                                                    downloader.downloadBtn_clicked(input_address.inputText.text, offlineStoragePath)
                                                }else{
                                                    success = false
                                                    alarmSignupWin.msg = "آدرس دانلود را وارد کنید"
                                                }
                                            }
                                        }
                                    }


                                    Item{Layout.fillWidth: true}
                                }
                            }

                            Item{Layout.fillHeight: true}

                            Item{
                                id: purchaseForm
                                Layout.fillWidth: true
                                Layout.preferredHeight: 50
                                //-- Alarm --//
                                Rectangle{
                                    id: alarmSignupWin

                                    property string msg: ""

                                    anchors.fill: parent

                                    color: (msg === "") ? "transparent" : (success) ?  "green" : "#E91E63"

                                    Label{
                                        id: lblAlarm3
                                        text: alarmSignupWin.msg
                                        anchors.centerIn: parent
                                        color: "white"

                                    }
                                }
                            }
                        }


                    }
                }
            }

        }

    }

}
