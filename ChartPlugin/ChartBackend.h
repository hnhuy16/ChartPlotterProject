#ifndef CHARTBACKEND_H
#define CHARTBACKEND_H

#include "core/IDataProcessor.h"

#include <QObject>
#include <QList>
#include <QPointF>
#include <QTimer>
#include <QtQmlIntegration/qqmlintegration.h>

#include <memory>
#include <vector>

class ChartBackend : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QList<QPointF> chartData READ chartData NOTIFY chartDataChanged)
    Q_PROPERTY(bool onlineMode READ onlineMode NOTIFY onlineModeChanged)
    Q_PROPERTY(double minX READ minX WRITE setMinX NOTIFY viewportChanged)
    Q_PROPERTY(double maxX READ maxX WRITE setMaxX NOTIFY viewportChanged)
    Q_PROPERTY(double minY READ minY WRITE setMinY NOTIFY viewportChanged)
    Q_PROPERTY(double maxY READ maxY WRITE setMaxY NOTIFY viewportChanged)

public:
    explicit ChartBackend(QObject *parent = nullptr);

    QList<QPointF> chartData() const;
    bool onlineMode() const;
    double minX() const;
    double maxX() const;
    double minY() const;
    double maxY() const;

    void setMinX(double minX);
    void setMaxX(double maxX);
    void setMinY(double minY);
    void setMaxY(double maxY);

    Q_INVOKABLE void setOfflineMode();
    Q_INVOKABLE void setOnlineMode();
    Q_INVOKABLE void generateDummyData();
    Q_INVOKABLE void clearData();
    Q_INVOKABLE void setViewport(double minX, double maxX, double minY, double maxY);

signals:
    void chartDataChanged();
    void onlineModeChanged();
    void viewportChanged();

private slots:
    void appendRealtimePoint();

private:
    void setProcessor(std::unique_ptr<IDataProcessor> processor);
    void refreshChartData();
    void updateChartDataFromProcessor();
    void setOnlineModeState(bool onlineMode);
    void setXViewport(double minX, double maxX);

    std::unique_ptr<IDataProcessor> m_processor;
    std::vector<QPointF> m_rawData;
    QList<QPointF> m_chartData;
    QTimer m_realtimeTimer;
    int m_targetPixelWidth = 800;
    bool m_onlineMode = false;
    double m_onlineX = 0.0;
    double m_minX = 0.0;
    double m_maxX = 199.0;
    double m_minY = -50.0;
    double m_maxY = 50.0;
};

#endif // CHARTBACKEND_H
