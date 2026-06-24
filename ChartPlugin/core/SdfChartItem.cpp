#include "SdfChartItem.h"

#include <QMatrix4x4>
#include <QSGGeometryNode>
#include <QSGMaterial>
#include <QSGMaterialShader>
#include <QVector2D>

#include <algorithm>
#include <cmath>
#include <cstring>

namespace {

struct LineVertex
{
    float x;
    float y;
    float distance;
};

const QSGGeometry::AttributeSet &lineAttributes()
{
    static const QSGGeometry::Attribute attributes[] = {
        QSGGeometry::Attribute::create(0, 2, QSGGeometry::FloatType, true),
        QSGGeometry::Attribute::create(1, 1, QSGGeometry::FloatType)
    };
    static const QSGGeometry::AttributeSet attributeSet = {
        2, sizeof(LineVertex), attributes
    };
    return attributeSet;
}

class SdfLineMaterialShader final : public QSGMaterialShader
{
public:
    SdfLineMaterialShader()
    {
        setShaderFileName(VertexStage,
                          QStringLiteral(":/ChartPlotter/shaders/line.vert.qsb"));
        setShaderFileName(FragmentStage,
                          QStringLiteral(":/ChartPlotter/shaders/line.frag.qsb"));
    }

    bool updateUniformData(RenderState &state, QSGMaterial *newMaterial,
                           QSGMaterial *) override;
};

class SdfLineMaterial final : public QSGMaterial
{
public:
    SdfLineMaterial()
    {
        setFlag(Blending);
    }

    QSGMaterialType *type() const override
    {
        static QSGMaterialType type;
        return &type;
    }

    QSGMaterialShader *createShader(QSGRendererInterface::RenderMode) const override
    {
        return new SdfLineMaterialShader;
    }

    int compare(const QSGMaterial *other) const override
    {
        const auto *otherLine = static_cast<const SdfLineMaterial *>(other);
        const QRgba64 rgba = color.rgba64();
        const QRgba64 otherRgba = otherLine->color.rgba64();
        return std::memcmp(&rgba, &otherRgba, sizeof(QRgba64));
    }

    QColor color;
};

bool SdfLineMaterialShader::updateUniformData(RenderState &state,
                                              QSGMaterial *newMaterial,
                                              QSGMaterial *)
{
    QByteArray *uniforms = state.uniformData();
    Q_ASSERT(uniforms->size() >= 84);

    if (state.isMatrixDirty()) {
        const QMatrix4x4 matrix = state.combinedMatrix();
        std::memcpy(uniforms->data(), matrix.constData(), 64);
    }

    const auto *material = static_cast<SdfLineMaterial *>(newMaterial);
    float color[4];
    material->color.getRgbF(&color[0], &color[1], &color[2], &color[3]);
    color[0] *= color[3];
    color[1] *= color[3];
    color[2] *= color[3];
    std::memcpy(uniforms->data() + 64, color, 16);

    if (state.isOpacityDirty()) {
        const float opacity = state.opacity();
        std::memcpy(uniforms->data() + 80, &opacity, 4);
    }

    return true;
}

QVector2D normalizedOrZero(const QVector2D &vector)
{
    const float length = vector.length();
    return length > 0.0001f ? vector / length : QVector2D();
}

} // namespace

SdfChartItem::SdfChartItem(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents, true);
}

QList<QPointF> SdfChartItem::chartData() const { return m_chartData; }
QColor SdfChartItem::lineColor() const { return m_lineColor; }
qreal SdfChartItem::lineWidth() const { return m_lineWidth; }
qreal SdfChartItem::minX() const { return m_minX; }
qreal SdfChartItem::maxX() const { return m_maxX; }
qreal SdfChartItem::minY() const { return m_minY; }
qreal SdfChartItem::maxY() const { return m_maxY; }

void SdfChartItem::setChartData(const QList<QPointF> &chartData)
{
    if (m_chartData == chartData)
        return;
    m_chartData = chartData;
    emit chartDataChanged();
    update();
}

void SdfChartItem::setLineColor(const QColor &lineColor)
{
    if (m_lineColor == lineColor)
        return;
    m_lineColor = lineColor;
    emit lineColorChanged();
    update();
}

void SdfChartItem::setLineWidth(qreal lineWidth)
{
    lineWidth = std::max<qreal>(0.0, lineWidth);
    if (qFuzzyCompare(m_lineWidth, lineWidth))
        return;
    m_lineWidth = lineWidth;
    emit lineWidthChanged();
    update();
}

#define DEFINE_RANGE_SETTER(Name, Member, Signal) \
    void SdfChartItem::set##Name(qreal value) \
    { \
        if (qFuzzyCompare(Member, value)) \
            return; \
        Member = value; \
        emit Signal(); \
        update(); \
    }

DEFINE_RANGE_SETTER(MinX, m_minX, minXChanged)
DEFINE_RANGE_SETTER(MaxX, m_maxX, maxXChanged)
DEFINE_RANGE_SETTER(MinY, m_minY, minYChanged)
DEFINE_RANGE_SETTER(MaxY, m_maxY, maxYChanged)

#undef DEFINE_RANGE_SETTER

QSGNode *SdfChartItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    const qreal xRange = m_maxX - m_minX;
    const qreal yRange = m_maxY - m_minY;
    if (width() <= 0.0 || height() <= 0.0 || m_lineWidth <= 0.0
            || qFuzzyIsNull(xRange) || qFuzzyIsNull(yRange)) {
        delete oldNode;
        return nullptr;
    }

    QList<QVector2D> points;
    points.reserve(m_chartData.size());
    for (const QPointF &point : m_chartData) {
        if (!std::isfinite(point.x()) || !std::isfinite(point.y()))
            continue;

        const float x = float((point.x() - m_minX) / xRange * width());
        const float y = float(height() - (point.y() - m_minY) / yRange * height());
        const QVector2D mappedPoint(x, y);
        if (points.isEmpty() || (mappedPoint - points.constLast()).lengthSquared() > 0.0001f)
            points.append(mappedPoint);
    }

    if (points.size() < 2) {
        delete oldNode;
        return nullptr;
    }

    auto *node = static_cast<QSGGeometryNode *>(oldNode);
    if (!node) {
        node = new QSGGeometryNode;
        auto *geometry = new QSGGeometry(lineAttributes(), 0);
        geometry->setDrawingMode(QSGGeometry::DrawTriangleStrip);
        geometry->setVertexDataPattern(QSGGeometry::DynamicPattern);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);

        auto *material = new SdfLineMaterial;
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
    }

    auto *geometry = node->geometry();
    geometry->allocate(points.size() * 2);
    auto *vertices = static_cast<LineVertex *>(geometry->vertexData());
    const float halfWidth = float(m_lineWidth * 0.5);

    for (qsizetype i = 0; i < points.size(); ++i) {
        QVector2D offset;
        if (i == 0) {
            const QVector2D tangent = normalizedOrZero(points[1] - points[0]);
            offset = QVector2D(-tangent.y(), tangent.x()) * halfWidth;
        } else if (i == points.size() - 1) {
            const QVector2D tangent = normalizedOrZero(points[i] - points[i - 1]);
            offset = QVector2D(-tangent.y(), tangent.x()) * halfWidth;
        } else {
            const QVector2D previousTangent = normalizedOrZero(points[i] - points[i - 1]);
            const QVector2D nextTangent = normalizedOrZero(points[i + 1] - points[i]);
            const QVector2D previousNormal(-previousTangent.y(), previousTangent.x());
            const QVector2D nextNormal(-nextTangent.y(), nextTangent.x());
            const QVector2D miter = normalizedOrZero(previousNormal + nextNormal);
            const float denominator = QVector2D::dotProduct(miter, nextNormal);
            const float miterLength = std::abs(denominator) > 0.1f
                    ? std::clamp(halfWidth / denominator,
                                 -halfWidth * 4.0f, halfWidth * 4.0f)
                    : halfWidth;
            offset = miter * miterLength;
        }

        const QVector2D left = points[i] - offset;
        const QVector2D right = points[i] + offset;
        vertices[i * 2] = { left.x(), left.y(), -1.0f };
        vertices[i * 2 + 1] = { right.x(), right.y(), 1.0f };
    }

    auto *material = static_cast<SdfLineMaterial *>(node->material());
    material->color = m_lineColor;
    node->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
    return node;
}
