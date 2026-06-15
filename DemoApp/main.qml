import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

import ChartPlotter

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Chart Plotter Demo")

    ChartBackend {
        id: chartBackend
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        ChartView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            backend: chartBackend
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 10

            Button {
                text: qsTr("Generate Data")
                onClicked: chartBackend.generateDummyData()
            }

            Button {
                text: qsTr("Clear")
                onClicked: chartBackend.clearData()
            }
        }
    }
}
