#ifndef CHARTBACKEND_H
#define CHARTBACKEND_H

#include "core/IDataProcessor.h"

#include <QObject>
#include <QList>
#include <QPointF>
#include <QtQmlIntegration/qqmlintegration.h>

#include <memory>
#include <vector>

class ChartBackend : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QList<QPointF> chartData READ chartData NOTIFY chartDataChanged)

public:
    explicit ChartBackend(QObject *parent = nullptr);

    QList<QPointF> chartData() const;

    Q_INVOKABLE void setOfflineMode();
    Q_INVOKABLE void setOnlineMode();
    Q_INVOKABLE void generateDummyData();
    Q_INVOKABLE void clearData();

signals:
    void chartDataChanged();

private:
    void setProcessor(std::unique_ptr<IDataProcessor> processor);
    void refreshChartData();

    std::unique_ptr<IDataProcessor> m_processor;
    std::vector<QPointF> m_rawData;
    QList<QPointF> m_chartData;
    int m_targetPixelWidth = 800;
};

#endif // CHARTBACKEND_H
