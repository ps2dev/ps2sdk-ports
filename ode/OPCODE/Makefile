LIBPATH=../lib
LIB_OPCODE=$(LIBPATH)/libopcode.a

EE_CFLAGS = -DDISABLE_TIMERS
EE_CFLAGS += -DNO_SIGNAL_H -DDEBUG_ERROR -DDEBUG_VIDEO -DUSE_MATH_H
EE_CFLAGS += -DENABLE_PS2SDK

OPCODE_OBJS  = OPC_AABBCollider.o OPC_AABBTree.o OPC_BaseModel.o \
	OPC_BoxPruning.o OPC_Collider.o OPC_Common.o OPC_HybridModel.o \
	OPC_LSSCollider.o OPC_MeshInterface.o OPC_Model.o \
	OPC_OBBCollider.o OPC_OptimizedTree.o OPC_Picking.o \
	OPC_PlanesCollider.o OPC_RayCollider.o OPC_SphereCollider.o \
	OPC_SweepAndPrune.o OPC_TreeBuilders.o OPC_TreeCollider.o \
	OPC_VolumeCollider.o Opcode.o StdAfx.o

EE_INCS  = -I../include 

all: $(LIB_OPCODE) 

$(LIB_OPCODE): $(OPCODE_OBJS)
	$(EE_AR) cru $(LIB_OPCODE) $(OPCODE_OBJS)

install: all
	mkdir -p $(PS2SDK)/ports/include/ode
	mkdir -p $(PS2SDK)/ports/lib
	cp -f $(LIB_OPCODE) $(PS2SDK)/ports/lib

clean:
	rm -f $(LIB_OPCODE) $(OPCODE_OBJS)

include ../Makefile.pref
include ../Makefile.eeglobal

