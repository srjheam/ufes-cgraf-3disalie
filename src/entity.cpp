#include "entity.h"

#include <cmath>
#include <iostream>
#include <limits>

#include <GL/glut.h>

#include "srutils.h"

class Entity::Impl {
  public:
    GLfloat o_x_;
    GLfloat o_y_;
    GLfloat o_z_;

    GLfloat height_;
    GLfloat width_;
    GLfloat depth_;

    GLdouble ttl_;
    bool hidden_;

    GLfloat terminalVelocityXZ_;
    GLfloat terminalVelocityY_;

    Vector3 lastVector_;
    Vector3 currentVector_;

    const Entity *lastCollidedX_ = nullptr;
    const Entity *lastCollidedY_ = nullptr;
    const Entity *lastCollidedZ_ = nullptr;

    Impl(
        GLfloat o_x,
        GLfloat o_y,
        GLfloat o_z,
        GLfloat width,
        GLfloat height,
        GLfloat depth,
        GLfloat terminalVelocityXZ,
        GLfloat terminalVelocityY
        ) : o_x_(o_x),
            o_y_(o_y),
            o_z_(o_z),
            height_(height),
            width_(width),
            depth_(depth),
            ttl_(std::numeric_limits<GLdouble>::max()),
            hidden_(false),
            terminalVelocityXZ_(terminalVelocityXZ),
            terminalVelocityY_(terminalVelocityY),
            lastVector_(),
            currentVector_() { }
};

Entity::Entity(
    GLfloat o_x,
    GLfloat o_y,
    GLfloat o_z,
    GLfloat width,
    GLfloat height,
    GLfloat depth,
    GLfloat terminalVelocityXZ,
    GLfloat terminalVelocityY
    ) : pimpl(new Impl(o_x, o_y, o_z, width, height, depth, terminalVelocityXZ, terminalVelocityY)) { }

Entity::Entity(Entity&&) noexcept = default;

Entity::~Entity() = default;

GLfloat Entity::o_x() const { return pimpl->o_x_; }

GLfloat Entity::o_y() const { return pimpl->o_y_; }

GLfloat Entity::o_z() const { return pimpl->o_z_; }

GLfloat Entity::height() const { return pimpl->height_; }
GLfloat Entity::width() const { return pimpl->width_; }
GLfloat Entity::depth() const { return pimpl->depth_; }

GLdouble Entity::ttl() const { return pimpl->ttl_; }
void Entity::ttl(const GLdouble &ttl) const { pimpl->ttl_ = ttl; }

bool Entity::hidden() const { return pimpl->hidden_; }
void Entity::hidden(const bool &hidden) const { pimpl->hidden_ = hidden; }

void Entity::die() const { pimpl->ttl_ = -1.0; pimpl->hidden_ = true; }

void Entity::movement_translate(GLfloat dx, GLfloat dy) const {
    pimpl->o_x_ += dx;
    pimpl->o_y_ += dy;
}

const Vector3 &Entity::vector_last() const {
    return pimpl->lastVector_;
}

const Vector3 &Entity::vector_current() const {
    return pimpl->currentVector_;
}

void Entity::vector_set_direction(GLfloat dx, GLfloat dy) const {
    pimpl->currentVector_.set_direction(dx, dy);
}

void Entity::vector_save_current_set_zero() const {
    pimpl->lastVector_ = pimpl->currentVector_;
    pimpl->currentVector_.set_zero();
}

void Entity::vector_save_current_set(const Vector3 &vector) const {
    pimpl->lastVector_ = pimpl->currentVector_;
    pimpl->currentVector_ = vector;
}

void Entity::vector_sum(GLfloat vx, GLfloat vy, GLfloat v) const {
    pimpl->currentVector_.sum(vx, vy, v);
}

bool Entity::aabb_isoverlapping_x(const Entity &entity) const {
    // =====-1=-=----=-=2-===

    return 
        (srutils::epslt(pimpl->o_x_, entity.o_x() + entity.width()) && srutils::epsgte(pimpl->o_x_, entity.o_x())) // 1
        || (srutils::epsgt(pimpl->o_x_ + pimpl->width_, entity.o_x()) && srutils::epslte(pimpl->o_x_ + pimpl->width_, entity.o_x() + entity.width())); // 2
}

bool Entity::aabb_isoverlapping_y(const Entity &entity) const {
    // ||
    // ||
    // ||
    // ii
    //  1
    // ||
    // ii
    // ||
    // ii
    // ii
    // ii
    // ii
    // ||
    // ii
    // ||
    //  2
    // ii
    // ||
    // ||
    // ||

    return
        (srutils::epsgt(pimpl->o_y_ + pimpl->height_, entity.o_y()) && srutils::epslte(pimpl->o_y_ + pimpl->height_, entity.o_y() + entity.height())) // 1
        || (srutils::epslt(pimpl->o_y_, entity.o_y() + entity.height()) && srutils::epsgte(pimpl->o_y_, entity.o_y())); // 2
}

bool Entity::aabb_isoverlapping(const Entity &entity) const {
    return aabb_isoverlapping_x(entity) && aabb_isoverlapping_y(entity);
}

bool Entity::aabb_isoverlapping_x_dx(const Entity &entity, GLfloat dx) const {
    // =====-1=-=----=-=2-===

    GLfloat tleft = pimpl->o_x_ + dx;
    GLfloat lright = tleft + pimpl->width_;
    
    GLfloat eleft = entity.o_x();
    GLfloat eright = eleft + entity.width();
    
    return ((tleft < eright) && (lright > eleft))
        || ((fmin(pimpl->o_x_, tleft) < eleft) && (fmax(lright, pimpl->o_x_ + pimpl->width_) > eright));
}

bool Entity::aabb_isoverlapping_y_dy(const Entity &entity, GLfloat dy) const {
    // ||
    // ||
    // ||
    // ii
    //  1
    // ||
    // ii
    // ||
    // ii
    // ii
    // ii
    // ii
    // ||
    // ii
    // ||
    //  2
    // ii
    // ||
    // ||
    // ||

    GLfloat tbot = pimpl->o_y_ + dy;
    GLfloat ttop = tbot + pimpl->height_;
    
    GLfloat ebot = entity.o_y();
    GLfloat etop = ebot + entity.height();
    
    return ((tbot < etop) && (ttop > ebot))
        || ((fmin(pimpl->o_y_, tbot) < ebot) && (fmax(ttop, pimpl->o_y_ + pimpl->height_) > etop));

}

bool Entity::aabb_isoverlapping_delta(const Entity &stationary, GLfloat dx, GLfloat dy) const {
    return aabb_isoverlapping_x_dx(stationary, dx) && aabb_isoverlapping_y_dy(stationary, dy);
}

bool Entity::aabb_isoverlapping_dx(const Entity &stationary, GLfloat dx) const {
    return aabb_isoverlapping_delta(stationary, dx, .0f);
}

bool Entity::aabb_isoverlapping_dy(const Entity &stationary, GLfloat dy) const {
    return aabb_isoverlapping_delta(stationary, .0f, dy);
}

GLfloat Entity::aabb_distanceof_x(const Entity &entity) const {
    if (aabb_isoverlapping_x(entity))
        return 0.f;

    if (srutils::epslte(pimpl->o_x_, entity.o_x())) { // T -> E
        return fabs(entity.o_x() - (pimpl->o_x_ + pimpl->width_));
    }
    else { // E -> T
        return (entity.o_x() + entity.width()) - pimpl->o_x_;
    }
}

GLfloat Entity::aabb_distanceof_y(const Entity &entity) const {
    if (aabb_isoverlapping_y(entity))
        return 0.f;

    if (srutils::epslte(pimpl->o_y_, entity.o_y())) { // T -> E
        return fabs(entity.o_y() - (pimpl->o_y_ + pimpl->height_));
    }
    else { // E -> T
        return (entity.o_y() + entity.height()) - pimpl->o_y_;
    }
}

bool Entity::aabb_isinsideof_x(const Entity &entity) const {
    return srutils::epsgte(pimpl->o_x_, entity.o_x()) && srutils::epslte(pimpl->o_x_ + pimpl->width_, entity.o_x() + entity.width());
}

bool Entity::aabb_isinsideof_y(const Entity &entity) const {
    return srutils::epsgte(pimpl->o_y_, entity.o_y()) && srutils::epslte(pimpl->o_y_ + pimpl->height_, entity.o_y() + entity.height());
}

bool Entity::aabb_isinsideof_dx(const Entity &entity, GLfloat dx) const {
    GLfloat nx = pimpl->o_x_ + dx;
    GLfloat nxwidth = nx + pimpl->width_;

    GLfloat ex = entity.o_x();
    GLfloat exwidth = ex + entity.width();

    return ((ex < nx) && (nxwidth < exwidth));
}

bool Entity::aabb_isinsideof_dy(const Entity &entity, GLfloat dy) const {
    GLfloat ny = pimpl->o_y_ + dy;
    GLfloat nyheight = ny + pimpl->height_;

    GLfloat ey = entity.o_y();
    GLfloat eyheight = ey + entity.height();

    return ((ey < ny) && (nyheight < eyheight));
}

GLfloat Entity::aabb_insideof_x(const Entity &entity, int8_t dir) const {
    // colision walls
    // --------------
    // |            |
    // |1          2|
    // |            |
    // --------------

    return dir > 0
        ? fabs((entity.o_x() + entity.width()) - (pimpl->o_x_ + pimpl->width_))
        : -fabs((entity.o_x()) - (pimpl->o_x_));
}

GLfloat Entity::aabb_insideof_y(const Entity &entity, int8_t dir) const {
        // colision walls
        // --------------
        // |     4      |
        // |            |
        // |     3      |
        // --------------

    return dir > 0
        ? fabs((entity.o_y() + entity.height()) - (pimpl->o_y_ + pimpl->height_))
        : -fabs((entity.o_y()) - (pimpl->o_y_));
}

const Entity *Entity::colisions_last_top() const {
    return (pimpl->lastVector_.direction_y() > 0) ? pimpl->lastCollidedY_ : nullptr;
}

const Entity *Entity::colisions_last_bottom() const {
    return (pimpl->lastVector_.direction_y() < 0) ? pimpl->lastCollidedY_ : nullptr;
}

const Entity *Entity::colisions_last_left() const {
    return (pimpl->lastVector_.direction_x() < 0) ? pimpl->lastCollidedX_ : nullptr;
}

const Entity *Entity::colisions_last_right() const {
    return (pimpl->lastVector_.direction_x() > 0) ? pimpl->lastCollidedX_ : nullptr;
}

const std::tuple<const Entity*, const Entity*> Entity::colisions_tuple() const {
    return std::make_tuple(pimpl->lastCollidedX_, pimpl->lastCollidedY_);
}

void Entity::colisions_set_last_x(const Entity &entity) const {
    pimpl->lastCollidedX_ = &entity;
}

void Entity::colisions_set_last_y(const Entity &entity) const {
    pimpl->lastCollidedY_ = &entity;
}

void Entity::colisions_reset() const {
    pimpl->lastCollidedX_ = nullptr;
    pimpl->lastCollidedY_ = nullptr;
}
