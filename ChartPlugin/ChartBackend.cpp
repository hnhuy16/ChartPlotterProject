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

double ChartBackend::minY() const
{
    return m_minY;
}

double ChartBackend::maxY() const
{
    return m_maxY;
}

void ChartBackend::setMinX(double minX)
{
    setViewport(minX, m_maxX, m_minY, m_maxY);
}

void ChartBackend::setMaxX(double maxX)
{
    setViewport(m_minX, maxX, m_minY, m_maxY);
}

void ChartBackend::setMinY(double minY)
{
    setViewport(m_minX, m_maxX, minY, m_maxY);
}

void ChartBackend::setMaxY(double maxY)
{
    setViewport(m_minX, m_maxX, m_minY, maxY);
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
    setXViewport(0.0, InitialRealtimeViewportWidth);
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
    setViewport(0.0, 1999.0, -48.0, 48.0);

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
    setXViewport(minX, maxX);

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

void ChartBackend::setViewport(double minX, double maxX, double minY, double maxY)
{
    if (minX >= maxX || minY >= maxY) {
        return;
    }

    const bool minXHasChanged = !qFuzzyCompare(m_minX, minX);
    const bool maxXHasChanged = !qFuzzyCompare(m_maxX, maxX);
    const bool minYHasChanged = !qFuzzyCompare(m_minY, minY);
    const bool maxYHasChanged = !qFuzzyCompare(m_maxY, maxY);

    if (!minXHasChanged && !maxXHasChanged && !minYHasChanged && !maxYHasChanged) {
        return;
    }

    m_minX = minX;
    m_maxX = maxX;
    m_minY = minY;
    m_maxY = maxY;

    if (minXHasChanged) {
        emit minXChanged();
    }
    if (maxXHasChanged) {
        emit maxXChanged();
    }
    if (minYHasChanged) {
        emit minYChanged();
    }
    if (maxYHasChanged) {
        emit maxYChanged();
    }

    emit viewportChanged();
}

void ChartBackend::resetBounds()
{
    if (m_chartData.isEmpty()) {
        return;
    }

    double minX = m_chartData.first().x();
    double maxX = minX;
    double minY = m_chartData.first().y();
    double maxY = minY;

    for (const QPointF &point : m_chartData) {
        minX = qMin(minX, point.x());
        maxX = qMax(maxX, point.x());
        minY = qMin(minY, point.y());
        maxY = qMax(maxY, point.y());
    }

    m_minX = minX;
    m_maxX = maxX;
    m_minY = minY;
    m_maxY = maxY;

    emit minXChanged();
    emit maxXChanged();
    emit minYChanged();
    emit maxYChanged();
    emit viewportChanged();
}

void ChartBackend::setXViewport(double minX, double maxX)
{
    setViewport(minX, maxX, m_minY, m_maxY);
}
