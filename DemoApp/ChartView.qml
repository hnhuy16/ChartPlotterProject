import QtQuick

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

    function zoomX(factor) {
        scaleX = Math.max(0.1, Math.min(50, scaleX * factor));
        chartCanvas.requestPaint();
    }

    function zoomY(factor) {
        scaleY = Math.max(0.1, Math.min(50, scaleY * factor));
        chartCanvas.requestPaint();
    }

    function resetView() {
        offsetX = 0;
        offsetY = 0;
        scaleX = 1.0;
        scaleY = 1.0;
        chartCanvas.requestPaint();
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

            const isOnlineMode = root.backend && root.backend.onlineMode && root.chartType !== "Pie";
            if (isOnlineMode) {
                const viewportMinX = root.backend.minX;
                const viewportMaxX = root.backend.maxX;
                if (Number.isFinite(viewportMinX) && Number.isFinite(viewportMaxX) && viewportMinX !== viewportMaxX) {
                    minX = viewportMinX;
                    maxX = viewportMaxX;
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

            if (root.chartType === "Bar") {
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
            } else {
                if (root.lineStyle === "Dashed") {
                    ctx.setLineDash([10, 5]);
                } else if (root.lineStyle === "Dotted") {
                    ctx.setLineDash([2, 4]);
                } else {
                    ctx.setLineDash([]);
                }

                ctx.lineWidth = 2;
                ctx.beginPath();

                let started = false;
                for (let k = 0; k < data.length; ++k) {
                    const item = data[k];
                    const xValue = item.x;
                    const yValue = item.y;

                    if (!Number.isFinite(xValue) || !Number.isFinite(yValue)) {
                        continue;
                    }

                    const canvasX = mapX(xValue);
                    const canvasY = mapY(yValue);

                    if (started) {
                        ctx.lineTo(canvasX, canvasY);
                    } else {
                        ctx.moveTo(canvasX, canvasY);
                        started = true;
                    }
                }

                if (started) {
                    ctx.stroke();
                }

                ctx.setLineDash([]);
                ctx.fillStyle = selectedChartColor;
                const pointRadius = data.length > 500 ? 1.5 : 2.5;
                for (let l = 0; l < data.length; ++l) {
                    const dot = data[l];
                    const dotX = dot.x;
                    const dotY = dot.y;

                    if (!Number.isFinite(dotX) || !Number.isFinite(dotY)) {
                        continue;
                    }

                    ctx.beginPath();
                    ctx.arc(mapX(dotX), mapY(dotY), pointRadius, 0, Math.PI * 2);
                    ctx.fill();
                }
            }

            ctx.restore();
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            hoverEnabled: true

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

                chartCanvas.requestPaint();
                wheel.accepted = true;
            }

            onPressed: function(mouse) {
                root.lastPanX = mouse.x;
                root.lastPanY = mouse.y;
            }

            onPositionChanged: function(mouse) {
                if (!(mouse.buttons & Qt.LeftButton)) {
                    return;
                }

                const deltaX = mouse.x - root.lastPanX;
                const deltaY = mouse.y - root.lastPanY;

                root.offsetX += deltaX;
                root.offsetY += deltaY;
                root.lastPanX = mouse.x;
                root.lastPanY = mouse.y;

                chartCanvas.requestPaint();
            }
        }
    }

    Connections {
        target: root.backend

        function onChartDataChanged() {
            chartCanvas.requestPaint();
        }

        function onViewportChanged() {
            chartCanvas.requestPaint();
        }
    }

    onChartDataChanged: chartCanvas.requestPaint()
    onChartTypeChanged: chartCanvas.requestPaint()
    onChartColorChanged: chartCanvas.requestPaint()
    onLineStyleChanged: chartCanvas.requestPaint()
    onWidthChanged: chartCanvas.requestPaint()
    onHeightChanged: chartCanvas.requestPaint()
    Component.onCompleted: chartCanvas.requestPaint()
}
