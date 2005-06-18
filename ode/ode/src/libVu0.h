/***************************************************************
/  la lib 0.1 (snapshot 2)                                     /
/  Copyright (C) 2001 - Sony Computer Entertainment Inc.       /
/                                                              /
/  header:   libVu0 (libVu0.h)                                 /
/  author:   Sony Computer Entertainment Inc.                  /
/  last mod:                                                   /
/  license:  GNU Library General Public License Version 2      /
/            (see LICENSE on the main directory)               /
***************************************************************/

#ifndef _LIBVU0_H_
#define _LIBVU0_H_

// basic type
typedef int		libVu0IVector[4] __attribute__((aligned (16)));
typedef float	libVu0FVector[4] __attribute__((aligned (16)));
typedef float	libVu0FMatrix[4][4] __attribute__((aligned (16)));

// prototypes
void  libVu0CopyVector(libVu0FVector v0, libVu0FVector v1);
void  libVu0CopyVectorXYZ(libVu0FVector v0, libVu0FVector v1);
void  libVu0FToI0Vector(libVu0IVector v0, libVu0FVector v1);
void  libVu0FToI4Vector(libVu0IVector v0, libVu0FVector v1);
void  libVu0IToF0Vector(libVu0FVector v0, libVu0IVector v1);
void  libVu0IToF4Vector(libVu0FVector v0, libVu0IVector v1);
void  libVu0ScaleVector(libVu0FVector v0, libVu0FVector v1, float s);
void  libVu0ScaleVectorXYZ(libVu0FVector v0, libVu0FVector v1, float s);
void  libVu0AddVector(libVu0FVector v0, libVu0FVector v1, libVu0FVector v2);
void  libVu0SubVector(libVu0FVector v0, libVu0FVector v1, libVu0FVector v2);
void  libVu0MulVector(libVu0FVector v0, libVu0FVector v1, libVu0FVector v2);
void  libVu0InterVector(libVu0FVector v0, libVu0FVector v1, libVu0FVector v2, float r);
void  libVu0InterVectorXYZ(libVu0FVector v0, libVu0FVector v1, libVu0FVector v2, float r);
void  libVu0DivVector(libVu0FVector v0, libVu0FVector v1, float q);
void  libVu0DivVectorXYZ(libVu0FVector v0, libVu0FVector v1, float q);
float libVu0InnerProduct(libVu0FVector v0, libVu0FVector v1);
void  libVu0OuterProduct(libVu0FVector v0, libVu0FVector v1, libVu0FVector v2);
void  libVu0Normalize(libVu0FVector v0, libVu0FVector v1);
void libVu0InverseMatrix(libVu0FMatrix m0, libVu0FMatrix m1);
void  libVu0ApplyMatrix(libVu0FVector v0, libVu0FMatrix m, libVu0FVector v1);
void  libVu0UnitMatrix(libVu0FMatrix m);
void  libVu0CopyMatrix(libVu0FMatrix m0, libVu0FMatrix m1);
void  libVu0TransposeMatrix(libVu0FMatrix m0, libVu0FMatrix m1);
void  libVu0MulMatrix(libVu0FMatrix m0, libVu0FMatrix m1, libVu0FMatrix m2);
void  libVu0InversMatrix(libVu0FMatrix m0, libVu0FMatrix m1);
void  libVu0RotMatrixX(libVu0FMatrix m0, libVu0FMatrix m1, float rx);
void  libVu0RotMatrixY(libVu0FMatrix m0, libVu0FMatrix m1, float ry);
void  libVu0RotMatrixZ(libVu0FMatrix m0, libVu0FMatrix m1, float rz);
void  libVu0RotMatrix(libVu0FMatrix m0, libVu0FMatrix m1, libVu0FVector rot);
void  libVu0TransMatrix(libVu0FMatrix m0, libVu0FMatrix m1, libVu0FVector tv);
void  libVu0CameraMatrix(libVu0FMatrix m, libVu0FVector p, libVu0FVector zd, libVu0FVector yd);
void  libVu0NormalLightMatrix(libVu0FMatrix m, libVu0FVector l0, libVu0FVector l1, libVu0FVector l2);
void  libVu0LightColorMatrix(libVu0FMatrix m, libVu0FVector c0, libVu0FVector c1, libVu0FVector c2, libVu0FVector a);
void  libVu0ViewScreenMatrix(libVu0FMatrix m, float scrz, float ax, float ay, float cx, float cy, float zmin, float zmax, float nearz, float farz);
void  libVu0DropShadowMatrix(libVu0FMatrix m, libVu0FVector lp, float a, float b, float c, int mode);
int   libVu0ClipAll(libVu0FVector minv, libVu0FVector maxv, libVu0FMatrix ms, libVu0FVector *vm, int n);
void libVu0ClampVector(libVu0FVector v0, libVu0FVector v1, float min, float max);
int libVu0ClipScreen(libVu0FVector v0);
int libVu0ClipScreen3(libVu0FVector v0, libVu0FVector v1, libVu0FVector v2);
void libVu0RotTransPersN(libVu0IVector *v0, libVu0FMatrix m0, libVu0FVector *v1, int n, int mode);
void libVu0RotTransPers(libVu0IVector v0, libVu0FMatrix m0, libVu0FVector v1, int mode);

#endif
