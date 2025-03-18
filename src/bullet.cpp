#include "bullet.h"
#include <GL/gl.h>

#include "shapes/box.h"

class Bullet::Impl {
public:
    Vector3 direction;
    GLdouble angle;
    
    Impl(Vector3 dir) 
        : direction(dir) {
            angle = direction.angle();
        }
};

Bullet::Bullet(
        GLfloat o_x,
        GLfloat o_y,
        GLfloat o_z,
        GLfloat height,
        GLfloat width,
        GLfloat depth,
        Vector3 direction)
            : Entity(
                o_x,
                o_y,
                o_z,
                height,
                width,
                depth,
                height * 15,
                height * 8
                ),
            pimpl(new Impl(direction)) { }

Bullet::Bullet(Bullet&&) noexcept = default;

Bullet::~Bullet() = default;

const Vector3 &Bullet::direction() const {
    return pimpl->direction;
}

void Bullet::draw(bool draw_axes) const {
    const static Box box(
        {
            { 0.0, 0.0, 0.0, 1.0 },
            { 1.0, 0.0, 0.0, 1.0 },
            { 1.0, 0.0, 0.0, 1.0 },
            { 1.0, 1.0, 1.0, 1.0 },
            { 128 },
        },
        width(),
        height(),
        depth()
    );

    glPushMatrix();
        glTranslatef(o_x(), o_y(), o_z());
        glTranslatef(width() / 2, height() / 2, 0);
        glRotatef(pimpl->angle, 0.0f, 0.0f, 1.0f);

        box.draw(draw_axes);
    glPopMatrix();
}
