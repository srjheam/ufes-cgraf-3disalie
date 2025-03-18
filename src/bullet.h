#ifndef BULLET_H_
#define BULLET_H_

#include <memory>

#include "entity.h"

class Bullet : public Entity {
  private:
    class Impl;
    std::unique_ptr<Impl> pimpl;

  public:
    Bullet(
      GLfloat o_x,
      GLfloat o_y,
      GLfloat o_z,
      GLfloat height,
      GLfloat width,
      GLfloat depth,
      Vector3 direction);

    Bullet(const Bullet&) = delete;
    Bullet& operator=(const Bullet&) = delete;
    
    Bullet(Bullet&&) noexcept;
    Bullet& operator=(Bullet&&) noexcept = default;

    ~Bullet();

    const Vector3 &direction() const;

    void draw(bool draw_axes = false) const;
};

#endif // BULLET_H_
