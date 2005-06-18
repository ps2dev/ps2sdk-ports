LIBPATH=../../lib
LIB_ODE=$(LIBPATH)/libode.a

EE_CFLAGS = -DDISABLE_TIMERS
EE_CFLAGS += -DNO_SIGNAL_H -DDEBUG_ERROR -DDEBUG_VIDEO -DUSE_MATH_H
EE_CFLAGS += -DENABLE_PS2SDK

ODE_OBJS  = fastdot.o fastldlt.o fastlsolve.o fastltsolve.o array.o collision_kernel.o \
collision_quadtreespace.o collision_space.o collision_std.o collision_transform.o \
collision_trimesh_box.o collision_trimesh_ccylinder.o \
collision_trimesh_distance.o collision_trimesh_ray.o collision_trimesh_sphere.o \
collision_trimesh_trimesh.o collision_util.o error.o export-dif.o joint.o lcp.o \
mass.o mat.o matrix.o memory.o misc.o obstack.o ode.o odemath.o quickstep.o \
rotation.o step.o stepfast.o testing.o timer.o util.o

EE_INCS  = -I../../include -I../../OPCODE

all: $(LIB_ODE) 

$(LIB_ODE): $(ODE_OBJS)
	$(EE_AR) cru $(LIB_ODE) $(ODE_OBJS)

install: all
	mkdir -p $(PS2SDK)/ports/include/ode
	mkdir -p $(PS2SDK)/ports/lib
	cp -f $(LIB_ODE) $(PS2SDK)/ports/lib
	cp -f ../../include/ode/*.h $(PS2SDK)/ports/include/ode

clean:
	rm -f $(LIB_ODE) $(LIB_ODEMAIN) $(ODE_OBJS)

include ../../Makefile.pref
include ../../Makefile.eeglobal

