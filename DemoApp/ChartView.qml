import QtQuick

Item {
    id: root

    property var backend: null
    property var chartData: []

    Canvas {
        id: canvas
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

            ctx.strokeStyle = "#e2e8f0";
            ctx.lineWidth = 1;

            const gridLines = 5;
            for (let i = 0; i <= gridLines; ++i) {
                const x = plotLeft + (plotWidth * i / gridLines);
                ctx.beginPath();
                ctx.moveTo(x, plotTop);
                ctx.lineTo(x, plotBottom);
                ctx.stroke();

                const y = plotTop + (plotHeight * i / gridLines);
                ctx.beginPath();
                ctx.moveTo(plotLeft, y);
                ctx.lineTo(plotRight, y);
                ctx.stroke();
            }

            ctx.strokeStyle = "#334155";
            ctx.lineWidth = 2;
            ctx.beginPath();
            ctx.moveTo(plotLeft, plotTop);
            ctx.lineTo(plotLeft, plotBottom);
            ctx.lineTo(plotRight, plotBottom);
            ctx.stroke();

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

            if (minX === maxX) {
                minX -= 1;
                maxX += 1;
            }

            if (minY === maxY) {
                minY -= 1;
                maxY += 1;
            }

            function mapX(value) {
                return plotLeft + ((value - minX) / (maxX - minX)) * plotWidth;
            }

            function mapY(value) {
                return plotBottom - ((value - minY) / (maxY - minY)) * plotHeight;
            }

            ctx.strokeStyle = "#2563eb";
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

            ctx.fillStyle = "#1d4ed8";
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
    }

    Connections {
        target: root.backend

        function onChartDataChanged() {
            canvas.requestPaint();
        }
    }

    onChartDataChanged: canvas.requestPaint()
    onWidthChanged: canvas.requestPaint()
    onHeightChanged: canvas.requestPaint()
    Component.onCompleted: canvas.requestPaint()
}
