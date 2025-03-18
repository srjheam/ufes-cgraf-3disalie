#ifndef SHAPE_H
#define SHAPE_H

#include <memory>
#include "vector3.h"
#include "anemics/v3f.h"
#include "anemics/material.h"

class Shape {
    protected:
        int8_t invert_normal;

    public:
        Shape(bool invert_normal = false);

        Shape(const Shape &) = delete;
        Shape &operator=(const Shape &) = delete;

        Shape(Shape &&) noexcept;
        Shape &operator=(Shape &&) noexcept = default;

        void set_material(material m) const;

        virtual ~Shape();

        virtual v3f centroid() const = 0;
        virtual void draw(bool show_axes = false) const;
};

#endif
