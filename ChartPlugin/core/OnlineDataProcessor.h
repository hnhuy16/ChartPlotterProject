#ifndef ONLINEDATAPROCESSOR_H
#define ONLINEDATAPROCESSOR_H

#include "IDataProcessor.h"
#include <deque>

class OnlineDataProcessor : public IDataProcessor {
public:
    // Giới hạn số lượng điểm giữ lại trong RAM (tránh tràn bộ nhớ)
    explicit OnlineDataProcessor(size_t maxWindowSize = 200);
    ~OnlineDataProcessor() override = default;

    void loadData(const std::vector<QPointF>& rawData) override;
    std::vector<QPointF> getProcessedData(int pixelWidth) override;
    ChartBounds getBounds() const override;
    void clear() override;

    // Thêm hàm nạp từng điểm một (đặc thù của stream)
    void appendPoint(const QPointF& point);

private:
    std::deque<QPointF> m_dataQueue; // Dùng deque để dễ dàng đẩy vào/rút ra 2 đầu
    size_t m_maxWindowSize;
};

#endif // ONLINEDATAPROCESSOR_H
