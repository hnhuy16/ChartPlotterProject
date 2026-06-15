#ifndef IDATAPROCESSOR_H
#define IDATAPROCESSOR_H

#include <vector>
#include <QPointF>

// Dùng struct để nhóm dữ liệu trả về cho tiện
struct ChartBounds {
    double minX, maxX;
    double minY, maxY;
};

class IDataProcessor {
public:
    // Virtual destructor là bắt buộc trong C++ OOP để tránh memory leak khi đa hình
    virtual ~IDataProcessor() = default;

    // Nạp dữ liệu thô vào bộ vi xử lý
    virtual void loadData(const std::vector<QPointF>& rawData) = 0;

    // Trả về tập dữ liệu đã được giảm mẫu (Downsampled) để vẽ
    virtual std::vector<QPointF> getProcessedData(int pixelWidth) = 0;

    // Lấy khoảng min-max để màn hình tự động scale trục X, Y
    virtual ChartBounds getBounds() const = 0;

    // Xóa bộ nhớ
    virtual void clear() = 0;
};

#endif // IDATAPROCESSOR_H