#ifndef PLATFORM_H
#define PLATFORM_H

#include <GL/gl.h>

#include "entity.h"
#include "color_rgb.h"

#include "shapes/box.h"

class Platform : public Entity {
  private:
    Box *_box;

  public:
    Platform(
      GLfloat o_x,
      GLfloat o_y,
      GLfloat o_z,
      GLfloat width,
      GLfloat height,
      GLfloat depth);
    
    Platform(const Platform&) = delete;
    Platform& operator=(const Platform&) = delete;
    
    Platform(Platform&&) noexcept = default;
    Platform& operator=(Platform&&) noexcept = default;

    virtual ~Platform() override = default;

    void draw(bool draw_axes = false) const;
};

#endif
