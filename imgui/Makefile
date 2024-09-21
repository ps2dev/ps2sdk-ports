EE_SRC_DIR = ../build/imgui
EE_LIB_DIR = lib/

EE_INCS += -I$(EE_SRC_DIR) -I$(PS2DEV)/gsKit/include -I$(PS2SDK)/ee/include
EE_LIBS += -L$(PS2DEV)/gskit/lib -I$(PS2SDK)/ee/lib -ldma -ldmakit -lgsKit -lgsKit_toolkit -lpad
EE_LIB = $(EE_LIB_DIR)libimgui.a
EE_OBJS = $(EE_SRC_DIR)/imgui.o $(EE_SRC_DIR)/imgui_draw.o $(EE_SRC_DIR)/imgui_tables.o $(EE_SRC_DIR)/imgui_widgets.o $(EE_SRC_DIR)/backends/imgui_impl_ps2sdk.o $(EE_SRC_DIR)/backends/imgui_impl_ps2gskit.o

all: $(EE_LIB_DIR) $(EE_LIB)

$(EE_LIB_DIR):
	mkdir -p $(EE_LIB_DIR)

install: all
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f $(EE_LIB) $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f $(EE_SRC_DIR)/*.h $(DESTDIR)$(PS2SDK)/ports/include
	cp -f $(EE_SRC_DIR)/backends/imgui_impl_ps2sdk.h $(DESTDIR)$(PS2SDK)/ports/include
	cp -f $(EE_SRC_DIR)/backends/imgui_impl_ps2gskit.h $(DESTDIR)$(PS2SDK)/ports/include

clean:
	rm -f -r $(EE_OBJS) $(EE_LIB_DIR)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal_cpp
