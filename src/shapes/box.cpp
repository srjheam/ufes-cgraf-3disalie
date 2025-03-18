#include "shapes/box.h"

#include <GL/gl.h>

class Box::Impl {
public:
    material mat_;
    float width_;
    float height_;
    float depth_;

    Impl(
        material mat,
        float width,
        float height,
        float depth
    ) : mat_(mat), width_(width), height_(height), depth_(depth) { }
};

Box::Box(
    material mat,
    float width,
    float height,
    float depth,
    bool invert_normal
) : Shape(invert_normal),
    pimpl(std::make_unique<Impl>(
        mat,
        width,
        height,
        depth
    )) { }

Box::Box(Box&&) noexcept = default;
Box& Box::operator=(Box&&) noexcept = default;

Box::~Box() = default;

v3f Box::centroid() const {
    return {
        0,
        0,
        0
    };
}

void Box::draw(bool show_axes) const {
    glPushMatrix();
        glScalef(pimpl->width_, pimpl->height_, pimpl->depth_);
    
        Shape::draw(show_axes);
    
        Shape::set_material(pimpl->mat_);

        glBegin(GL_QUADS);
            // Z == .5 face
            glNormal3f(0.0f, 0.0f, 1.0f * this->invert_normal);
            glVertex3f(-0.5f, -0.5f, 0.5f);
            glVertex3f(0.5f, -0.5f, 0.5f);
            glVertex3f(0.5f, 0.5f, 0.5f);
            glVertex3f(-0.5f, 0.5f, 0.5f);

            // Z == -.5 face
            glNormal3f(0.0f, 0.0f, -1.0f * this->invert_normal);
            glVertex3f(-0.5f, -0.5f, -0.5f);
            glVertex3f(-0.5f, 0.5f, -0.5f);
            glVertex3f(0.5f, 0.5f, -0.5f);
            glVertex3f(0.5f, -0.5f, -0.5f);

            // X == .5 face
            glNormal3f(1.0f * this->invert_normal, 0.0f, 0.0f);
            glVertex3f(0.5f, -0.5f, -0.5f);
            glVertex3f(0.5f, 0.5f, -0.5f);
            glVertex3f(0.5f, 0.5f, 0.5f);
            glVertex3f(0.5f, -0.5f, 0.5f);

            // X == -.5 face
            glNormal3f(-1.0f * this->invert_normal, 0.0f, 0.0f);
            glVertex3f(-0.5f, -0.5f, -0.5f);
            glVertex3f(-0.5f, -0.5f, 0.5f);
            glVertex3f(-0.5f, 0.5f, 0.5f);
            glVertex3f(-0.5f, 0.5f, -0.5f);

            // Y == .5 face
            glNormal3f(0.0f, 1.0f * this->invert_normal, 0.0f);
            glVertex3f(-0.5f, 0.5f, -0.5f);
            glVertex3f(-0.5f, 0.5f, 0.5f);
            glVertex3f(0.5f, 0.5f, 0.5f);
            glVertex3f(0.5f, 0.5f, -0.5f);

            // Y == -.5 face
            glNormal3f(0.0f, -1.0f * this->invert_normal, 0.0f);
            glVertex3f(-0.5f, -0.5f, -0.5f);
            glVertex3f(0.5f, -0.5f, -0.5f);
            glVertex3f(0.5f, -0.5f, 0.5f);
            glVertex3f(-0.5f, -0.5f, 0.5f);
        glEnd();
    glPopMatrix();
}
