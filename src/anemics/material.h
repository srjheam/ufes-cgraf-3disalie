#ifndef MATERIAL_H
#define MATERIAL_H

struct material {
    float mat_emission[4];
    float mat_color_a[4];
    float mat_color_d[4];
    float mat_specular[4];
    float mat_shininess[1];
};

#endif
