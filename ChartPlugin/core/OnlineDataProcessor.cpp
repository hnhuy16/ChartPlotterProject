#include "OnlineDataProcessor.h"
#include "LttbAlgorithm.h"
#include <limits>

// Khởi tạo giới hạn số điểm tối đa trên màn hình
OnlineDataProcessor::OnlineDataProcessor(size_t maxWindowSize)
    : m_maxWindowSize(maxWindowSize) {}

void OnlineDataProcessor::loadData(const std::vector<QPointF>& rawData) {
    clear();
    // Nạp lần lượt từng điểm vào hàng đợi
    for (const auto& pt : rawData) {
        appendPoint(pt);
    }
}

void OnlineDataProcessor::appendPoint(const QPointF& point) {
    m_dataQueue.push_back(point);

    // Nếu số lượng điểm vượt quá khung cửa sổ (Window Size), xóa điểm cũ nhất ở đầu
    if (m_dataQueue.size() > m_maxWindowSize) {
        m_dataQueue.pop_front();
    }
}

std::vector<QPointF> OnlineDataProcessor::getProcessedData(int pixelWidth) {
    const std::vector<QPointF> data(m_dataQueue.begin(), m_dataQueue.end());
    return LttbAlgorithm::downsample(data, pixelWidth);
}

ChartBounds OnlineDataProcessor::getBounds() const {
    if (m_dataQueue.empty()) {
        return {0.0, 0.0, 0.0, 0.0};
    }

    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();

    // Tính Min/Max dựa trên các điểm ĐANG CÓ trong hàng đợi (khung hình hiện tại)
    for (const auto& point : m_dataQueue) {
        if (point.x() < minX) minX = point.x();
        if (point.x() > maxX) maxX = point.x();
        if (point.y() < minY) minY = point.y();
        if (point.y() > maxY) maxY = point.y();
    }

    return {minX, maxX, minY, maxY};
}

void OnlineDataProcessor::clear() {
    m_dataQueue.clear();
}
