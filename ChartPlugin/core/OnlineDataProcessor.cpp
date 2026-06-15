#include "OnlineDataProcessor.h"

#include "LttbAlgorithm.h"

#include <limits>

void OnlineDataProcessor::loadData(const std::vector<QPointF>& rawData)
{
    m_data = rawData;
}

void OnlineDataProcessor::appendPoint(const QPointF& point)
{
    m_data.push_back(point);
}

std::vector<QPointF> OnlineDataProcessor::getProcessedData(int pixelWidth)
{
    return LttbAlgorithm::downsample(m_data, pixelWidth);
}

ChartBounds OnlineDataProcessor::getBounds() const
{
    if (m_data.empty()) {
        return {0.0, 0.0, 0.0, 0.0};
    }

    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();

    for (const auto& point : m_data) {
        if (point.x() < minX) minX = point.x();
        if (point.x() > maxX) maxX = point.x();
        if (point.y() < minY) minY = point.y();
        if (point.y() > maxY) maxY = point.y();
    }

    return {minX, maxX, minY, maxY};
}

void OnlineDataProcessor::clear()
{
    m_data.clear();
}
