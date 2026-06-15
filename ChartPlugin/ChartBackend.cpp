#include "ChartBackend.h"

#include "core/OfflineDataProcessor.h"
#include "core/OnlineDataProcessor.h"

#include <QtMath>

namespace {
constexpr int RealtimeIntervalMs = 50;
constexpr double InitialRealtimeViewportWidth = 199.0;
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

double ChartBackend::minX() const
{
    return m_minX;
}

double ChartBackend::maxX() const
{
    return m_maxX;
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
    setViewport(0.0, InitialRealtimeViewportWidth);
    m_processor = std::make_unique<OnlineDataProcessor>();
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

    const double viewportWidth = m_maxX - m_minX;
    const double maxX = x;
    const double minX = maxX - viewportWidth;
    setViewport(minX, maxX);

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

void ChartBackend::setViewport(double minX, double maxX)
{
    if (qFuzzyCompare(m_minX, minX) && qFuzzyCompare(m_maxX, maxX)) {
        return;
    }

    m_minX = minX;
    m_maxX = maxX;
    emit viewportChanged();
}
