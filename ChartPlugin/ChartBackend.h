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

public:
    explicit ChartBackend(QObject *parent = nullptr);

    QList<QPointF> chartData() const;
    bool onlineMode() const;

    Q_INVOKABLE void setOfflineMode();
    Q_INVOKABLE void setOnlineMode();
    Q_INVOKABLE void generateDummyData();
    Q_INVOKABLE void clearData();

signals:
    void chartDataChanged();
    void onlineModeChanged();

private slots:
    void appendRealtimePoint();

private:
    void setProcessor(std::unique_ptr<IDataProcessor> processor);
    void refreshChartData();
    void updateChartDataFromProcessor();
    void setOnlineModeState(bool onlineMode);

    std::unique_ptr<IDataProcessor> m_processor;
    std::vector<QPointF> m_rawData;
    QList<QPointF> m_chartData;
    QTimer m_realtimeTimer;
    int m_targetPixelWidth = 800;
    bool m_onlineMode = false;
    double m_onlineX = 0.0;
};

#endif // CHARTBACKEND_H
