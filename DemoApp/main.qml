import QtQuick
import QtQuick.Window

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Chart Plotter Demo")

    Text {
        anchors.centerIn: parent
        text: "Hello Chart Plotter Plugin!"
        font.pixelSize: 24
    }
}