#include "boundary.h"

#include <GL/glut.h>
#include "shapes/box.h"
#include "color_rgb.h"

Boundary::Boundary(
    GLfloat o_x,
    GLfloat o_y,
    GLfloat o_z,
    GLfloat width,
    GLfloat height,
    GLfloat depth
    ) : Entity(
            o_x,
            o_y,
            o_z,
            width,
            height,
            depth,
            0,
            0
        ), _box(new Box(
            {
                { 0.2f, 0.2f, 0.2f, 1.0 },   
                { 0.36f, 0.22f, 0.03f, 1.0 },
                { 0.36f, 0.22f, 0.03f, 1.0 },
                { 0.1f, 0.1f, 0.1f, 1.0 },   
                { 32 },       
            },
            width,
            height,
            depth,
            true
        )) { }
        
void Boundary::draw(bool draw_axes) const {
    glPushMatrix();
        glTranslatef(o_x(), o_y(), o_z());
        glTranslatef(width() / 2, height() / 2, 0);

        this->_box->draw(draw_axes);
    glPopMatrix();
}
