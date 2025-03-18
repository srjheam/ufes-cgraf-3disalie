#ifndef BOX_H
#define BOX_H

#include "shape.h"
#include "anemics/material.h"

class Box : public Shape {
    private:
        // pimpl idiom
        class Impl;
        std::unique_ptr<Impl> pimpl;

    public:
        Box(
            material mat,
            float width,
            float height,
            float depth,
            bool invert_normal = false
        );

        Box(const Box&) = delete;
        Box& operator=(const Box&) = delete;
        
        Box(Box&&) noexcept;
        Box& operator=(Box&&) noexcept;
        
        ~Box();
        
        v3f centroid() const override;
        void draw(bool show_axes = false) const override;
};

#endif
