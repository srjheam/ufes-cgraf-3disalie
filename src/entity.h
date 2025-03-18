#ifndef ENTITY_H
#define ENTITY_H

#include <GL/gl.h>

#include <memory>
#include <tuple>

#include "vector3.h"

class Entity {
  private:
    class Impl;
    std::unique_ptr<Impl> pimpl;

  public:
    Entity(
      GLfloat o_x,
      GLfloat o_y,
      GLfloat o_z,
      GLfloat width,
      GLfloat height,
      GLfloat depth,
      GLfloat terminalVelocityXZ,
      GLfloat terminalVelocityY);

    Entity(const Entity &) = delete;
    Entity &operator=(const Entity &) = delete;

    Entity(Entity &&) noexcept;
    Entity &operator=(Entity &&) noexcept = default;

    virtual ~Entity();
    
    GLfloat o_x() const;
    GLfloat o_y() const;
    GLfloat o_z() const;

    GLfloat height() const;
    GLfloat width() const;
    GLfloat depth() const;

    GLdouble ttl() const;
    void ttl(const GLdouble &ttl) const;

    bool hidden() const;
    void hidden(const bool &hidden) const;

    void die() const;

    virtual void movement_translate(GLfloat dx, GLfloat dy) const;

    const Vector3 &vector_last() const;
    const Vector3 &vector_current() const;

    //! Does not change the velocity
    void vector_set_direction(GLfloat dx, GLfloat dy) const;

    void vector_save_current_set_zero() const;

    void vector_save_current_set(const Vector3 &vector) const;

    //! Vector3 must be normalized
    void vector_sum(GLfloat vx, GLfloat vy, GLfloat velocity) const;

    bool aabb_isoverlapping_x(const Entity &entity) const;
    bool aabb_isoverlapping_y(const Entity &entity) const;

    bool aabb_isoverlapping(const Entity &entity) const;

    bool aabb_isoverlapping_x_dx(const Entity &entity, GLfloat dx) const;
    bool aabb_isoverlapping_y_dy(const Entity &entity, GLfloat dy) const;

    bool aabb_isoverlapping_delta(const Entity &stationary, GLfloat dx,
                                  GLfloat dy) const;
    bool aabb_isoverlapping_dx(const Entity &stationary, GLfloat dx) const;
    bool aabb_isoverlapping_dy(const Entity &stationary, GLfloat dy) const;

    //! Return zero if overlapping
    GLfloat aabb_distanceof_x(const Entity &entity) const;
    //! Return zero if overlapping
    GLfloat aabb_distanceof_y(const Entity &entity) const;

    bool aabb_isinsideof_x(const Entity &entity) const;
    bool aabb_isinsideof_y(const Entity &entity) const;

    bool aabb_isinsideof_dx(const Entity &entity, GLfloat dx) const;
    bool aabb_isinsideof_dy(const Entity &entity, GLfloat dy) const;

    //! Return the shortest distance between the nearest borders; the signal
    //! follows the cardinal direction relative to the container even if the
    //! entities are not overlapping at all
    GLfloat aabb_insideof_x(const Entity &entity, int8_t dir) const;
    //! Return the shortest distance between the nearest borders; the signal
    //! follows the cardinal direction relative to the container even if the
    //! entities are not overlapping at all
    GLfloat aabb_insideof_y(const Entity &entity, int8_t dir) const;

    const Entity *colisions_last_top() const;
    const Entity *colisions_last_bottom() const;
    const Entity *colisions_last_left() const;
    const Entity *colisions_last_right() const;

    const std::tuple<const Entity *, const Entity *> colisions_tuple() const;

    void colisions_set_last_x(const Entity &entity) const;
    void colisions_set_last_y(const Entity &entity) const;

    void colisions_reset() const;

    virtual void draw(bool draw_axes = false) const = 0;
};

#endif
