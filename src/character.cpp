#include "character.h"

#include <GL/glut.h>

#include <iostream>
#include <cmath>
#include <algorithm>

#include "srutils.h"

#include "shapes/box.h"
#include "shapes/sphere.h"

class Character::Impl {
  public:
    Box *body_;
    Sphere *head_;
    Box *gun_;

    float currJumpYAcc = .0f;
    bool jumping = false;

    GLdouble sinceLastShotDt = .0f;

    //! [-45.0, +45.0]
    GLdouble gunAngleDeg = 0.0f;

    CharacterDirection direction_ = RIGHT;

    Impl(
        Box *body,
        Sphere *sphere,
        Box *gun)
            : body_(body),
            head_(sphere),
            gun_(gun) { }
    ~Impl() {
        delete body_;
        delete head_;
        delete gun_;
    }

    void draw_legs(const Character &ref, bool draw_axes) const {
        auto legsWidth = 2 * (ref.width() / 3);
        auto legsGap = ref.width() / 12;
        auto legWidth = legsWidth / 2 - legsGap / 2;
        auto legsHeight = (2 * ref.height() / 5);
        auto legsDepth = ref.depth() / 2;
        auto legsX0 = (ref.width() - legsWidth) / 2;
        auto legsY0 = 0;
        auto legsZ0 = (ref.depth() - legsDepth) / 2;
        
        const static Box leg(
            {
                { 0.0, 0.0, 0.0, 1},
                { 1.0, 0.0, 0.0, 1},
                { 1.0, 0.0, 0.0, 1},
                { 1.0, 1.0, 1.0, 1},
                { 128.0 }
            },
            legWidth,
            legsHeight,
            legsDepth
        );

        glTranslatef(legsX0, legsY0, legsZ0);
            leg.draw(draw_axes);

            glTranslatef(legWidth + legsGap, 0, 0);
                leg.draw(draw_axes);

            glTranslatef(-(legWidth + legsGap), 0, 0);
        glTranslatef(-legsX0, -legsY0, -legsZ0);
    }

    void draw_body(const Character &ref, bool draw_axes) const {
        auto bodyX0 = 0;
        auto bodyY0 = 0; //(2 * ref.height() / 5);
        auto bodyZ0 = 0;
        if (ref.jump_can_they()) {}
        glTranslatef(bodyX0, bodyY0, bodyZ0);
            body_->draw(draw_axes);
        glTranslatef(-bodyX0, -bodyY0, -bodyZ0);
    }

    void draw_head(const Character &ref, bool draw_axes) const {
        auto cx = ref.depth() / 2;
        auto cy = this->head_->radius() + 7 * (ref.height() / 10);
        auto cz = ref.width() / 2;

        glTranslatef(cx, cy, cz);
            head_->draw(draw_axes);
        glTranslatef(-cx, -cy, -cz);
    }

    void draw_arm(const Character &ref, bool draw_axes) const {
        glPushMatrix();
            glTranslatef(
                0,
                2 * ref.height() / 5 + 3 * (ref.height() / 20),
                0);

            glRotatef(direction_ == CharacterDirection::RIGHT ? gunAngleDeg : 180. - gunAngleDeg, 0, 0, 1);

            this->gun_->draw(draw_axes);
        glPopMatrix();
    }
};

Character::Character(
    GLfloat o_x,
    GLfloat o_y,
    GLfloat o_z,
    GLfloat width,
    GLfloat height,
    GLfloat depth,
    ColorRgb body
    ) : Entity(
            o_x,
            o_y,
            o_z,
            width,
            height,
            depth,
            height * 10,
            height * 5
        ), pimpl(new Impl(
            new Box(
                {
                    { 0.0, 0.0, 0.0, 1.0 },
                    { body.gl_red(), body.gl_green(), body.gl_blue(), 1.0 },
                    { body.gl_red(), body.gl_green(), body.gl_blue(), 1.0 },
                    { 1.0, 1.0, 1.0, 1.0 },
                    { 128 },
                },
                5 * (width / 6),
                height, // 3 * (height / 10),
                depth),
            new Sphere(
                {
                    { 0.0, 0.0, 0.0, 1.0 },
                    { body.gl_red(), body.gl_green(), body.gl_blue(), 1.0 },
                    { body.gl_red(), body.gl_green(), body.gl_blue(), 1.0 },
                    { 1.0, 1.0, 1.0, 1.0 },
                    { 128 },
                },
                3 * height / 20),
            new Box(
                {
                    { 0.0, 0.0, 0.0, 1.0 },
                    { 0.9, 1.0, 0.0, 1.0 },
                    { 0.9, 1.0, 0.0, 1.0 },
                    { 1.0, 1.0, 1.0, 1.0 },
                    { 128 },
                },
                width / 4,
                height / 20,
                depth / 4))) { }

Character::Character(Character&&) noexcept = default;

Character::~Character() = default;

const enum CharacterDirection &Character::direction() const {
    return pimpl->direction_;
}

void Character::direction(const enum CharacterDirection &direction) const {
    pimpl->direction_ = direction;
}

void Character::movement_translate(GLfloat dx, GLfloat dy) const {
    if (dx > 0)
        pimpl->direction_ = RIGHT;
    else if (dx < 0)
        pimpl->direction_ = LEFT;

    if (pimpl->jumping) {
        const float maxJumpYAcc = 3 * height();
        dy = srutils::minabseqsignf(dy, maxJumpYAcc - pimpl->currJumpYAcc);
        pimpl->currJumpYAcc += dy;
        pimpl->jumping = pimpl->jumping && (pimpl->currJumpYAcc < maxJumpYAcc);
    }

    Entity::movement_translate(dx, dy);
}

const bool &Character::jumping() const {
    return pimpl->jumping;
}

void Character::jumping(const bool &direction) const {
    pimpl->jumping = direction;
}

bool Character::jump_can_they() const {
    return pimpl->jumping || colisions_last_bottom() != nullptr;
}

void Character::jump_start() const {
    pimpl->currJumpYAcc = 0;
    pimpl->jumping = true;
}

void Character::jump_end() const {
    pimpl->jumping = false;
}

void Character::draw(bool draw_axes) const {
    //
    // given that a human head is 1/8 of their body height, the body is 3/8 and the legs are 4/8
    // https://www.google.com/search?q=head+proportion+to+body
    //
    // but this proportion is too strange here, so I will swap the body and legs proportions
    //

    if (hidden())
        return;

    glPushMatrix();
        glTranslatef(o_x(), o_y(), o_z());
        glTranslatef(width() / 2, height() / 2, 0);

        //pimpl->draw_legs(*this, draw_axes);
        
        pimpl->draw_body(*this, draw_axes);
        
        //pimpl->draw_head(*this, draw_axes);
        
        pimpl->draw_arm(*this, draw_axes);
    glPopMatrix();
}

void Character::aim(GLfloat dx, GLfloat dy) const {
    auto angle = (std::atan2(dy, dx) * 180 / M_PI);
    auto angle2 = (std::atan2(dy, -dx) * 180 / M_PI);

    if (direction() == RIGHT)
        pimpl->gunAngleDeg = std::clamp((angle > -90. && angle < 90.) ? angle : angle2, -45.0, 45.0);
    else
    {
        if (angle > 90.0)
            pimpl->gunAngleDeg = std::clamp(angle2, 0.0, 45.0);
        else if (angle < -90.0)
            pimpl->gunAngleDeg = std::clamp(angle2, -45.0, 0.0);
        else
            pimpl->gunAngleDeg = std::clamp(angle, -45.0, 45.0);
    }
}

bool Character::shot_add_dt(GLdouble dt) const {
    pimpl->sinceLastShotDt += dt;
    return pimpl->sinceLastShotDt > 3.0f * 1000;
}

Bullet Character::shoot() const {
    pimpl->sinceLastShotDt = 0.0f;

    auto angle = pimpl->gunAngleDeg * M_PI / 180.0;
    auto dx = cos(angle);
    auto dy = sin(angle);

    auto escalar_leave_x = 1;
    if (direction() == LEFT) {
        dx = -dx;
        escalar_leave_x = (-1 * width() + dx) / dx;
    }
    else
        escalar_leave_x = (1 * width() + dx) / dx;

    auto x0 = (o_x() + width() / 2) + dx * escalar_leave_x;
    auto y0 = (o_y() + height() / 2) + dy * escalar_leave_x;
    auto z0 = o_z() + depth() / 2;

    auto size = height() / 10;
    return Bullet(x0, y0, z0, size, size, size, Vector3(dx, dy));
}
