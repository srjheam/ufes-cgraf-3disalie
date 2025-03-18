#ifndef BOUNDARY_H
#define BOUNDARY_H

#include <GL/gl.h>

#include "entity.h"
#include "color_rgb.h"

#include "shapes/box.h"

class Boundary : public Entity {
  private:
    Box *_box;

  public:
    Boundary(
      GLfloat o_x,
      GLfloat o_y,
      GLfloat o_z,
      GLfloat width,
      GLfloat height,
      GLfloat depth);
    
    Boundary(const Boundary&) = delete;
    Boundary& operator=(const Boundary&) = delete;
    
    Boundary(Boundary&&) noexcept = default;
    Boundary& operator=(Boundary&&) noexcept = default;

    virtual ~Boundary() override = default;

    void draw(bool draw_axes = false) const;
};

#endif
