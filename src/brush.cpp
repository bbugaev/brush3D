#include "brush.h"

Brush::Brush(std::vector<Vertex> vertices, QImage* textureImage) {
    this->vertices = vertices;
    this->textureImage = textureImage;
}

void Brush::paintTriangle(QVector2D point1, QVector2D point2, QVector2D point3) {
    int minX = round(fmin(point1.x(), fmin(point2.x(), point3.x())) * textureImage->width());
    int maxX = round(fmax(point1.x(), fmax(point2.x(), point3.x())) * textureImage->width());

    for (int x = minX; x <= maxX; x++) {
        int minY = round(Geometry::getMinY(point1, point2, point3, x / (1.0 * textureImage->width()))
                         * textureImage->height());
        int maxY = round(Geometry::getMaxY(point1, point2, point3, x / (1.0 * textureImage->width()))
                         * textureImage->height());

        for (int y = minY; y <= maxY; y++) {
            textureImage->setPixelColor(QPoint(x, y), QColor(255, 0, 0));
        }
    }
}

void Brush::paintRound(QVector2D centerOfRound, float radius) {
    float radiusSquare = radius * radius;
    centerOfRound.setX(centerOfRound.x() * textureImage->width());
    centerOfRound.setY(centerOfRound.y() * textureImage->height());

    int minX = fmax(0.0, round(centerOfRound.x() - radius + 0.5));
    int maxX = fmin(textureImage->width() - 1, round(centerOfRound.x() + radius - 0.5));

    for (int x = minX; x <= maxX; x++) {
        float dx = x - centerOfRound.x();
        float dy = sqrt(radiusSquare - dx * dx);
        int minY = fmax(0.0, round(centerOfRound.y() - dy));
        int maxY = fmin(textureImage->height() - 1, round(centerOfRound.y() + dy));

        for (int y = minY; y <= maxY; y++) {
            textureImage->setPixelColor(QPoint(x, y), QColor(255, 0, 0));
        }
    }
}

void Brush::paint(QVector2D point, QMatrix4x4 matrixModelView, QMatrix4x4 matrixProjection) {
    for (int dx = -30; dx <= 30; dx++) {
        for (int dy = -30; dy <= 30; dy++) {
            if (dx * dx + dy * dy <= 30) {
                paintSmallRound(QVector2D(point.x() + dx / 300.0, point.y() + dy / 300.0), matrixModelView, matrixProjection);
            }
        }
    }
}

void Brush::paintSmallRound(QVector2D point, QMatrix4x4 matrixModelView, QMatrix4x4 matrixProjection) {
    float fX = matrixProjection.column(0).x();
    float fY = matrixProjection.column(1).y();
    int intersectedTriangleId = -1;
    float minDistanceToIntersection = 0;
    QVector2D intersectionPoint;
    for (size_t i = 0; i < vertices.size() / 3; i++) {
        QVector3D vector1 = QVector3D(matrixModelView * QVector4D(vertices[3 * i].position(), 1.0));
        QVector3D vector2 = QVector3D(matrixModelView * QVector4D(vertices[3 * i + 1].position(), 1.0));
        QVector3D vector3 = QVector3D(matrixModelView * QVector4D(vertices[3 * i + 2].position(), 1.0));
        QVector3D rayVector = QVector3D(point.x() / fX, point.y() / fY, -1.0);

        QVector3D *currentIntersectionPoint = Geometry::intersectRayAndTriangle(vector1, vector2, vector3, rayVector);

        if (currentIntersectionPoint != nullptr) {
            float currentDistanceToIntersection = QVector3D::dotProduct(*currentIntersectionPoint, *currentIntersectionPoint);
            if (intersectedTriangleId == -1 || currentDistanceToIntersection < minDistanceToIntersection) {
                intersectedTriangleId = i;
                minDistanceToIntersection = currentDistanceToIntersection;
                intersectionPoint = Geometry::getPointInUVCoordinates(vector2 - vector1,
                                                                      vector3 - vector1,
                                                                      *currentIntersectionPoint - vector1,
                                                                      vertices[3 * i].uv(),
                                                                      vertices[3 * i + 1].uv() - vertices[3 * i].uv(),
                                                                      vertices[3 * i + 2].uv() - vertices[3 * i].uv());
            }
        }
    }

    if (intersectedTriangleId != -1) {
        /*
        paintTriangle(vertices[3 * intersectedTriangleId].uv(),
                             vertices[3 * intersectedTriangleId + 1].uv(),
                             vertices[3 * intersectedTriangleId + 2].uv());
        */
        paintRound(intersectionPoint, radius / 5);
    }
}

void Brush::setRadius(double radius) {
    this->radius = radius;
}

QImage *Brush::getTextureImage() {
    return textureImage;
}
