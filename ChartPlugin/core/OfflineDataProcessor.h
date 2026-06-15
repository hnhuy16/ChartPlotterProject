#ifndef OFFLINEDATAPROCESSOR_H
#define OFFLINEDATAPROCESSOR_H

#include "IDataProcessor.h"

class OfflineDataProcessor : public IDataProcessor {
public:
    OfflineDataProcessor() = default;
    ~OfflineDataProcessor() override = default;

    void loadData(const std::vector<QPointF>& rawData) override;
    std::vector<QPointF> getProcessedData(int pixelWidth) override;
    ChartBounds getBounds() const override;
    void clear() override;

private:
    std::vector<QPointF> m_rawData;
    ChartBounds m_bounds;

    // Hàm private để tự động tìm min max mỗi khi có dữ liệu mới
    void calculateBounds();
};

#endif // OFFLINEDATAPROCESSOR_H