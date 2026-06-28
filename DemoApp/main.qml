import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtCore

import ChartPlotter

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Chart Plotter Demo")

    ChartBackend {
        id: chartBackend
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Select Data File")
        currentFolder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        nameFilters: [qsTr("Data files (*.csv *.txt)"), qsTr("All files (*)")]
        onAccepted: chartBackend.loadFromFile(fileDialog.selectedFile)
    }

    function setChartColor(color) {
        chartView.chartColor = color;
        chartView.canvas.requestPaint();
    }

    function setLineStyle(style) {
        chartView.lineStyle = style;
        chartView.canvas.requestPaint();
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        ChartView {
            id: chartView
            Layout.fillWidth: true
            Layout.fillHeight: true
            backend: chartBackend
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            spacing: 8

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Zoom X+")
                onClicked: chartView.zoomX(1.1)
            }

            Button {
                text: qsTr("Zoom X-")
                onClicked: chartView.zoomX(1 / 1.1)
            }

            Button {
                text: qsTr("Zoom Y+")
                onClicked: chartView.zoomY(1.1)
            }

            Button {
                text: qsTr("Zoom Y-")
                onClicked: chartView.zoomY(1 / 1.1)
            }

            Button {
                text: qsTr("Crop")
                checkable: true
                checked: chartView.cropMode
                highlighted: checked
                enabled: chartView.chartType !== "Pie"
                onToggled: chartView.cropMode = checked

                ToolTip.visible: hovered
                ToolTip.text: qsTr("Drag a region to zoom")
            }

            Button {
                text: qsTr("Reset")
                onClicked: chartView.resetView()
            }

            Item {
                Layout.fillWidth: true
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            spacing: 8

            Item {
                Layout.fillWidth: true
            }

            Label {
                text: qsTr("Color")
            }

            Button {
                checkable: true
                checked: Qt.colorEqual(chartView.chartColor, "red")
                highlighted: checked
                implicitWidth: 32
                implicitHeight: 32
                text: qsTr("")
                onClicked: setChartColor("red")

                background: Rectangle {
                    radius: 4
                    color: "#dc2626"
                    border.width: parent.checked ? 3 : 1
                    border.color: parent.checked ? "#111827" : "#94a3b8"
                }
            }

            Button {
                checkable: true
                checked: Qt.colorEqual(chartView.chartColor, "blue")
                highlighted: checked
                implicitWidth: 32
                implicitHeight: 32
                text: qsTr("")
                onClicked: setChartColor("blue")

                background: Rectangle {
                    radius: 4
                    color: "#2563eb"
                    border.width: parent.checked ? 3 : 1
                    border.color: parent.checked ? "#111827" : "#94a3b8"
                }
            }

            Button {
                checkable: true
                checked: Qt.colorEqual(chartView.chartColor, "green")
                highlighted: checked
                implicitWidth: 32
                implicitHeight: 32
                text: qsTr("")
                onClicked: setChartColor("green")

                background: Rectangle {
                    radius: 4
                    color: "#16a34a"
                    border.width: parent.checked ? 3 : 1
                    border.color: parent.checked ? "#111827" : "#94a3b8"
                }
            }

            Button {
                checkable: true
                checked: Qt.colorEqual(chartView.chartColor, "orange")
                highlighted: checked
                implicitWidth: 32
                implicitHeight: 32
                text: qsTr("")
                onClicked: setChartColor("orange")

                background: Rectangle {
                    radius: 4
                    color: "#f97316"
                    border.width: parent.checked ? 3 : 1
                    border.color: parent.checked ? "#111827" : "#94a3b8"
                }
            }

            Label {
                text: qsTr("Line")
            }

            Button {
                text: qsTr("Solid")
                checkable: true
                checked: chartView.lineStyle === "Solid"
                highlighted: checked
                onClicked: setLineStyle("Solid")
            }

            Button {
                text: qsTr("Dashed")
                checkable: true
                checked: chartView.lineStyle === "Dashed"
                highlighted: checked
                onClicked: setLineStyle("Dashed")
            }

            Button {
                text: qsTr("Dotted")
                checkable: true
                checked: chartView.lineStyle === "Dotted"
                highlighted: checked
                onClicked: setLineStyle("Dotted")
            }

            Item {
                Layout.fillWidth: true
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 10

            Button {
                text: qsTr("Line")
                checkable: true
                checked: chartView.chartType === "Line"
                highlighted: chartView.chartType === "Line"
                onClicked: {
                    chartView.chartType = "Line";
                    chartView.canvas.requestPaint();
                }
            }

            Button {
                text: qsTr("Bar")
                checkable: true
                checked: chartView.chartType === "Bar"
                highlighted: chartView.chartType === "Bar"
                onClicked: {
                    chartView.chartType = "Bar";
                    chartView.canvas.requestPaint();
                }
            }

            Button {
                text: qsTr("Pie")
                checkable: true
                checked: chartView.chartType === "Pie"
                highlighted: chartView.chartType === "Pie"
                onClicked: {
                    chartView.chartType = "Pie";
                    chartView.canvas.requestPaint();
                }
            }

            Button {
                text: qsTr("Generate Data")
                onClicked: chartBackend.generateDummyData()
            }

            Button {
                text: qsTr("Open File")
                onClicked: fileDialog.open()
            }

            Button {
                text: qsTr("Start Online")
                onClicked: chartBackend.setOnlineMode()
            }

            Button {
                text: qsTr("Stop Online")
                enabled: chartBackend.onlineMode
                onClicked: chartBackend.setOfflineMode()
            }

            Button {
                text: qsTr("Clear")
                onClicked: chartBackend.clearData()
            }
        }
    }
}
