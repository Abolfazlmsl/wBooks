import QtQuick 2.12
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

//-- Left Menu --//
Rectangle{
    id: leftMenu

    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        propagateComposedEvents: true

        TreeView {
            id: delegateTreeView

            anchors.fill: parent
            clip: true
            anchors.margins: 1

            model: contentModel
            selectionEnabled: true
            hoverEnabled: true

            color: "white"
            handleColor: "white"
            hoverColor: "skyblue"
            selectedColor: "cornflowerblue"
            selectedItemColor: "white"
            handleStyle: TreeView.Handle.Chevron
            rowHeight: 50
            rowPadding: 0
            rowSpacing: 0
            font.pixelSize: 12
        }

        onEntered: {
            epubArea.enabled = false
            mainArea.enabled = false
            cursorShape = Qt.PointingHandCursor
        }

        onExited: {
            epubArea.enabled = true
            mainArea.enabled = true
            cursorShape = Qt.ArrowCursor
        }
    }
}
