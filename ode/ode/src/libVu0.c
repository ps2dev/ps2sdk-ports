/***************************************************************
/  la lib 0.1 (snapshot 2)                                     /
/  Copyright (C) 2001 - Sony Computer Entertainment Inc.       /
/                                                              /
/  module:   libVu0 (libVu0.c)                                 /
/  author:   Sony Computer Entertainment Inc.                  /
/  last mod:                                                   /
/  license:  GNU Library General Public License Version 2      /
/            (see LICENSE on the main directory)               /
***************************************************************/

#include <libVu0.h>

void libVu0ApplyMatrix(libVu0FVector v0, libVu0FMatrix m0,libVu0FVector v1)
{
	asm __volatile__(
	"lqc2    vf4,0x0(%1)            \n\t"
	"lqc2    vf5,0x10(%1)           \n\t"
	"lqc2    vf6,0x20(%1)           \n\t"
	"lqc2    vf7,0x30(%1)           \n\t"
	"lqc2    vf8,0x0(%2)            \n\t"
	"vmulax.xyzw	ACC,   vf4,vf8  \n\t"
	"vmadday.xyzw	ACC,   vf5,vf8  \n\t"
	"vmaddaz.xyzw	ACC,   vf6,vf8  \n\t"
	"vmaddw.xyzw	vf9,vf7,vf8     \n\t"
	"sqc2    vf9,0x0(%0)            \n\t"
	: : "r" (v0) , "r" (m0) ,"r" (v1));
}

void libVu0MulMatrix(libVu0FMatrix m0, libVu0FMatrix m1,libVu0FMatrix m2)
{

	asm __volatile__(
	"lqc2    vf4,0x0(%2)           \n\t"
	"lqc2    vf5,0x10(%2)          \n\t"
	"lqc2    vf6,0x20(%2)          \n\t"
	"lqc2    vf7,0x30(%2)          \n\t"
	"li    $7,4                    \n"
"1:                                    \n\t"
	"lqc2    vf8,0x0(%1)           \n\t"
	"vmulax.xyzw	ACC,   vf4,vf8 \n\t"
	"vmadday.xyzw	ACC,   vf5,vf8 \n\t"
	"vmaddaz.xyzw	ACC,   vf6,vf8 \n\t"
	"vmaddw.xyzw	vf9,vf7,vf8    \n\t"
	"sqc2    vf9,0x0(%0)           \n\t"
	"addi    $7,-1                 \n\t"
	"addi    %1,0x10               \n\t"
	"addi    %0,0x10               \n\t"
	"bne    $0,$7,1b               \n\t"
	:: "r" (m0), "r" (m2), "r" (m1) : "$7");
}

void libVu0OuterProduct(libVu0FVector v0, libVu0FVector v1, libVu0FVector v2)
{
	asm __volatile__(
	"lqc2    vf4,0x0(%1)                        \n\t"
	"lqc2    vf5,0x0(%2)                        \n\t"
	"vopmula.xyz	ACC,vf4,vf5                 \n\t"
	"vopmsub.xyz	vf6,vf5,vf4                 \n\t"
	"vsub.w vf6,vf6,vf6		#vf6.xyz=0; \n\t"
	"sqc2    vf6,0x0(%0)                        \n\t"
	: : "r" (v0) , "r" (v1) ,"r" (v2));
}

float libVu0InnerProduct(libVu0FVector v0, libVu0FVector v1)
{
	register float ret;

	asm __volatile__(
	"lqc2    vf4,0x0(%1)     \n\t"
	"lqc2    vf5,0x0(%2)     \n\t"
	"vmul.xyz vf5,vf4,vf5    \n\t"
	"vaddy.x vf5,vf5,vf5     \n\t"
	"vaddz.x vf5,vf5,vf5     \n\t"
	"qmfc2   $2 ,vf5         \n\t"
	"mtc1    $2,%0           \n\t"
	: "=f" (ret) :"r" (v0) ,"r" (v1) :"$2" );
	return ret;
}

void libVu0Normmalize(libVu0FVector v0, libVu0FVector v1)
{
#if 0
	asm __volatile__(
	"lqc2    vf4,0x0(%1)    \n\t"                        
	"vmul.xyz vf5,vf4,vf4   \n\t"
	"vaddy.x vf5,vf5,vf5    \n\t"
	"vaddz.x vf5,vf5,vf5    \n\t"

	"vrsqrt Q,vf0w,vf5x     \n\t"
	"vsub.xyzw vf6,vf0,vf0		#vf6.xyzw=0;  \n\t"
	"vaddw.xyzw vf6,vf6,vf4		#vf6.w=vf4.w; \n\t"
	"vwaitq                 \n\t"

	"vmulq.xyz  vf6,vf4,Q   \n\t"
	"sqc2    vf6,0x0(%0)    \n\t"
	: : "r" (v0) , "r" (v1));

#else   //rsqrt bug 

        asm __volatile__(
        "lqc2    vf4,0x0(%1)    \n\t"
        "vmul.xyz vf5,vf4,vf4   \n\t"
        "vaddy.x vf5,vf5,vf5    \n\t"
        "vaddz.x vf5,vf5,vf5    \n\t"

        "vsqrt Q,vf5x           \n\t"
        "vwaitq                 \n\t"
        "vaddq.x vf5x,vf0x,Q    \n\t"
        "vdiv    Q,vf0w,vf5x    \n\t"
        "vsub.xyzw vf6,vf0,vf0           #vf6.xyzw=0;  \n\t"
        "vwaitq                 \n\t"

        "vmulq.xyz  vf6,vf4,Q   \n\t"
        "sqc2    vf6,0x0(%0)    \n\t"
        : : "r" (v0) , "r" (v1));
#endif
}

void libVu0TransposeMatrix(libVu0FMatrix m0, libVu0FMatrix m1)
{
	asm __volatile__(
	"lq $8,0x0000(%1)        \n\t"
	"lq $9,0x0010(%1)        \n\t"
	"lq $10,0x0020(%1)       \n\t"
	"lq $11,0x0030(%1)       \n\t"

	"pextlw     $12,$9,$8    \n\t"
	"pextuw     $13,$9,$8    \n\t"
	"pextlw     $14,$11,$10  \n\t"
	"pextuw     $15,$11,$10  \n\t"

	"pcpyld     $8,$14,$12   \n\t"
	"pcpyud     $9,$12,$14   \n\t"
	"pcpyld     $10,$15,$13  \n\t"
	"pcpyud     $11,$13,$15  \n\t"

	"sq $8,0x0000(%0)        \n\t"
	"sq $9,0x0010(%0)        \n\t"
	"sq $10,0x0020(%0)       \n\t"
	"sq $11,0x0030(%0)       \n\t"
	: : "r" (m0) , "r" (m1):"$8","$9","$10","$11","$12","$13","$14","$15");
}

void libVu0InverseMatrix(libVu0FMatrix m0, libVu0FMatrix m1)
{
	asm __volatile__(
	"lq $8,0x0000(%1)        \n\t"
	"lq $9,0x0010(%1)        \n\t"
	"lq $10,0x0020(%1)       \n\t"
	"lqc2 vf4,0x0030(%1)     \n\t"

	"vmove.xyzw vf5,vf4      \n\t"
	"vsub.xyz vf4,vf4,vf4		#vf4.xyz=0;  \n\t"
	"vmove.xyzw vf9,vf4      \n\t"
	"qmfc2    $11,vf4        \n\t"

	"pextlw     $12,$9,$8    \n\t"
	"pextuw     $13,$9,$8    \n\t"
	"pextlw     $14,$11,$10  \n\t"
	"pextuw     $15,$11,$10  \n\t"
	"pcpyld     $8,$14,$12   \n\t"
	"pcpyud     $9,$12,$14   \n\t"
	"pcpyld     $10,$15,$13  \n\t"

	"qmtc2    $8,vf6         \n\t"
	"qmtc2    $9,vf7         \n\t"
	"qmtc2    $10,vf8        \n\t"

	"vmulax.xyz	ACC,   vf6,vf5  \n\t"
	"vmadday.xyz	ACC,   vf7,vf5  \n\t"
	"vmaddz.xyz	vf4,vf8,vf5     \n\t"
	"vsub.xyz	vf4,vf9,vf4     \n\t"

	"sq $8,0x0000(%0)        \n\t"  
	"sq $9,0x0010(%0)        \n\t"
	"sq $10,0x0020(%0)       \n\t"
	"sqc2 vf4,0x0030(%0)     \n\t"
	: : "r" (m0) , "r" (m1):"$8","$9","$10","$11","$12","$13","$14","$15");

}

void libVu0DivVector(libVu0FVector v0, libVu0FVector v1, float q)
{
	asm __volatile__(
	"lqc2    vf4,0x0(%1)    \n\t"
	"mfc1    $8,%2          \n\t"
	"qmtc2    $8,vf5        \n\t"
	"vdiv    Q,vf0w,vf5x    \n\t"
	"vwaitq                 \n\t"
	"vmulq.xyzw      vf4,vf4,Q   \n\t"
	"sqc2    vf4,0x0(%0)         \n\t"
	: : "r" (v0) , "r" (v1), "f" (q):"$8");
}

void libVu0DivVectorXYZ(libVu0FVector v0, libVu0FVector v1, float q)
{
	asm __volatile__(
	"lqc2    vf4,0x0(%1)   \n\t"
	"mfc1    $8,%2         \n\t" 
	"qmtc2    $8,vf5       \n\t"

	"vdiv    Q,vf0w,vf5x   \n\t"
	"vwaitq                \n\t"
	"vmulq.xyz      vf4,vf4,Q  \n\t"

	"sqc2    vf4,0x0(%0)       \n\t"
	: : "r" (v0) , "r" (v1), "f" (q) : "$8");
}

void libVu0InterVector(libVu0FVector v0, libVu0FVector v1, libVu0FVector v2, float t)
{
	asm __volatile__(
	"lqc2    vf4,0x0(%1)    \n\t"
	"lqc2    vf5,0x0(%2)    \n\t"
	"mfc1    $8,%3          \n\t"
	"qmtc2    $8,vf6        \n\t"

        "vaddw.x    vf7,vf0,vf0	#vf7.x=1;    \n\t"
        "vsub.x    vf8,vf7,vf6	#vf8.x=1-t;  \n\t" 
	"vmulax.xyzw	ACC,   vf4,vf6       \n\t"
	"vmaddx.xyzw	vf9,vf5,vf8          \n\t"

	"sqc2    vf9,0x0(%0)                 \n\t"
	: : "r" (v0) , "r" (v1), "r" (v2), "f" (t) : "$8");
}

void libVu0AddVector(libVu0FVector v0, libVu0FVector v1, libVu0FVector v2)
{
	asm __volatile__(
	"lqc2    vf4,0x0(%1)         \n\t"
	"lqc2    vf5,0x0(%2)         \n\t"
	"vadd.xyzw	vf6,vf4,vf5  \n\t"
	"sqc2    vf6,0x0(%0)         \n\t"
	: : "r" (v0) , "r" (v1), "r" (v2));
}

void libVu0SubVector(libVu0FVector v0, libVu0FVector v1, libVu0FVector v2)
{
	asm __volatile__(
	"lqc2    vf4,0x0(%1)         \n\t"
	"lqc2    vf5,0x0(%2)         \n\t"
	"vsub.xyzw	vf6,vf4,vf5  \n\t"
	"sqc2    vf6,0x0(%0)         \n\t"
	: : "r" (v0) , "r" (v1), "r" (v2));
}

void libVu0MulVector(libVu0FVector v0, libVu0FVector v1, libVu0FVector v2)
{
	asm __volatile__(
	"lqc2    vf4,0x0(%1)         \n\t"
	"lqc2    vf5,0x0(%2)         \n\t"
	"vmul.xyzw	vf6,vf4,vf5  \n\t"
	"sqc2    vf6,0x0(%0)         \n\t"
	: : "r" (v0) , "r" (v1), "r" (v2));
}

void libVu0ScaleVector(libVu0FVector v0, libVu0FVector v1, float t)
{
	asm __volatile__(
	"lqc2    vf4,0x0(%1)         \n\t"
	"mfc1    $8,%2               \n\t"
	"qmtc2    $8,vf5             \n\t"
	"vmulx.xyzw	vf6,vf4,vf5  \n\t"
	"sqc2    vf6,0x0(%0)         \n\t"
	: : "r" (v0) , "r" (v1), "f" (t):"$8");
}

void libVu0TransMatrix(libVu0FMatrix m0, libVu0FMatrix m1, libVu0FVector tv)
{
	asm __volatile__(
	"lqc2    vf4,0x0(%2)        \n\t"
	"lqc2    vf5,0x30(%1)       \n\t"

	"lq    $7,0x0(%1)           \n\t"
	"lq    $8,0x10(%1)          \n\t"
	"lq    $9,0x20(%1)          \n\t"
	"vadd.xyz	vf5,vf5,vf4 \n\t"
	"sq    $7,0x0(%0)           \n\t"
	"sq    $8,0x10(%0)          \n\t"
	"sq    $9,0x20(%0)          \n\t"
	"sqc2    vf5,0x30(%0)       \n\t"
	: : "r" (m0) , "r" (m1), "r" (tv):"$7","$8","$9");
}

void libVu0CopyVector(libVu0FVector v0, libVu0FVector v1)
{
	asm __volatile__(
	"lq    $6,0x0(%1)  \n\t"
	"sq    $6,0x0(%0)  \n\t"
	: : "r" (v0) , "r" (v1):"$6");
}

void libVu0CopyMatrix(libVu0FMatrix m0, libVu0FMatrix m1)
{
	asm __volatile__(
	"lq    $6,0x0(%1)  \n\t"
	"lq    $7,0x10(%1) \n\t"
	"lq    $8,0x20(%1) \n\t"
	"lq    $9,0x30(%1) \n\t"
	"sq    $6,0x0(%0)  \n\t"
	"sq    $7,0x10(%0) \n\t"
	"sq    $8,0x20(%0) \n\t"
	"sq    $9,0x30(%0) \n\t"
	: : "r" (m0) , "r" (m1):"$6","$7","$8","$9");
}

void libVu0FToI4Vector(libVu0IVector v0, libVu0FVector v1)
{
	asm __volatile__(
	"lqc2    vf4,0x0(%1)     \n\t"
	"vftoi4.xyzw	vf5,vf4  \n\t"
	"sqc2    vf5,0x0(%0)     \n\t"
	: : "r" (v0) , "r" (v1));
}

void libVu0FToI0Vector(libVu0IVector v0, libVu0FVector v1)
{
	asm __volatile__(
	"lqc2    vf4,0x0(%1)     \n\t"
	"vftoi0.xyzw	vf5,vf4  \n\t"
	"sqc2    vf5,0x0(%0)     \n\t"
	: : "r" (v0) , "r" (v1));
}

void libVu0IToF4Vector(libVu0FVector v0, libVu0IVector v1)
{
	asm __volatile__(
	"lqc2    vf4,0x0(%1)     \n\t"
	"vitof4.xyzw	vf5,vf4  \n\t"
	"sqc2    vf5,0x0(%0)     \n\t"
	: : "r" (v0) , "r" (v1));
}

void libVu0IToF0Vector(libVu0FVector v0, libVu0IVector v1)
{
	asm __volatile__(
	"lqc2    vf4,0x0(%1)      \n\t"
	"vitof0.xyzw	vf5,vf4   \n\t"
	"sqc2    vf5,0x0(%0)      \n\t"
	: : "r" (v0) , "r" (v1));
}

void libVu0UnitMatrix(libVu0FMatrix m0)
{
	asm __volatile__(
	"vsub.xyzw	vf4,vf0,vf0 #vf4.xyzw=0; \n\t"
	"vadd.w	vf4,vf4,vf0      \n\t"
	"vmr32.xyzw	vf5,vf4  \n\t"
	"vmr32.xyzw	vf6,vf5  \n\t"
	"vmr32.xyzw	vf7,vf6  \n\t"
	"sqc2    vf4,0x30(%0)    \n\t"
	"sqc2    vf5,0x20(%0)    \n\t"
	"sqc2    vf6,0x10(%0)    \n\t"
	"sqc2    vf7,0x0(%0)     \n\t"
	: : "r" (m0));
}

asm (
	".rdata    \n\t"
 	".align 4  \n\t"
	".local ps2_vu0_ecossin_data  \n\t"
	".type    ps2_vu0_ecossin_data@object \n\t"
        ".size    ps2_vu0_ecossin_data,16  \n"
"vu0ecossin_data:  \n\t"
 	".word	0x362e9c14, 0xb94fb21f, 0x3c08873e, 0xbe2aaaa4 \n\t"
	".previous \n\t"
);


//inline static
void _libVu0ecossin(void)
{	
	asm __volatile__(
	"la	$8,vu0ecossin_data \n\t"
	"lqc2	vf05,0x0($8)	   \n\t"
	"vmr32.w vf06,vf06	   \n\t"
	"vaddx.x vf04,vf00,vf06	   \n\t"
	"vmul.x vf06,vf06,vf06	   \n\t"
	"vmulx.yzw vf04,vf04,vf00# VF04.yzw=0 \n\t"
	"vmulw.xyzw vf08,vf05,vf06 \n\t"
	"vsub.xyzw vf05,vf00,vf00  \n\t"
	"vmulx.xyzw vf08,vf08,vf06 \n\t"
	"vmulx.xyz vf08,vf08,vf06  \n\t"
	"vaddw.x vf04,vf04,vf08	   \n\t"
	"vmulx.xy vf08,vf08,vf06   \n\t"	
	"vaddz.x vf04,vf04,vf08	   \n\t"
	"vmulx.x vf08,vf08,vf06	   \n\t"
	"vaddy.x vf04,vf04,vf08	   \n\t"
	"vaddx.x vf04,vf04,vf08	   \n\t"

	"vaddx.xy vf04,vf05,vf04   \n\t" 

	"vmul.x vf07,vf04,vf04	   \n\t"
	"vsubx.w vf07,vf00,vf07	   \n\t"

	"vsqrt Q,vf07w		   \n\t"
	"vwaitq                    \n\t"

	"vaddq.x vf07,vf00,Q       \n\t"

	"bne	$7,$0,1f           \n\t"
	"vaddx.x vf04,vf05,vf07	   \n\t"
	"b	2f                 \n"
"1:	\n\t"
	"vsubx.x vf04,vf05,vf07    \n"	
"2:	\n\t"

	: : :"$7","$8");
}


void libVu0RotMatrixZ(libVu0FMatrix m0, libVu0FMatrix m1, float rz)
{
	asm __volatile__(
	"mtc1	$0,$f4  \n\t"
	"c.olt.s %2,$f4 \n\t"
	"li.s    $f4,1.57079637050628662109e0 \n\t"	
	"bc1f    1f     \n\t"
	"add.s   %2,$f4,%2   #rx=rx+PI/2 \n\t"
	"li 	$7,1        #cos(rx)=sin(rx+PI/2) \n\t"
	"b	2f      \n"
"1:     \n\t"
	"sub.s   %2,$f4,%2   #rx=PI/2-rx  \n\t"
	"move	$7,$0   \n"
"2:     \n\t"

        "mfc1    $8,%2  \n\t"
        "qmtc2   $8,vf6 \n\t"
	
	: : "r" (m0) , "r" (m1), "f" (rz):"$7","$8","$f4");

	_libVu0ecossin( /* arg: vf6 */ );

	asm __volatile__(
        "vmove.xyzw vf06,vf05    \n\t"
	"vmove.xyzw vf07,vf05    \n\t"
	"vmove.xyzw vf09,vf00    \n\t"
	"vsub.xyz vf09,vf09,vf09 \n\t"
	"vmr32.xyzw vf08,vf09	 \n\t"

	"vsub.zw vf04,vf04,vf04  \n\t"
	"vaddx.y vf06,vf05,vf04  \n\t"
	"vaddy.x vf06,vf05,vf04  \n\t"
	"vsubx.x vf07,vf05,vf04  \n\t"
	"vaddy.y vf07,vf05,vf04  \n\t"

	"li    $7,4              \n"
"10:    \n\t"
	"lqc2    vf4,0x0(%1)     \n\t"
	"vmulax.xyzw	ACC,   vf6,vf4  \n\t"
	"vmadday.xyzw	ACC,   vf7,vf4  \n\t"
	"vmaddaz.xyzw	ACC,   vf8,vf4  \n\t"
	"vmaddw.xyzw	vf5,  vf9,vf4   \n\t"
	"sqc2    vf5,0x0(%0)            \n\t"
	"addi    $7,-1           \n\t"     
	"addi    %1,0x10         \n\t"
	"addi    %0,0x10         \n\t"
	"bne    $0,$7,10b        \n\t"
	
	: : "r" (m0) , "r" (m1), "f" (rz):"$7");
}

void libVu0RotMatrixX(libVu0FMatrix m0, libVu0FMatrix m1, float rx)
{
	asm __volatile__(
	"mtc1	$0,$f4           \n\t"
	"c.olt.s %2,$f4          \n\t"
	"li.s    $f4,1.57079637050628662109e0  \n\t"	
	"bc1f    1f              \n\t"
	"add.s   %2,$f4,%2	 \n\t"
	"li 	$7,1		 \n\t"
	"b	2f               \n"
"1:     \n\t"
	"sub.s   %2,$f4,%2       \n\t"
	"move	$7,$0            \n"
"2:     \n\t"

	"mfc1    $8,%2           \n\t"
	"qmtc2    $8,$vf6        \n\t"
	
	: : "r" (m0) , "r" (m1), "f" (rx):"$7","$8","$f4");

	_libVu0ecossin( /* arg: vf6 */ );

	asm __volatile__(
	"vmove.xyzw vf06,vf05    \n\t"	
	"vmove.xyzw vf07,vf05    \n\t"
	"vmove.xyzw vf08,vf05    \n\t"
	"vmove.xyzw vf09,vf05    \n\t"
	"vaddw.x vf06,vf05,vf00  \n\t"
	"vaddw.w vf09,vf05,vf00  \n\t"

	"vsub.zw vf04,vf04,vf04  \n\t"
	"vaddx.z vf07,vf05,vf04  \n\t"
	"vaddy.y vf07,vf05,vf04  \n\t"
	"vsubx.y vf08,vf05,vf04  \n\t"
	"vaddy.z vf08,vf05,vf04  \n\t"

	"li    $7,4              \n"
"10:    \n\t"
	"lqc2    vf4,0x0(%1)     \n\t"
	"vmulax.xyzw	ACC,   vf6,vf4  \n\t"
	"vmadday.xyzw	ACC,   vf7,vf4  \n\t"
	"vmaddaz.xyzw	ACC,   vf8,vf4  \n\t"
	"vmaddw.xyzw	vf5,  vf9,vf4   \n\t"
	"sqc2    vf5,0x0(%0)     \n\t"
	"addi    $7,-1           \n\t"
	"addi    %1,0x10         \n\t"
	"addi    %0,0x10         \n\t"
	"bne    $0,$7,10b        \n\t"
	
	: : "r" (m0) , "r" (m1), "f" (rx):"$7");
}

void libVu0RotMatrixY(libVu0FMatrix m0, libVu0FMatrix m1, float ry)
{
	asm __volatile__(
	"mtc1	$0,$f4      \n\t"
	"c.olt.s %2,$f4     \n\t"
	"li.s    $f4,1.57079637050628662109e0  \n\t"	
	"bc1f    1f         \n\t"
	"add.s   %2,$f4,%2  \n\t"
	"li 	$7,1        \n\t"
	"b	2f          \n"
"1:     \n\t"
	"sub.s   %2,$f4,%2  \n\t"
	"move	$7,$0       \n"
"2:     \n\t"

	"mfc1    $8,%2      \n\t"
	"qmtc2    $8,vf6    \n\t"
	
	: : "r" (m0) , "r" (m1), "f" (ry):"$7","$8","$f4");

	_libVu0ecossin( /* arg: vf6 */ );

	asm __volatile__(
	"vmove.xyzw vf06,vf05    \n\t"   
	"vmove.xyzw vf07,vf05    \n\t"
	"vmove.xyzw vf08,vf05    \n\t"
	"vmove.xyzw vf09,vf05    \n\t"
	"vaddw.y vf07,vf05,vf00  \n\t"
	"vaddw.w vf09,vf05,vf00  \n\t"

	"vsub.zw vf04,vf04,vf04  \n\t"
	"vsubx.z vf06,vf05,vf04  \n\t"
	"vaddy.x vf06,vf05,vf04  \n\t"
	"vaddx.x vf08,vf05,vf04  \n\t"
	"vaddy.z vf08,vf05,vf04  \n\t"

	"li    $7,4              \n"
"10:    \n\t"
	"lqc2    vf4,0x0(%1)     \n\t"
	"vmulax.xyzw	ACC,   vf6,vf4  \n\t"
	"vmadday.xyzw	ACC,   vf7,vf4  \n\t"
	"vmaddaz.xyzw	ACC,   vf8,vf4  \n\t"
	"vmaddw.xyzw	vf5,  vf9,vf4   \n\t"
	"sqc2    vf5,0x0(%0)            \n\t"
	"addi    $7,-1                  \n\t"
	"addi    %1,0x10                \n\t"
	"addi    %0,0x10                \n\t"
	"bne    $0,$7,10b               \n\t"
	
	: : "r" (m0) , "r" (m1), "f" (ry):"$7");
}

void libVu0RotMatrix(libVu0FMatrix m0, libVu0FMatrix m1, libVu0FVector rot)
{
	libVu0RotMatrixZ(m0, m1, rot[2]);
	libVu0RotMatrixY(m0, m0, rot[1]);
	libVu0RotMatrixX(m0, m0, rot[0]);
}

void libVu0ClampVector(libVu0FVector v0, libVu0FVector v1, float min, float max)
{
	asm __volatile__(
        "mfc1    $8,%2        \n\t"
        "mfc1    $9,%3        \n\t"
	"lqc2    vf6,0x0(%1)  \n\t"
        "qmtc2    $8,vf4      \n\t"
        "qmtc2    $9,vf5      \n\t"
	"vmaxx.xyzw $vf06,$vf06,$vf04  \n\t"
	"vminix.xyzw $vf06,$vf06,$vf05 \n\t"
	"sqc2    vf6,0x0(%0)  \n\t"
	: : "r" (v0) , "r" (v1), "f" (min), "f" (max):"$8","$9");
}

void libVu0CameraMatrix(libVu0FMatrix m, libVu0FVector p, libVu0FVector zd, libVu0FVector yd)
{
	libVu0FMatrix	m0;
	libVu0FVector	xd;

	libVu0UnitMatrix(m0);
	libVu0OuterProduct(xd, yd, zd);
	libVu0Normmalize(m0[0], xd);
	libVu0Normmalize(m0[2], zd);
	libVu0OuterProduct(m0[1], m0[2], m0[0]);
	libVu0TransMatrix(m0, m0, p);
	libVu0InverseMatrix(m, m0);
}
	
void libVu0NormmalLightMatrix(libVu0FMatrix m, libVu0FVector l0, libVu0FVector l1, libVu0FVector l2)
{
        libVu0FVector  t;
        libVu0ScaleVector(t, l0, -1); libVu0Normmalize(m[0], t);
        libVu0ScaleVector(t, l1, -1); libVu0Normmalize(m[1], t);
        libVu0ScaleVector(t, l2, -1); libVu0Normmalize(m[2], t);

	m[3][0] = m[3][1] = m[3][2] = 0.0f;
	m[3][3] = 1.0f;

	libVu0TransposeMatrix(m, m);
}

void libVu0LightColorMatrix(libVu0FMatrix m, libVu0FVector c0, libVu0FVector c1, libVu0FVector c2, libVu0FVector a)
{
	libVu0CopyVector(m[0], c0);
	libVu0CopyVector(m[1], c1);
	libVu0CopyVector(m[2], c2);
	libVu0CopyVector(m[3],  a);
}

void libVu0ViewScreenMatrix(libVu0FMatrix m, float scrz, float ax, float ay,
	       float cx, float cy, float zmin, float zmax, float nearz, float farz)
{
	float	az, cz;
	libVu0FMatrix	mt;

	cz = (-zmax * nearz + zmin * farz) / (-nearz + farz);
	az  = farz * nearz * (-zmin + zmax) / (-nearz + farz);

	//     | scrz    0  0 0 |
	// m = |    0 scrz  0 0 | 
	//     |    0    0  0 1 |
	//     |    0    0  1 0 |
	libVu0UnitMatrix(m);
	m[0][0] = scrz;
	m[1][1] = scrz;
	m[2][2] = 0.0f;
	m[3][3] = 0.0f;
	m[3][2] = 1.0f;
	m[2][3] = 1.0f;

	//     | ax  0  0 cx |
	// m = |  0 ay  0 cy | 
	//     |  0  0 az cz |
	//     |  0  0  0  1 |
	libVu0UnitMatrix(mt);
	mt[0][0] = ax;
	mt[1][1] = ay;
	mt[2][2] = az;
	mt[3][0] = cx;
	mt[3][1] = cy;
	mt[3][2] = cz;

	libVu0MulMatrix(m, mt, m);
	return;
}

void libVu0DropShadowMatrix(libVu0FMatrix m, libVu0FVector lp, float a, float b, float c, int mode)
{
    if (mode) {	// spot light
	float x = lp[0], y = lp[1], z = lp[2];
	float d = (float)1-(a*x+b*y+c*z);

	m[0][0] = a*x+d, m[1][0] = b*x,   m[2][0] = c*x,   m[3][0] = -x;
	m[0][1] = a*y,   m[1][1] = b*y+d, m[2][1] = c*y,   m[3][1] = -y;
	m[0][2] = a*z,   m[1][2] = b*z,   m[2][2] = c*z+d, m[3][2] = -z;
	m[0][3] = a,     m[1][3] = b,     m[2][3] = c,     m[3][3] = d-(float)1;
    }
    else {		// parallel light
	float p  = lp[0], q = lp[1], r = lp[2];
	float n  = a*p+b*q+c*r;
	float nr = -(float)1.0/n;

	m[0][0] = nr*(a*p-n), m[1][0] = nr*(b*p),   m[2][0] = nr*(c*p),   m[3][0] = nr*(-p);
	m[0][1] = nr*(a*q),   m[1][1] = nr*(b*q-n), m[2][1] = nr*(c*q),   m[3][1] = nr*(-q);
	m[0][2] = nr*(a*r),   m[1][2] = nr*(b*r),   m[2][2] = nr*(c*r-n), m[3][2] = nr*(-r);
	m[0][3] = (float)0,          m[1][3] = (float)0,          m[2][3] =(float) 0,          m[3][3] = nr*(-n);
    }
}

void libVu0RotTransPersN(libVu0IVector *v0, libVu0FMatrix m0, libVu0FVector *v1, int n, int mode)
{
	asm __volatile__(
	"lqc2	vf4,0x0(%1)             \n\t"
	"lqc2	vf5,0x10(%1)            \n\t"
	"lqc2	vf6,0x20(%1)            \n\t"
	"lqc2	vf7,0x30(%1)            \n"
"1:     \n\t"
	"lqc2	vf8,0x0(%2)             \n\t"
	"vmulax.xyzw     ACC, vf4,vf8   \n\t"
	"vmadday.xyzw    ACC, vf5,vf8   \n\t"
	"vmaddaz.xyzw    ACC, vf6,vf8   \n\t"
	"vmaddw.xyzw      vf9,vf7,vf8   \n\t"
	"vdiv    Q,vf0w,vf9w            \n\t"
	"vwaitq                         \n\t"
	"vmulq.xyz	vf9,vf9,Q       \n\t"
	"vftoi4.xyzw	vf10,vf9        \n\t"
	"beqz	%4,2f                   \n\t"
	"vftoi0.zw	vf10,vf9	\n"
"2:     \n\t"
	"sqc2	vf10,0x0(%0)            \n\t"

	"addi	%3,-1                   \n\t"
	"addi	%2,0x10                 \n\t"
	"addi	%0,0x10                 \n\t"
	"bne	$0,%3,1b                \n\t"
	: : "r" (v0) , "r" (m0) ,"r" (v1), "r" (n) ,"r" (mode));
}

void libVu0RotTransPers(libVu0IVector v0, libVu0FMatrix m0, libVu0FVector v1, int mode)
{
	asm __volatile__(
	"lqc2	vf4,0x0(%1)             \n\t"
	"lqc2	vf5,0x10(%1)            \n\t"
	"lqc2	vf6,0x20(%1)            \n\t"
	"lqc2	vf7,0x30(%1)            \n\t"
	"lqc2	vf8,0x0(%2)             \n\t"
	"vmulax.xyzw     ACC, vf4,vf8   \n\t"
	"vmadday.xyzw    ACC, vf5,vf8   \n\t"
	"vmaddaz.xyzw    ACC, vf6,vf8   \n\t"
	"vmaddw.xyzw      vf9,vf7,vf8   \n\t"
	"vdiv    Q,vf0w,vf9w            \n\t"
	"vwaitq                         \n\t"
	"vmulq.xyz	vf9,vf9,Q       \n\t"
	"vftoi4.xyzw	vf10,vf9        \n\t"
	"beqz	%3,1f                   \n\t"
	"vftoi0.zw	vf10,vf9        \n"	
"1:     \n\t"
	"sqc2	vf10,0x0(%0)            \n\t"

	: : "r" (v0) , "r" (m0) ,"r" (v1), "r" (mode));
}

void  libVu0CopyVectorXYZ(libVu0FVector v0, libVu0FVector v1)
{
	v0[0]=v1[0];
	v0[1]=v1[1];
	v0[2]=v1[2];
}

void libVu0InterVectorXYZ(libVu0FVector v0, libVu0FVector v1, libVu0FVector v2, float r)
{
	asm __volatile__(
	"lqc2    vf4,0x0(%1)      \n\t"
	"lqc2    vf5,0x0(%2)      \n\t"
	"mfc1    $8,%3            \n\t"
	"qmtc2   $8,vf6           \n\t"

	"vmove.w    vf9,vf4       \n\t"
	"vaddw.x    vf7,vf0,vf0   \n\t"
	"vsub.x     vf8,vf7,vf6   \n\t"
	"vmulax.xyz ACC,vf4,vf6   \n\t"
	"vmaddx.xyz	vf9,vf5,vf8  \n\t"

	"sqc2    vf9,0x0(%0)      \n\t"
	: : "r" (v0) , "r" (v1), "r" (v2), "f" (r) : "$8");
}

void  libVu0ScaleVectorXYZ(libVu0FVector v0, libVu0FVector v1, float s)
{
	asm __volatile__(
	"lqc2      vf4,0x0(%1)    \n\t"
	"mfc1      $8,%2          \n\t"
	"qmtc2     $8,vf5         \n\t"
	"vmulx.xyz vf4,vf4,vf5    \n\t"
	"sqc2      vf4,0x0(%0)    \n\t"
	: : "r" (v0) , "r" (v1), "f" (s):"$8");
}

int libVu0ClipScreen(libVu0FVector v0)
{
	register int ret;

	asm __volatile__(
	"vsub.xyzw  vf04,vf00,vf00    \n\t"
	".set push                    \n\t"
	".set mips3                   \n\t"
	"li     %0,0x4580000045800000 \n\t"
	".set pop                     \n\t"
	"lqc2    vf07,0x0(%1)         \n\t"

	"qmtc2  %0,vf06               \n\t"
	"ctc2    $0,$vi16             \n\t"

	"vsub.xyw   vf05,vf07,vf04    \n\t"
        "vsub.xy    vf05,vf06,vf07    \n\t"

	"vnop                         \n\t"
	"vnop                         \n\t"
	"vnop                         \n\t"
	"vnop                         \n\t"
        "vnop                         \n\t"
        "cfc2    %0,$vi16             \n\t"
        "andi	%0,%0,0xc0            \n\t"

	:"=r"(ret): "r" (v0));
	return ret;
}

int libVu0ClipScreen3(libVu0FVector v0, libVu0FVector v1, libVu0FVector v2)
{
	register int ret;

	asm __volatile__(
	"vsub.xyzw  vf04,vf00,vf00     \n\t"
	".set push                     \n\t"
	".set mips3                    \n\t"
	"li     %0,0x4580000045800000  \n\t"
	".set pop                      \n\t"
	"lqc2    vf06,0x0(%1)          \n\t"
	"lqc2    vf08,0x0(%2)          \n\t"
	"lqc2    vf09,0x0(%3)          \n\t"

	"qmtc2  %0,vf07                \n\t"
	"ctc2    $0,$vi16              \n\t"  

	"vsub.xyw  vf05,vf06,vf04      \n\t"
	"vsub.xy    vf05,vf07,vf06     \n\t"
	"vsub.xyw   vf05,vf08,vf04     \n\t"
	"vsub.xy    vf05,vf07,vf08     \n\t"
	"vsub.xyw   vf05,vf09,vf04     \n\t"
	"vsub.xy    vf05,vf07,vf09     \n\t"

	"vnop                          \n\t"
        "vnop                          \n\t"
        "vnop                          \n\t"
        "vnop                          \n\t"
        "vnop                          \n\t"
        "cfc2    %0,$vi16              \n\t"
	"andi	%0,%0,0xc0             \n\t"

	:"=r"(ret): "r" (v0), "r" (v1), "r" (v2) );
	return ret;
}

#define UNDER_X		 1
#define UNDER_Y		 2
#define UNDER_W		 4
#define OVER_X		 8
#define OVER_Y		16
#define OVER_W		32
int libVu0ClipAll(libVu0FVector minv, libVu0FVector maxv, libVu0FMatrix ms, libVu0FVector *vm, int n)
{
	register int ret;

	asm __volatile__(
	"lqc2    vf8,0x0(%4)         \n\t"
	"lqc2    vf4,0x0(%3)         \n\t"
	"lqc2    vf5,0x10(%3)        \n\t"
	"lqc2    vf6,0x20(%3)        \n\t"
	"lqc2    vf7,0x30(%3)        \n\t"

	"lqc2    vf9,0x0(%1)	     \n\t"
	"lqc2    vf10,0x0(%2)	     \n\t"
	"lqc2    vf11,0x0(%1)	     \n\t"
	"lqc2    vf12,0x0(%2)	     \n"

"1:     \n\t"
	"vmulax.xyzw	ACC,vf4,vf8  \n\t"
	"vmadday.xyzw	ACC,vf5,vf8  \n\t"
	"vmaddaz.xyzw	ACC,vf6,vf8  \n\t"
	"vmaddw.xyzw	vf8,vf7,vf8  \n\t"

	"vmulw.xyz    vf11,vf9,vf8   \n\t"
	"vmulw.xyz    vf12,vf10,vf8  \n\t"

	"vnop			     \n\t"	
	"vnop                        \n\t"
        "ctc2    $0,$vi16            \n\t"
        "vsub.xyw   vf11,vf8,vf11    \n\t" 
	"vsub.xyw   vf12,vf12,vf8    \n\t"
	"vmove.w    vf11,vf9         \n\t"
	"vmove.w    vf12,vf10        \n\t"
	"vnop                        \n\t"
        "addi	%4,0x10              \n\t"
	"lqc2    vf8,0x0(%4)         \n\t"
	"addi	%5,-1                \n\t"
	"cfc2    %0,$vi16            \n\t"
	"andi	%0,$2,0xc0           \n\t"
	"beqz	%0,2f                \n\t"

	"bne	$0,%5,1b             \n\t"

	"addi	%0,$0,1              \n"
"2:     \n\t"

	:"=r"(ret): "r" (minv),"r" (maxv),"r" (ms),"r" (vm),"r" (n)  );
	return ret;
}

