#ifndef LTTBALGORITHM_H
#define LTTBALGORITHM_H

#include <QPointF>
#include <vector>

class LttbAlgorithm {
public:
    static std::vector<QPointF> downsample(const std::vector<QPointF>& data, int threshold);
};

#endif // LTTBALGORITHM_H
