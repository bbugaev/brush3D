#include "include/abstractbrush.hpp"

#include "details/geometry.hpp"
#include "details/face.hpp"

AbstractBrush::AbstractBrush(const ObjectModel &objectModel, TextureStorage &textureStorage) :
        objectModel_(objectModel), textureStorage_(textureStorage) {
}

AbstractBrush::~AbstractBrush() {
}

void AbstractBrush::setRadius(double radius) {
    this->radius_ = radius;
}

void AbstractBrush::setColor(const glm::u8vec3 &color) {
    this->color_ = color;
}

double AbstractBrush::getRadius() const {
    return radius_;
}

glm::u8vec3 AbstractBrush::getColor() const {
    return color_;
}

bool AbstractBrush::isInsideBrush(const glm::i32vec2 &screenPoint, const glm::i32vec2 &brushCenter) const {
    return glm::length(glm::vec2(brushCenter - screenPoint)) < getRadius();
}

BrushStroke AbstractBrush::paint(const glm::i32vec2 &previousPoint, const glm::i32vec2 &lastPoint,
                                 const glm::mat4x4 &matrixModelView, const glm::mat4x4 &matrixProjection,
                                 const IdsStorage &idsStorage) {
    BrushStroke diff;

    glm::vec2 d(previousPoint - lastPoint);
    int cntRounds = glm::length(d);
    for (int i = 0; i < cntRounds; ++i) {
        glm::i32vec2 currentPoint = previousPoint + (lastPoint - previousPoint) * i / cntRounds;
        auto currentPointDiff = paint(currentPoint, matrixModelView, matrixProjection, idsStorage);
        diff.add(currentPointDiff);
    }

    return diff;
}
