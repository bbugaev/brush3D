#include "pixelsfastbrush.h"

PixelsFastBrush::PixelsFastBrush(const ObjectModel& objectModel, const TextureStorage& textureStorage) : AbstractBrush(objectModel, textureStorage) {
    uint32_t w = textureStorage.getWidth();
    uint32_t h = textureStorage.getHeight();
    coordinatesFromUv_.resize(w * h);
    pixelsUvOfTriangle_.resize(objectModel.getFacesNumber());

    for (uint32_t faceId = 0; faceId < objectModel.getFacesNumber(); faceId++) {
        Face face(objectModel, faceId);

        int minX = fmax(0, round(fmin(face.getUv(0).x, fmin(face.getUv(1).x, face.getUv(2).x)) * textureStorage.getWidth()));
        int maxX = fmin(textureStorage.getWidth() - 1, round(fmax(face.getUv(0).x, fmax(face.getUv(1).x, face.getUv(2).x))
                                                   * textureStorage.getWidth()));

        for (int x = minX; x <= maxX; x++) {
            float xUv = x / (1.0 * textureStorage.getWidth());
            int minY = fmax(0, round(Geometry::getMinY(face.getUvs(), xUv) * textureStorage.getHeight()));
            int maxY = fmin(textureStorage.getHeight() - 1, round(Geometry::getMaxY(face.getUvs(), xUv) * textureStorage.getHeight()));

            for (int y = minY; y <= maxY; y++) {
                float yUv = y / (1.0 * textureStorage.getHeight());
                glm::vec3 point = Geometry::getPointFromUVCoordinates(face.getUvs(), face.getPositions(), glm::vec2(xUv, yUv));
                coordinatesFromUv_[x * h + y] = point;
                pixelsUvOfTriangle_[faceId].push_back(glm::u32vec2(x, y));
            }
        }
    }
}

BrushStroke PixelsFastBrush::paint(const glm::i32vec2& point, const glm::mat4x4& matrixModelView, const glm::mat4x4& projection,
                                   const IdsStorage& idsStorage) {
    BrushStroke diff;
    auto ids = getIntersectedTrianglesIds(point, idsStorage);
    for (size_t id : ids) {
        paintTriangle(id, matrixModelView, projection, point, idsStorage, diff);
    }
    return diff;
}

void PixelsFastBrush::paintTriangle(size_t id, const glm::mat4x4& matrixModelView, const glm::mat4x4& projection, const glm::i32vec2& brushCenter,
                                    const IdsStorage& idsStorage, BrushStroke &diff) {
    for (glm::u32vec2 pixel : pixelsUvOfTriangle_[id]) {
        glm::vec3 point(matrixModelView * glm::vec4(coordinatesFromUv_[pixel.x * textureStorage_.getHeight() + pixel.y], 1.0));
        glm::i32vec2 screenPoint(Geometry::toScreenCoordinates(point, projection, glm::u32vec2(idsStorage.getWidth(), idsStorage.getHeight())));

        if (screenPoint.x >= 0 && screenPoint.y >= 0 && screenPoint.x < (int32_t) idsStorage.getWidth() && screenPoint.y < (int32_t) idsStorage.getHeight()
                && glm::length(glm::vec2(brushCenter - screenPoint)) < getRadius()
                && ((idsStorage.hasNeighbourWithId(screenPoint, id)) || (objectModel_.areAdjacentFaces(id, idsStorage.getId(screenPoint))))) {
            diff.add(ColorChange(pixel, textureStorage_.getColor(pixel), getColor()));
            textureStorage_.setColor(pixel, getColor());
        }
    }
}

std::unordered_set<size_t> PixelsFastBrush::getIntersectedTrianglesIds(const glm::i32vec2& brushCenter, const IdsStorage& idsStorage) {
    std::unordered_set<size_t> ids;
    for (int dx = -getRadius(); dx <= getRadius(); dx++) {
        for (int dy = -getRadius(); dy <= getRadius(); dy++) {
            if (dx * dx + dy * dy <= getRadius() * getRadius()) {
                glm::i32vec2 point(brushCenter.x + dx, brushCenter.y + dy);
                if (point.x >= 0 && point.y >= 0 && point.x < (int32_t) idsStorage.getWidth() && point.y < (int32_t) idsStorage.getHeight()
                        && idsStorage.getId(point.x, point.y) < objectModel_.getFacesNumber()) {
                    ids.insert(idsStorage.getId(point.x, point.y));
                }
            }
        }
    }
    return ids;
}
