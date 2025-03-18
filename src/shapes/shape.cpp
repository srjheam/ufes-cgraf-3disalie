#include "shapes/shape.h"

#include <GL/gl.h>
#include <GL/glut.h>

Shape::Shape(bool invert_normal)
    : invert_normal(invert_normal ? -1 : 1) { }

Shape::Shape(Shape&&) noexcept = default;

void Shape::set_material(material m) const {
   glMaterialfv(GL_FRONT, GL_EMISSION, m.mat_emission);
   glMaterialfv(GL_FRONT, GL_AMBIENT, m.mat_color_a);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, m.mat_color_d);
   glMaterialfv(GL_FRONT, GL_SPECULAR, m.mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, m.mat_shininess);
}

Shape::~Shape() = default;

void Shape::draw(bool show_axes) const {
    if (!show_axes)
        return;

    v3f center = centroid();

    GLfloat mat_ambient_r[] = {1.0, 0.0, 0.0, 1.0};
    GLfloat mat_ambient_g[] = {0.0, 1.0, 0.0, 1.0};
    GLfloat mat_ambient_b[] = {0.0, 0.0, 1.0, 1.0};
    GLfloat no_mat[] = {0.0, 0.0, 0.0, 1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, no_mat);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_mat);

    const float size = 1.5f;
    glPushMatrix();
        glTranslatef(center.x, center.y, center.z);
        glDisable(GL_LIGHTING);
            // x axis red
            glPushMatrix();
                glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_r);
                glColor3fv(mat_ambient_r);
                glScalef(size, size * 0.1, size * 0.1);
                glTranslatef(0.5, 0, 0); // put in one end
                glutSolidCube(1.0);
            glPopMatrix();

            // y axis green
            glPushMatrix();
                glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_g);
                glColor3fv(mat_ambient_g);
                glRotatef(90, 0, 0, 1);
                glScalef(size, size * 0.1, size * 0.1);
                glTranslatef(0.5, 0, 0); // put in one end
                glutSolidCube(1.0);
            glPopMatrix();

            // z axis blue
            glPushMatrix();
                glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_b);
                glColor3fv(mat_ambient_b);
                glRotatef(-90, 0, 1, 0);
                glScalef(size, size * 0.1, size * 0.1);
                glTranslatef(0.5, 0, 0); // put in one end
                glutSolidCube(1.0);
            glPopMatrix();
        glEnable(GL_LIGHTING);
    glPopMatrix();
}
