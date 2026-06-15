#include "ChartBackend.h"

#include "core/OfflineDataProcessor.h"
#include "core/OnlineDataProcessor.h"

#include <QtMath>

namespace {
constexpr int RealtimeIntervalMs = 50;
constexpr size_t RealtimeWindowSize = 200;
}

ChartBackend::ChartBackend(QObject *parent) : QObject(parent)
{
    m_realtimeTimer.setInterval(RealtimeIntervalMs);
    connect(&m_realtimeTimer, &QTimer::timeout, this, &ChartBackend::appendRealtimePoint);
}

QList<QPointF> ChartBackend::chartData() const
{
    return m_chartData;
}

bool ChartBackend::onlineMode() const
{
    return m_onlineMode;
}

void ChartBackend::setOfflineMode()
{
    m_realtimeTimer.stop();
    setOnlineModeState(false);
    setProcessor(std::make_unique<OfflineDataProcessor>());
}

void ChartBackend::setOnlineMode()
{
    m_realtimeTimer.stop();
    m_rawData.clear();
    m_chartData.clear();
    m_onlineX = 0.0;
    m_processor = std::make_unique<OnlineDataProcessor>(RealtimeWindowSize);
    setOnlineModeState(true);

    emit chartDataChanged();
    m_realtimeTimer.start();
}

void ChartBackend::generateDummyData()
{
    setOfflineMode();

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
    m_realtimeTimer.stop();
    setOnlineModeState(false);
    m_rawData.clear();
    m_chartData.clear();

    if (m_processor) {
        m_processor->clear();
    }

    emit chartDataChanged();
}

void ChartBackend::appendRealtimePoint()
{
    auto *onlineProcessor = dynamic_cast<OnlineDataProcessor *>(m_processor.get());
    if (!onlineProcessor) {
        m_realtimeTimer.stop();
        setOnlineModeState(false);
        return;
    }

    const double x = m_onlineX++;
    const double wave = qSin(x * 0.15) * 40.0;
    const double detail = qSin(x * 0.043) * 10.0;
    onlineProcessor->appendPoint(QPointF(x, wave + detail));

    updateChartDataFromProcessor();
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
    updateChartDataFromProcessor();
}

void ChartBackend::updateChartDataFromProcessor()
{
    if (!m_processor) {
        m_chartData.clear();
        emit chartDataChanged();
        return;
    }

    const std::vector<QPointF> processedData = m_processor->getProcessedData(m_targetPixelWidth);
    m_chartData = QList<QPointF>(processedData.begin(), processedData.end());

    emit chartDataChanged();
}

void ChartBackend::setOnlineModeState(bool onlineMode)
{
    if (m_onlineMode == onlineMode) {
        return;
    }

    m_onlineMode = onlineMode;
    emit onlineModeChanged();
}
