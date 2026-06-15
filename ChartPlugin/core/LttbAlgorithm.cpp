#include "LttbAlgorithm.h"

#include <cmath>

std::vector<QPointF> LttbAlgorithm::downsample(const std::vector<QPointF>& data, int threshold) {
    const auto dataSize = data.size();

    if (threshold <= 0 || data.empty()) {
        return {};
    }

    const auto targetSize = static_cast<std::size_t>(threshold);

    if (targetSize >= dataSize) {
        return data;
    }

    if (targetSize == 1) {
        return {data.front()};
    }

    if (targetSize == 2) {
        return {data.front(), data.back()};
    }

    std::vector<QPointF> sampled;
    sampled.reserve(targetSize);
    sampled.push_back(data.front());

    const double bucketSize = static_cast<double>(dataSize - 2) / static_cast<double>(targetSize - 2);
    std::size_t currentIndex = 0;

    for (std::size_t i = 0; i < targetSize - 2; ++i) {
        const auto averageRangeStart = static_cast<std::size_t>(std::floor((i + 1) * bucketSize)) + 1;
        auto averageRangeEnd = static_cast<std::size_t>(std::floor((i + 2) * bucketSize)) + 1;
        if (averageRangeEnd > dataSize) {
            averageRangeEnd = dataSize;
        }

        double averageX = 0.0;
        double averageY = 0.0;
        const auto averageRangeLength = averageRangeEnd - averageRangeStart;

        for (std::size_t j = averageRangeStart; j < averageRangeEnd; ++j) {
            averageX += data[j].x();
            averageY += data[j].y();
        }

        if (averageRangeLength > 0) {
            averageX /= static_cast<double>(averageRangeLength);
            averageY /= static_cast<double>(averageRangeLength);
        } else {
            averageX = data.back().x();
            averageY = data.back().y();
        }

        const auto rangeStart = static_cast<std::size_t>(std::floor(i * bucketSize)) + 1;
        auto rangeEnd = static_cast<std::size_t>(std::floor((i + 1) * bucketSize)) + 1;
        if (rangeEnd > dataSize - 1) {
            rangeEnd = dataSize - 1;
        }

        double maxArea = -1.0;
        std::size_t maxAreaIndex = rangeStart;
        const auto& currentPoint = data[currentIndex];

        for (std::size_t j = rangeStart; j < rangeEnd; ++j) {
            const double area = std::abs(
                (currentPoint.x() - averageX) * (data[j].y() - currentPoint.y()) -
                (currentPoint.x() - data[j].x()) * (averageY - currentPoint.y()));

            if (area > maxArea) {
                maxArea = area;
                maxAreaIndex = j;
            }
        }

        sampled.push_back(data[maxAreaIndex]);
        currentIndex = maxAreaIndex;
    }

    sampled.push_back(data.back());
    return sampled;
}
