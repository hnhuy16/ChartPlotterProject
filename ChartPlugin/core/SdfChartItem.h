#ifndef SDFCHARTITEM_H
#define SDFCHARTITEM_H

#include <QColor>
#include <QList>
#include <QPointF>
#include <QQuickItem>
#include <QtQmlIntegration/qqmlintegration.h>

class SdfChartItem : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QList<QPointF> chartData READ chartData WRITE setChartData NOTIFY chartDataChanged)
    Q_PROPERTY(QColor lineColor READ lineColor WRITE setLineColor NOTIFY lineColorChanged)
    Q_PROPERTY(qreal lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)
    Q_PROPERTY(qreal minX READ minX WRITE setMinX NOTIFY minXChanged)
    Q_PROPERTY(qreal maxX READ maxX WRITE setMaxX NOTIFY maxXChanged)
    Q_PROPERTY(qreal minY READ minY WRITE setMinY NOTIFY minYChanged)
    Q_PROPERTY(qreal maxY READ maxY WRITE setMaxY NOTIFY maxYChanged)

public:
    explicit SdfChartItem(QQuickItem *parent = nullptr);

    QList<QPointF> chartData() const;
    QColor lineColor() const;
    qreal lineWidth() const;
    qreal minX() const;
    qreal maxX() const;
    qreal minY() const;
    qreal maxY() const;

    void setChartData(const QList<QPointF> &chartData);
    void setLineColor(const QColor &lineColor);
    void setLineWidth(qreal lineWidth);
    void setMinX(qreal minX);
    void setMaxX(qreal maxX);
    void setMinY(qreal minY);
    void setMaxY(qreal maxY);

signals:
    void chartDataChanged();
    void lineColorChanged();
    void lineWidthChanged();
    void minXChanged();
    void maxXChanged();
    void minYChanged();
    void maxYChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

private:
    QList<QPointF> m_chartData;
    QColor m_lineColor = QColorConstants::Svg::blue;
    qreal m_lineWidth = 2.0;
    qreal m_minX = 0.0;
    qreal m_maxX = 1.0;
    qreal m_minY = 0.0;
    qreal m_maxY = 1.0;
};

#endif // SDFCHARTITEM_H
