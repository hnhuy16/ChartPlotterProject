import QtQuick
import ChartPlotter

Item {
    id: root

    property var backend: null
    property var chartData: []
    property string chartType: "Line"
    property color chartColor: "blue"
    property string lineStyle: "Solid"
    property alias canvas: chartCanvas
    property real offsetX: 0
    property real offsetY: 0
    property real scaleX: 1
    property real scaleY: 1
    property real lastPanX: 0
    property real lastPanY: 0
    property bool cropMode: false
    property rect selectionBox: Qt.rect(0, 0, 0, 0)
    property bool cropSelecting: false
    property real cropStartX: 0
    property real cropStartY: 0
    property bool pointingVisible: false
    property real pointingMouseX: 0
    property real pointingMouseY: 0
    property real pointingDataX: NaN
    property real pointingDataY: NaN
    property real pointingPointX: NaN
    property real pointingPointY: NaN
    property real pointingCanvasX: NaN
    property real pointingCanvasY: NaN
    readonly property real pointingRadius: 14
    readonly property var dataBounds: {
        const data = root.backend ? root.backend.chartData : (root.chartData || []);
        let minX = Number.POSITIVE_INFINITY;
        let maxX = Number.NEGATIVE_INFINITY;
        let minY = Number.POSITIVE_INFINITY;
        let maxY = Number.NEGATIVE_INFINITY;

        for (let i = 0; i < data.length; ++i) {
            const point = data[i];
            if (!Number.isFinite(point.x) || !Number.isFinite(point.y)) {
                continue;
            }
            minX = Math.min(minX, point.x);
            maxX = Math.max(maxX, point.x);
            minY = Math.min(minY, point.y);
            maxY = Math.max(maxY, point.y);
        }

        if (!Number.isFinite(minX) || !Number.isFinite(minY)) {
            return { minX: 0, maxX: 1, minY: 0, maxY: 1 };
        }
        if (minX === maxX) {
            minX -= 1;
            maxX += 1;
        }
        if (minY === maxY) {
            minY -= 1;
            maxY += 1;
        }
        return { minX: minX, maxX: maxX, minY: minY, maxY: maxY };
    }

    function updatePointing(mouseX, mouseY) {
        if (cropMode) {
            clearPointing();
            return;
        }

        pointingMouseX = mouseX;
        pointingMouseY = mouseY;

        const plotLeft = chartCanvas.leftPadding;
        const plotRight = Math.max(plotLeft + 1, chartCanvas.width - chartCanvas.rightPadding);
        const plotTop = chartCanvas.topPadding;
        const plotBottom = Math.max(plotTop + 1, chartCanvas.height - chartCanvas.bottomPadding);

        if (mouseX < plotLeft || mouseX > plotRight
                || mouseY < plotTop || mouseY > plotBottom
                || root.chartType === "Pie") {
            pointingVisible = false;
            chartCanvas.requestPaint();
            return;
        }

        const data = root.backend ? root.backend.chartData : (root.chartData || []);
        let minX = Number.POSITIVE_INFINITY;
        let maxX = Number.NEGATIVE_INFINITY;
        let minY = Number.POSITIVE_INFINITY;
        let maxY = Number.NEGATIVE_INFINITY;

        for (let i = 0; i < data.length; ++i) {
            const x = data[i].x;
            const y = data[i].y;
            if (!Number.isFinite(x) || !Number.isFinite(y)) {
                continue;
            }
            minX = Math.min(minX, x);
            maxX = Math.max(maxX, x);
            minY = Math.min(minY, y);
            maxY = Math.max(maxY, y);
        }

        if (!Number.isFinite(minX) || !Number.isFinite(minY)) {
            pointingVisible = false;
            chartCanvas.requestPaint();
            return;
        }

        if (root.backend) {
            const viewportMinX = root.backend.minX;
            const viewportMaxX = root.backend.maxX;
            const viewportMinY = root.backend.minY;
            const viewportMaxY = root.backend.maxY;
            if (Number.isFinite(viewportMinX) && Number.isFinite(viewportMaxX)
                    && viewportMinX !== viewportMaxX) {
                minX = viewportMinX;
                maxX = viewportMaxX;
            }
            if (Number.isFinite(viewportMinY) && Number.isFinite(viewportMaxY)
                    && viewportMinY !== viewportMaxY) {
                minY = viewportMinY;
                maxY = viewportMaxY;
            }
        }

        if (minX === maxX) {
            minX -= 1;
            maxX += 1;
        }
        if (minY === maxY) {
            minY -= 1;
            maxY += 1;
        }
        if (root.chartType === "Bar" && !root.backend) {
            minY = Math.min(minY, 0);
            maxY = Math.max(maxY, 0);
        }

        const plotWidth = plotRight - plotLeft;
        const plotHeight = plotBottom - plotTop;

        function mapX(value) {
            const normalized = (value - minX) / (maxX - minX);
            return plotLeft + root.offsetX + normalized * plotWidth * root.scaleX;
        }

        function mapY(value) {
            const normalized = (value - minY) / (maxY - minY);
            return plotBottom + root.offsetY - normalized * plotHeight * root.scaleY;
        }

        // Keep the cursor position in data space as well as canvas space.
        pointingDataX = minX + ((mouseX - plotLeft - root.offsetX)
                               / (plotWidth * root.scaleX)) * (maxX - minX);
        pointingDataY = minY + ((plotBottom + root.offsetY - mouseY)
                               / (plotHeight * root.scaleY)) * (maxY - minY);

        let nearestDistanceSquared = root.pointingRadius * root.pointingRadius;
        let nearestPoint = null;
        let nearestCanvasX = NaN;
        let nearestCanvasY = NaN;

        for (let j = 0; j < data.length; ++j) {
            const point = data[j];
            if (!Number.isFinite(point.x) || !Number.isFinite(point.y)) {
                continue;
            }

            const canvasX = mapX(point.x);
            const canvasY = mapY(point.y);
            if (canvasX < plotLeft || canvasX > plotRight
                    || canvasY < plotTop || canvasY > plotBottom) {
                continue;
            }

            const deltaX = canvasX - mouseX;
            const deltaY = canvasY - mouseY;
            const distanceSquared = deltaX * deltaX + deltaY * deltaY;
            if (distanceSquared <= nearestDistanceSquared) {
                nearestDistanceSquared = distanceSquared;
                nearestPoint = point;
                nearestCanvasX = canvasX;
                nearestCanvasY = canvasY;
            }
        }

        pointingVisible = nearestPoint !== null;
        if (pointingVisible) {
            pointingPointX = nearestPoint.x;
            pointingPointY = nearestPoint.y;
            pointingCanvasX = nearestCanvasX;
            pointingCanvasY = nearestCanvasY;
        }
        chartCanvas.requestPaint();
    }

    function clearPointing() {
        pointingVisible = false;
        chartCanvas.requestPaint();
    }

    function refreshPointing() {
        if (pointerArea.containsMouse) {
            updatePointing(pointerArea.mouseX, pointerArea.mouseY);
        } else {
            clearPointing();
        }
    }

    function formatPointingValue(value) {
        return Number(value).toLocaleString(Qt.locale(), "g", 6);
    }

    function zoomX(factor) {
        scaleX = Math.max(0.1, Math.min(50, scaleX * factor));
        chartCanvas.requestPaint();
        refreshPointing();
    }

    function zoomY(factor) {
        scaleY = Math.max(0.1, Math.min(50, scaleY * factor));
        chartCanvas.requestPaint();
        refreshPointing();
    }

    function resetView() {
        offsetX = 0;
        offsetY = 0;
        scaleX = 1.0;
        scaleY = 1.0;
        if (backend) {
            backend.resetBounds();
        }
        chartCanvas.requestPaint();
        refreshPointing();
    }

    function clampToPlotX(value) {
        return Math.max(chartCanvas.leftPadding,
                        Math.min(chartCanvas.width - chartCanvas.rightPadding, value));
    }

    function clampToPlotY(value) {
        return Math.max(chartCanvas.topPadding,
                        Math.min(chartCanvas.height - chartCanvas.bottomPadding, value));
    }

    function applyCrop() {
        const selectionLeft = selectionBox.x;
        const selectionTop = selectionBox.y;
        const selectionWidth = selectionBox.width;
        const selectionHeight = selectionBox.height;

        cropSelecting = false;
        if (selectionWidth < 8 || selectionHeight < 8) {
            selectionBox = Qt.rect(0, 0, 0, 0);
            cropMode = false;
            chartCanvas.requestPaint();
            return;
        }

        const plotLeft = chartCanvas.leftPadding;
        const plotTop = chartCanvas.topPadding;
        const plotRight = chartCanvas.width - chartCanvas.rightPadding;
        const plotBottom = chartCanvas.height - chartCanvas.bottomPadding;
        const plotWidth = plotRight - plotLeft;
        const plotHeight = plotBottom - plotTop;
        if (root.backend && Number.isFinite(root.backend.minX)
                && Number.isFinite(root.backend.maxX)
                && Number.isFinite(root.backend.minY)
                && Number.isFinite(root.backend.maxY)) {
            const currentMinX = root.backend.minX;
            const currentMaxX = root.backend.maxX;
            const currentMinY = root.backend.minY;
            const currentMaxY = root.backend.maxY;
            const visibleMinX = currentMinX
                    + ((selectionLeft - plotLeft - offsetX) / (plotWidth * scaleX))
                    * (currentMaxX - currentMinX);
            const visibleMaxX = currentMinX
                    + ((selectionLeft + selectionWidth - plotLeft - offsetX)
                       / (plotWidth * scaleX)) * (currentMaxX - currentMinX);
            const visibleMaxY = currentMinY
                    + ((plotBottom + offsetY - selectionTop) / (plotHeight * scaleY))
                    * (currentMaxY - currentMinY);
            const visibleMinY = currentMinY
                    + ((plotBottom + offsetY - selectionTop - selectionHeight)
                       / (plotHeight * scaleY)) * (currentMaxY - currentMinY);

            root.backend.setViewport(visibleMinX, visibleMaxX,
                                     visibleMinY, visibleMaxY);
            offsetX = 0;
            offsetY = 0;
            scaleX = 1;
            scaleY = 1;
        }

        selectionBox = Qt.rect(0, 0, 0, 0);
        cropMode = false;
        chartCanvas.requestPaint();
        refreshPointing();
    }

    Canvas {
        id: chartCanvas
        anchors.fill: parent
        antialiasing: true

        readonly property real leftPadding: 56
        readonly property real rightPadding: 20
        readonly property real topPadding: 20
        readonly property real bottomPadding: 44

        onPaint: {
            const ctx = getContext("2d");
            const w = width;
            const h = height;

            ctx.clearRect(0, 0, w, h);

            if (w <= 0 || h <= 0) {
                return;
            }

            ctx.fillStyle = "#f8fafc";
            ctx.fillRect(0, 0, w, h);

            const plotLeft = leftPadding;
            const plotRight = Math.max(plotLeft + 1, w - rightPadding);
            const plotTop = topPadding;
            const plotBottom = Math.max(plotTop + 1, h - bottomPadding);
            const plotWidth = plotRight - plotLeft;
            const plotHeight = plotBottom - plotTop;
            const data = root.backend ? root.backend.chartData : (root.chartData || []);
            const selectedChartColor = root.chartColor.toString();

            function transformedX(normalizedValue) {
                return plotLeft + root.offsetX + normalizedValue * plotWidth * root.scaleX;
            }

            function transformedY(normalizedValue) {
                return plotBottom + root.offsetY - normalizedValue * plotHeight * root.scaleY;
            }

            if (root.chartType !== "Pie") {
                ctx.save();
                ctx.beginPath();
                ctx.rect(plotLeft, plotTop, plotWidth, plotHeight);
                ctx.clip();

                ctx.strokeStyle = "#e2e8f0";
                ctx.lineWidth = 1;

                const gridLines = 5;
                for (let i = 0; i <= gridLines; ++i) {
                    const normalized = i / gridLines;
                    const x = transformedX(normalized);
                    ctx.beginPath();
                    ctx.moveTo(x, plotTop);
                    ctx.lineTo(x, plotBottom);
                    ctx.stroke();

                    const y = transformedY(1 - normalized);
                    ctx.beginPath();
                    ctx.moveTo(plotLeft, y);
                    ctx.lineTo(plotRight, y);
                    ctx.stroke();
                }

                ctx.restore();

                ctx.strokeStyle = "#334155";
                ctx.lineWidth = 2;
                ctx.beginPath();
                ctx.moveTo(plotLeft, plotTop);
                ctx.lineTo(plotLeft, plotBottom);
                ctx.lineTo(plotRight, plotBottom);
                ctx.stroke();
            }

            if (data.length === 0) {
                ctx.fillStyle = "#64748b";
                ctx.font = "14px sans-serif";
                ctx.textAlign = "center";
                ctx.textBaseline = "middle";
                ctx.fillText(qsTr("No data"), plotLeft + plotWidth / 2, plotTop + plotHeight / 2);
                return;
            }

            let minX = Number.POSITIVE_INFINITY;
            let maxX = Number.NEGATIVE_INFINITY;
            let minY = Number.POSITIVE_INFINITY;
            let maxY = Number.NEGATIVE_INFINITY;

            for (let j = 0; j < data.length; ++j) {
                const point = data[j];
                const px = point.x;
                const py = point.y;

                if (!Number.isFinite(px) || !Number.isFinite(py)) {
                    continue;
                }

                minX = Math.min(minX, px);
                maxX = Math.max(maxX, px);
                minY = Math.min(minY, py);
                maxY = Math.max(maxY, py);
            }

            if (!Number.isFinite(minX) || !Number.isFinite(minY)) {
                return;
            }

            if (root.backend && root.chartType !== "Pie") {
                const viewportMinX = root.backend.minX;
                const viewportMaxX = root.backend.maxX;
                const viewportMinY = root.backend.minY;
                const viewportMaxY = root.backend.maxY;
                if (Number.isFinite(viewportMinX) && Number.isFinite(viewportMaxX) && viewportMinX !== viewportMaxX) {
                    minX = viewportMinX;
                    maxX = viewportMaxX;
                }
                if (Number.isFinite(viewportMinY) && Number.isFinite(viewportMaxY) && viewportMinY !== viewportMaxY) {
                    minY = viewportMinY;
                    maxY = viewportMaxY;
                }
            }

            if (minX === maxX) {
                minX -= 1;
                maxX += 1;
            }

            if (minY === maxY) {
                minY -= 1;
                maxY += 1;
            }

            function mapX(value) {
                return transformedX((value - minX) / (maxX - minX));
            }

            if (root.chartType === "Bar" && !root.backend) {
                minY = Math.min(minY, 0);
                maxY = Math.max(maxY, 0);
            }

            function mapY(value) {
                return transformedY((value - minY) / (maxY - minY));
            }

            ctx.save();
            ctx.beginPath();
            ctx.rect(plotLeft, plotTop, plotWidth, plotHeight);
            ctx.clip();
            ctx.strokeStyle = selectedChartColor;
            ctx.fillStyle = selectedChartColor;

            if (root.chartType === "Bar") {
                const baselineY = mapY(0);
                const barWidth = Math.max(1, Math.min(48, plotWidth / Math.max(data.length, 1) * root.scaleX * 0.75));

                for (let k = 0; k < data.length; ++k) {
                    const item = data[k];
                    const xValue = item.x;
                    const yValue = item.y;

                    if (!Number.isFinite(xValue) || !Number.isFinite(yValue)) {
                        continue;
                    }

                    const canvasX = mapX(xValue);
                    const canvasY = mapY(yValue);
                    const barTop = Math.min(canvasY, baselineY);
                    const barHeight = Math.abs(baselineY - canvasY);

                    ctx.fillRect(canvasX - barWidth / 2, barTop, barWidth, Math.max(1, barHeight));
                }
            } else if (root.chartType === "Pie") {
                const pieValues = [];
                for (let k = 0; k < data.length && pieValues.length < 10; ++k) {
                    const value = Math.abs(data[k].y);
                    if (Number.isFinite(value) && value > 0) {
                        pieValues.push(value);
                    }
                }

                const total = pieValues.reduce(function(sum, value) { return sum + value; }, 0);
                if (total > 0) {
                    const centerX = plotLeft + plotWidth / 2;
                    const centerY = plotTop + plotHeight / 2;
                    const radius = Math.max(8, Math.min(plotWidth, plotHeight) * 0.42);
                    let startAngle = -Math.PI / 2;

                    for (let l = 0; l < pieValues.length; ++l) {
                        const sliceAngle = pieValues[l] / total * Math.PI * 2;
                        const endAngle = startAngle + sliceAngle;

                        ctx.beginPath();
                        ctx.moveTo(centerX, centerY);
                        ctx.arc(centerX, centerY, radius, startAngle, endAngle);
                        ctx.closePath();
                        ctx.globalAlpha = 0.55 + (l % 4) * 0.12;
                        ctx.fillStyle = selectedChartColor;
                        ctx.fill();
                        ctx.globalAlpha = 1.0;
                        ctx.strokeStyle = "#f8fafc";
                        ctx.lineWidth = 1;
                        ctx.stroke();

                        startAngle = endAngle;
                    }
                }
            }

            ctx.restore();

            if (root.pointingVisible) {
                ctx.beginPath();
                ctx.arc(root.pointingCanvasX, root.pointingCanvasY, 6, 0, Math.PI * 2);
                ctx.fillStyle = "#ffffff";
                ctx.fill();
                ctx.lineWidth = 3;
                ctx.strokeStyle = selectedChartColor;
                ctx.stroke();
            }

            if (root.selectionBox.width > 0 && root.selectionBox.height > 0) {
                ctx.fillStyle = "rgba(0, 150, 255, 0.3)";
                ctx.fillRect(root.selectionBox.x, root.selectionBox.y,
                             root.selectionBox.width, root.selectionBox.height);
                ctx.strokeStyle = "#0096ff";
                ctx.lineWidth = 1;
                ctx.strokeRect(root.selectionBox.x + 0.5, root.selectionBox.y + 0.5,
                               Math.max(0, root.selectionBox.width - 1),
                               Math.max(0, root.selectionBox.height - 1));
            }
        }

        Item {
            id: sdfPlotArea
            x: chartCanvas.leftPadding
            y: chartCanvas.topPadding
            width: Math.max(1, chartCanvas.width - chartCanvas.leftPadding - chartCanvas.rightPadding)
            height: Math.max(1, chartCanvas.height - chartCanvas.topPadding - chartCanvas.bottomPadding)
            clip: true
            visible: root.chartType === "Line"

            SdfChartItem {
                x: root.offsetX
                y: sdfPlotArea.height + root.offsetY - height
                width: sdfPlotArea.width * root.scaleX
                height: sdfPlotArea.height * root.scaleY
                chartData: root.backend ? root.backend.chartData : root.chartData
                lineColor: root.chartColor
                lineWidth: 2
                minX: root.backend ? root.backend.minX : root.dataBounds.minX
                maxX: root.backend ? root.backend.maxX : root.dataBounds.maxX
                minY: root.backend ? root.backend.minY : root.dataBounds.minY
                maxY: root.backend ? root.backend.maxY : root.dataBounds.maxY
            }
        }

        MouseArea {
            id: pointerArea
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            hoverEnabled: true
            cursorShape: root.cropMode ? Qt.CrossCursor : Qt.ArrowCursor

            onWheel: function(wheel) {
                const zoomFactor = wheel.angleDelta.y > 0 ? 1.1 : 1 / 1.1;
                const oldScaleX = root.scaleX;
                const oldScaleY = root.scaleY;
                const zoomX = wheel.modifiers & Qt.ControlModifier;
                const zoomY = wheel.modifiers & Qt.ShiftModifier;
                const zoomBoth = !zoomX && !zoomY;

                if (zoomX || zoomBoth) {
                    root.scaleX = Math.max(0.1, Math.min(50, root.scaleX * zoomFactor));
                }

                if (zoomY || zoomBoth) {
                    root.scaleY = Math.max(0.1, Math.min(50, root.scaleY * zoomFactor));
                }

                const plotLeft = chartCanvas.leftPadding;
                const plotBottom = Math.max(chartCanvas.topPadding + 1, chartCanvas.height - chartCanvas.bottomPadding);
                const dataX = (wheel.x - plotLeft - root.offsetX) / oldScaleX;
                const dataY = (wheel.y - plotBottom - root.offsetY) / oldScaleY;

                if (zoomX || zoomBoth) {
                    root.offsetX = wheel.x - plotLeft - dataX * root.scaleX;
                }

                if (zoomY || zoomBoth) {
                    root.offsetY = wheel.y - plotBottom - dataY * root.scaleY;
                }

                root.updatePointing(wheel.x, wheel.y);
                wheel.accepted = true;
            }

            onPressed: function(mouse) {
                let isCropping = root.cropMode
                        || (mouse.modifiers & Qt.AltModifier);
                if (isCropping && root.chartType !== "Pie") {
                    const plotLeft = chartCanvas.leftPadding;
                    const plotRight = chartCanvas.width - chartCanvas.rightPadding;
                    const plotTop = chartCanvas.topPadding;
                    const plotBottom = chartCanvas.height - chartCanvas.bottomPadding;
                    if (mouse.x < plotLeft || mouse.x > plotRight
                            || mouse.y < plotTop || mouse.y > plotBottom) {
                        mouse.accepted = false;
                        return;
                    }

                    const startX = root.clampToPlotX(mouse.x);
                    const startY = root.clampToPlotY(mouse.y);
                    root.cropStartX = startX;
                    root.cropStartY = startY;
                    root.selectionBox = Qt.rect(startX, startY, 0, 0);
                    root.cropSelecting = true;
                    root.clearPointing();
                    chartCanvas.requestPaint();
                    return;
                }

                root.lastPanX = mouse.x;
                root.lastPanY = mouse.y;
            }

            onPositionChanged: function(mouse) {
                let isCropping = root.cropMode
                        || (mouse.modifiers & Qt.AltModifier);
                if ((isCropping || root.cropSelecting) && root.cropSelecting) {
                    const endX = root.clampToPlotX(mouse.x);
                    const endY = root.clampToPlotY(mouse.y);
                    const startX = root.cropStartX;
                    const startY = root.cropStartY;
                    root.selectionBox = Qt.rect(Math.min(startX, endX),
                                                Math.min(startY, endY),
                                                Math.abs(endX - startX),
                                                Math.abs(endY - startY));
                    chartCanvas.requestPaint();
                    return;
                }

                if (isCropping) {
                    return;
                }

                if (mouse.buttons & Qt.LeftButton) {
                    const deltaX = mouse.x - root.lastPanX;
                    const deltaY = mouse.y - root.lastPanY;

                    root.offsetX += deltaX;
                    root.offsetY += deltaY;
                    root.lastPanX = mouse.x;
                    root.lastPanY = mouse.y;
                }

                root.updatePointing(mouse.x, mouse.y);
            }

            onReleased: function(mouse) {
                let isCropping = root.cropMode
                        || (mouse.modifiers & Qt.AltModifier);
                if ((isCropping || root.cropSelecting) && root.cropSelecting) {
                    const endX = root.clampToPlotX(mouse.x);
                    const endY = root.clampToPlotY(mouse.y);
                    const startX = root.cropStartX;
                    const startY = root.cropStartY;
                    root.selectionBox = Qt.rect(Math.min(startX, endX),
                                                Math.min(startY, endY),
                                                Math.abs(endX - startX),
                                                Math.abs(endY - startY));
                    root.applyCrop();
                }
            }

            onCanceled: {
                root.cropSelecting = false;
                root.selectionBox = Qt.rect(0, 0, 0, 0);
                chartCanvas.requestPaint();
            }
            onExited: root.clearPointing()
        }

        Rectangle {
            id: pointingTooltip
            visible: root.pointingVisible
            z: 2
            color: "#e6ffffff"
            border.color: "#94a3b8"
            border.width: 1
            radius: 4
            width: pointingText.implicitWidth + 16
            height: pointingText.implicitHeight + 10
            x: Math.min(chartCanvas.width - width - 4, root.pointingMouseX + 12)
            y: Math.max(4, Math.min(chartCanvas.height - height - 4,
                                   root.pointingMouseY - height - 12))

            Text {
                id: pointingText
                anchors.centerIn: parent
                color: "#0f172a"
                text: qsTr("(X: %1, Y: %2)")
                    .arg(root.formatPointingValue(root.pointingPointX))
                    .arg(root.formatPointingValue(root.pointingPointY))
            }
        }
    }

    Connections {
        target: root.backend

        function onChartDataChanged() {
            chartCanvas.requestPaint();
            root.refreshPointing();
        }

        function onViewportChanged() {
            chartCanvas.requestPaint();
            root.refreshPointing();
        }
    }

    onChartDataChanged: {
        chartCanvas.requestPaint();
        refreshPointing();
    }
    onChartTypeChanged: {
        if (chartType === "Pie") {
            cropMode = false;
        }
        chartCanvas.requestPaint();
        refreshPointing();
    }
    onCropModeChanged: {
        cropSelecting = false;
        if (!cropMode) {
            selectionBox = Qt.rect(0, 0, 0, 0);
        }
        clearPointing();
    }
    onChartColorChanged: chartCanvas.requestPaint()
    onLineStyleChanged: chartCanvas.requestPaint()
    onWidthChanged: {
        chartCanvas.requestPaint();
        refreshPointing();
    }
    onHeightChanged: {
        chartCanvas.requestPaint();
        refreshPointing();
    }
    Component.onCompleted: chartCanvas.requestPaint()
}
