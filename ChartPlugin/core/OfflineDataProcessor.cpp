#include "OfflineDataProcessor.h"
#include "LttbAlgorithm.h"
#include <limits>

void OfflineDataProcessor::loadData(const std::vector<QPointF>& rawData) {
    m_rawData = rawData;
    calculateBounds(); // Tự động cập nhật Min/Max ngay khi có dữ liệu mới
}

std::vector<QPointF> OfflineDataProcessor::getProcessedData(int pixelWidth) {
    return LttbAlgorithm::downsample(m_rawData, pixelWidth);
}

ChartBounds OfflineDataProcessor::getBounds() const {
    return m_bounds;
}

void OfflineDataProcessor::clear() {
    m_rawData.clear();
    m_bounds = {0.0, 0.0, 0.0, 0.0};
}

void OfflineDataProcessor::calculateBounds() {
    // Tránh lỗi khi mảng rỗng
    if (m_rawData.empty()) {
        m_bounds = {0.0, 0.0, 0.0, 0.0};
        return;
    }

    // Khởi tạo giá trị Min/Max ban đầu bằng các giới hạn của kiểu double
    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();

    // Duyệt qua toàn bộ mảng để tìm các điểm biên (C++11 range-based for loop)
    for (const auto& point : m_rawData) {
        if (point.x() < minX) minX = point.x();
        if (point.x() > maxX) maxX = point.x();
        if (point.y() < minY) minY = point.y();
        if (point.y() > maxY) maxY = point.y();
    }

    m_bounds = {minX, maxX, minY, maxY};
}
