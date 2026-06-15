#ifndef ONLINEDATAPROCESSOR_H
#define ONLINEDATAPROCESSOR_H

#include "IDataProcessor.h"

class OnlineDataProcessor : public IDataProcessor {
public:
    OnlineDataProcessor() = default;
    ~OnlineDataProcessor() override = default;

    void loadData(const std::vector<QPointF>& rawData) override;
    std::vector<QPointF> getProcessedData(int pixelWidth) override;
    ChartBounds getBounds() const override;
    void clear() override;

    void appendPoint(const QPointF& point);

private:
    std::vector<QPointF> m_data;
};

#endif // ONLINEDATAPROCESSOR_H
