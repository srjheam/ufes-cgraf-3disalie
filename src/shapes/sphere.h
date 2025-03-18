#ifndef SPHERE_H
#define SPHERE_H

#include "shape.h"
#include "anemics/material.h"

class Sphere : public Shape {
    private:
        // pimpl idiom
        class Impl;
        std::unique_ptr<Impl> pimpl;

    public:
        Sphere(
            material mat,
            float radius,
            bool invert_normal = false
        );

        Sphere(const Sphere&) = delete;
        Sphere& operator=(const Sphere&) = delete;
        
        Sphere(Sphere&&) noexcept;
        Sphere& operator=(Sphere&&) noexcept;
        
        ~Sphere();

        float radius() const;

        v3f centroid() const override;
        void draw(bool show_axes = false) const override;
};

#endif
