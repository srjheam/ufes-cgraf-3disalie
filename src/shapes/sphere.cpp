#include "shapes/sphere.h"

#include <GL/gl.h>
#include <GL/glu.h>

class Sphere::Impl {
public:
    material mat_;
    float radius_;

    Impl(
        material mat,
        float radius
    ) : mat_(mat), radius_(radius) { }
};

Sphere::Sphere(
    material mat,
    float radius,
    bool invert_normal
) : Shape(invert_normal),
    pimpl(std::make_unique<Impl>(
        mat,
        radius
    )) { }

Sphere::Sphere(Sphere&&) noexcept = default;
Sphere& Sphere::operator=(Sphere&&) noexcept = default;

Sphere::~Sphere() = default;

float Sphere::radius() const {
    return pimpl->radius_;
}

v3f Sphere::centroid() const {
    return { 0.0f, 0.0f, 0.0f };
}

void Sphere::draw(bool show_axes) const {
    Shape::draw(show_axes);
    
    Shape::set_material(pimpl->mat_);

    GLUquadric* quadric = gluNewQuadric();
        gluQuadricDrawStyle(quadric, GLU_FILL);
        gluQuadricNormals(quadric, GLU_SMOOTH);
        gluQuadricTexture(quadric, GL_TRUE);
        gluQuadricOrientation(quadric, this->invert_normal < 0 ? GLU_INSIDE : GLU_OUTSIDE);
        gluSphere(quadric, pimpl->radius_, 32, 32);
    gluDeleteQuadric(quadric);
}
