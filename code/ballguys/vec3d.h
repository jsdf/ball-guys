
#ifndef VEC3D_H
#define VEC3D_H

#include <stdio.h>

typedef struct Vec3d
{
    float x;
    float y;
    float z;
} Vec3d;

Vec3d *Vec3d_alloc(float x, float y, float z);
void Vec3d_set(Vec3d *self, float x, float y, float z);
Vec3d *Vec3d_init(Vec3d *self, float x, float y, float z);
Vec3d *Vec3d_origin(Vec3d *self);
Vec3d *Vec3d_copyFrom(Vec3d *self, Vec3d *other);
float Vec3d_distanceTo(Vec3d *self, Vec3d *other);
float Vec3d_distanceToSq(Vec3d *self, Vec3d *other);
Vec3d *Vec3d_directionTo(Vec3d *self, Vec3d *other, Vec3d *result);

Vec3d *Vec3d_add(Vec3d *self, Vec3d *other);
Vec3d *Vec3d_sub(Vec3d *self, Vec3d *other);

Vec3d *Vec3d_mul(Vec3d *self, Vec3d *other);

Vec3d *Vec3d_div(Vec3d *self, Vec3d *other);

Vec3d *Vec3d_mulScalar(Vec3d *self, float scalar);
Vec3d *Vec3d_divScalar(Vec3d *self, float scalar);

Vec3d *Vec3d_normalise(Vec3d *self);
Vec3d *Vec3d_lerp(Vec3d *self, Vec3d *v, float alpha);

float Vec3d_magSq(Vec3d *self);
float Vec3d_mag(Vec3d *self);
float Vec3d_dot(Vec3d *self, Vec3d *other);
void Vec3d_cross(Vec3d *self, Vec3d *other, Vec3d *result);

int Vec3d_validate(Vec3d *self);

char *Vec3d_toString(Vec3d *self, char *buffer);
char *Vec3d_toStringSafe(Vec3d *self, char *buffer, int max);
void Vec3d_print(Vec3d *self, FILE *dest);

#ifdef __cplusplus
#include <string>
std::string Vec3d_toStdString(Vec3d *self);
#endif

#endif /* !VEC3D_H */
