#include "ChartBackend.h"

#include "core/OfflineDataProcessor.h"
#include "core/OnlineDataProcessor.h"

#include <QtMath>

ChartBackend::ChartBackend(QObject *parent) : QObject(parent) {}

QList<QPointF> ChartBackend::chartData() const
{
    return m_chartData;
}

void ChartBackend::setOfflineMode()
{
    setProcessor(std::make_unique<OfflineDataProcessor>());
}

void ChartBackend::setOnlineMode()
{
    setProcessor(std::make_unique<OnlineDataProcessor>());
}

void ChartBackend::generateDummyData()
{
    if (!m_processor) {
        setOfflineMode();
    }

    std::vector<QPointF> dummyData;
    dummyData.reserve(2000);

    for (int i = 0; i < 2000; ++i) {
        const double x = static_cast<double>(i);
        const double wave = qSin(x * 0.025) * 40.0;
        const double detail = qSin(x * 0.17) * 8.0;
        dummyData.emplace_back(x, wave + detail);
    }

    m_rawData = std::move(dummyData);

    if (m_processor) {
        m_processor->loadData(m_rawData);
    }

    refreshChartData();
}

void ChartBackend::clearData()
{
    m_rawData.clear();
    m_chartData.clear();

    if (m_processor) {
        m_processor->clear();
    }

    emit chartDataChanged();
}

void ChartBackend::setProcessor(std::unique_ptr<IDataProcessor> processor)
{
    m_processor = std::move(processor);
    refreshChartData();
}

void ChartBackend::refreshChartData()
{
    if (!m_processor) {
        m_chartData.clear();
        emit chartDataChanged();
        return;
    }

    m_processor->loadData(m_rawData);
    const std::vector<QPointF> processedData = m_processor->getProcessedData(m_targetPixelWidth);
    m_chartData = QList<QPointF>(processedData.begin(), processedData.end());

    emit chartDataChanged();
}
