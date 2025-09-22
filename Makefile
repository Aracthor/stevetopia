uniq=	$(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1)))

CXX=	g++
EMXX=	em++
MKDIR=	mkdir
RM=	rm -f
RMDIR=	rm -fd

SRCS_DIR=	srcs/
INC_DIR=	srcs/
OBJS_DIR=	objs/
HATCHER_DIR=	hatcher/hatcher/
IMGUI_REPO=	imgui/
BIN_DIR=	bin/
NATIVE_DIR=	native/
WEBASM_DIR=	webasm/
RELEASE_DIR=	release/
DEBUG_DIR=	debug/

SRCS_FILES=	Components/InventoryComponent.cpp			\
		Components/MovementComponent.cpp			\
		Components/NameComponent.cpp				\
									\
		Updaters/ActionPlanningUpdater.cpp			\
		Updaters/ForestUpdater.cpp				\
		Updaters/GrowableUpdater.cpp				\
		Updaters/HarvestableUpdater.cpp				\
		Updaters/InventoryUpdater.cpp				\
		Updaters/MovingEntitiesUpdater.cpp			\
		Updaters/ObstacleUpdater.cpp				\
		Updaters/WorkerUpdater.cpp				\
									\
		RenderComponents/ItemDisplayComponent.cpp		\
									\
		RenderUpdaters/BlueprintRenderUpdater.cpp		\
		RenderUpdaters/CameraRenderUpdater.cpp			\
		RenderUpdaters/DemoImguiRenderUpdater.cpp		\
		RenderUpdaters/DebugShortcutsRenderUpdater.cpp		\
		RenderUpdaters/EntityCreatorRenderUpdater.cpp		\
		RenderUpdaters/FPSPanelRenderUpdater.cpp		\
		RenderUpdaters/GridRenderUpdater.cpp			\
		RenderUpdaters/GridControlPanelRenderUpdater.cpp	\
		RenderUpdaters/InventoryPanelRenderUpdater.cpp		\
		RenderUpdaters/MoveOrderRenderUpdater.cpp		\
		RenderUpdaters/SelectionRectangleRenderUpdater.cpp	\
		RenderUpdaters/SelectedRenderUpdater.cpp	 	\
		RenderUpdaters/StaticMeshRenderUpdater.cpp 		\
		RenderUpdaters/SteveRenderUpdater.cpp			\
		RenderUpdaters/WorldTickerRenderUpdater.cpp		\
									\
		WorldComponents/Blueprint.cpp				\
		WorldComponents/Camera.cpp				\
		WorldComponents/SquareGrid.cpp				\
									\
		utils/EntityFinder.cpp					\
		utils/Pathfinding.cpp					\
		utils/TransformationHelper.cpp				\
									\
		EntityDescriptors.cpp					\
		main.cpp						\

NATIVE_NAME=	exec

OBJS_NATIVE_DIR=		$(OBJS_DIR)$(NATIVE_DIR)
OBJS_NATIVE_RELEASE_DIR=	$(OBJS_NATIVE_DIR)$(RELEASE_DIR)
OBJS_NATIVE_DEBUG_DIR=		$(OBJS_NATIVE_DIR)$(DEBUG_DIR)
OBJS_WEBASM_DIR=		$(OBJS_DIR)$(WEBASM_DIR)
OBJS_WEBASM_RELEASE_DIR=	$(OBJS_WEBASM_DIR)$(RELEASE_DIR)
OBJS_WEBASM_DEBUG_DIR=		$(OBJS_WEBASM_DIR)$(DEBUG_DIR)
NATIVE_BIN_DIR=			$(BIN_DIR)
WEBASM_BIN_DIR=			$(BIN_DIR)

SRCS_DIRS=		$(call uniq,$(dir $(SRCS_FILES)))

# Order matters here : subfolders must be on top of their root folder, because it is the order used to delete them.
OBJS_DIRS=		$(SRCS_DIRS:%=$(OBJS_NATIVE_RELEASE_DIR)%)	\
			$(SRCS_DIRS:%=$(OBJS_NATIVE_DEBUG_DIR)%)	\
			$(SRCS_DIRS:%=$(OBJS_WEBASM_RELEASE_DIR)%)	\
			$(SRCS_DIRS:%=$(OBJS_WEBASM_DEBUG_DIR)%)	\
			$(OBJS_NATIVE_DIR)				\
			$(OBJS_WEBASM_DIR)				\
			$(OBJS_DIR)					\

BIN_DIRS=		$(NATIVE_BIN_DIR)	\
			$(WEBASM_BIN_DIR)	\


HATCHER_LIBDIR=		$(HATCHER_DIR)$(BIN_DIR)
HATCHER_LIBNAME=	libhatcher
HATCHER_NATIVE_RELEASE=	$(HATCHER_DIR)$(BIN_DIR)$(HATCHER_LIBNAME)_release.a
HATCHER_NATIVE_DEBUG=	$(HATCHER_DIR)$(BIN_DIR)$(HATCHER_LIBNAME)_debug.a
HATCHER_WEBASM_RELEASE=	$(HATCHER_DIR)$(BIN_DIR)$(HATCHER_LIBNAME)_webasm_release.a
HATCHER_WEBASM_DEBUG=	$(HATCHER_DIR)$(BIN_DIR)$(HATCHER_LIBNAME)_webasm_debug.a
IMGUI_LIBNAME=		libimgui
IMGUI_NATIVE=		$(HATCHER_DIR)$(BIN_DIR)$(IMGUI_LIBNAME)_native.a
IMGUI_WEBASM=		$(HATCHER_DIR)$(BIN_DIR)$(IMGUI_LIBNAME)_webasm.a
HATCHER_BINS=		$(HATCHER_NATIVE_RELEASE)	\
			$(HATCHER_NATIVE_DEBUG)		\
			$(HATCHER_WEBASM_RELEASE)	\
			$(HATCHER_WEBASM_DEBUG)		\
			$(IMGUI_NATIVE)			\
			$(IMGUI_WEBASM)			\


CXX_COMMON_FLAGS=	-Wall -Werror			\
			-std=c++17			\
			-I $(INC_DIR)			\
			-I $(HATCHER_DIR)$(INC_DIR)	\
			-I $(HATCHER_DIR)$(IMGUI_REPO)	\

CXX_RELEASE_FLAGS=	$(CXX_COMMON_FLAGS)	\
			-O3

CXX_DEBUG_FLAGS=	$(CXX_COMMON_FLAGS)	\
			-g3			\
			-DNDEBUG		\

CXX_NATIVE_FLAGS=

EMXX_FLAGS=		-fexceptions

LD_NATIVE_COMMON_FLAGS=	-lSDL2 -lGL -lGLEW -ldl

LD_NATIVE_RELEASE_FLAGS=$(HATCHER_NATIVE_RELEASE)	\
			$(IMGUI_NATIVE)			\
			$(LD_NATIVE_COMMON_FLAGS)	\
			-O3

LD_NATIVE_DEBUG_FLAGS=	$(HATCHER_NATIVE_DEBUG)		\
			$(IMGUI_NATIVE)			\
			$(LD_NATIVE_COMMON_FLAGS)	\
			-g3

LD_WEBASM_COMMON_FLAGS=	-s WASM=1	\
			-s USE_SDL=2 	\
			-fexceptions	\
			-s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=0	\
			-s MIN_WEBGL_VERSION=2					\
			-s MAX_WEBGL_VERSION=2					\
			-s FULL_ES3=1 						\
			--preload-file assets/					\
			--preload-file shaders/					\

LD_WEBASM_RELEASE_FLAGS=$(HATCHER_WEBASM_RELEASE)	\
			$(IMGUI_WEBASM)			\
			$(LD_WEBASM_COMMON_FLAGS)	\
			-O3

LD_WEBASM_DEBUG_FLAGS=	$(HATCHER_WEBASM_DEBUG)		\
			$(IMGUI_WEBASM)			\
			$(LD_WEBASM_COMMON_FLAGS)	\
			-g3				\
			-s DEMANGLE_SUPPORT=1 		\
			-s GL_ASSERTIONS=1 		\
			-s GL_DEBUG=1 			\


SRCS=   	$(addprefix $(SRCS_DIR),$(SRCS_FILES))

OBJS_NATIVE_RELEASE=	$(SRCS:$(SRCS_DIR)%.cpp=$(OBJS_NATIVE_RELEASE_DIR)%.o)
OBJS_NATIVE_DEBUG=	$(SRCS:$(SRCS_DIR)%.cpp=$(OBJS_NATIVE_DEBUG_DIR)%.o)
OBJS_WEBASM_RELEASE=	$(SRCS:$(SRCS_DIR)%.cpp=$(OBJS_WEBASM_RELEASE_DIR)%.o)
OBJS_WEBASM_DEBUG=	$(SRCS:$(SRCS_DIR)%.cpp=$(OBJS_WEBASM_DEBUG_DIR)%.o)
OBJS=			$(OBJS_NATIVE_RELEASE)	\
			$(OBJS_NATIVE_DEBUG)	\
			$(OBJS_WEBASM_RELEASE)	\
			$(OBJS_WEBASM_DEBUG)	\

DEPS=			$(OBJS:.o=.dep)

BIN_NATIVE_RELEASE=	$(NATIVE_BIN_DIR)$(NATIVE_NAME)_release
BIN_NATIVE_DEBUG=	$(NATIVE_BIN_DIR)$(NATIVE_NAME)_debug
BIN_WEBASM_RELEASE=	$(WEBASM_BIN_DIR)$(NATIVE_NAME)_release.js
BIN_WEBASM_DEBUG=	$(WEBASM_BIN_DIR)$(NATIVE_NAME)_debug.js
RESIDUE_WEBASM_RELASE=	$(BIN_WEBASM_RELEASE:%.js=%.worker.js) $(BIN_WEBASM_RELEASE:%.js=%.wasm) $(BIN_WEBASM_RELEASE:%.js=%.data)
RESIDUE_WEBASM_DEBUG=	$(BIN_WEBASM_DEBUG:%.js=%.worker.js) $(BIN_WEBASM_DEBUG:%.js=%.wasm) $(BIN_WEBASM_DEBUG:%.js=%.data)
BINS=			$(BIN_NATIVE_RELEASE)	\
			$(BIN_NATIVE_DEBUG)	\
			$(BIN_WEBASM_RELEASE)	\
			$(BIN_WEBASM_DEBUG)	\

RESIDUE_BINS=		$(RESIDUE_WEBASM_RELASE)\
			$(RESIDUE_WEBASM_DEBUG)	\



.SECONDEXPANSION:

include $(shell test -d $(OBJS_DIR) && find $(OBJS_DIR) -name "*.dep")

%/:
	$(MKDIR) -p $@


$(OBJS_NATIVE_DIR):		| $$(dir $$(@D))
$(OBJS_NATIVE_RELEASE_DIR):	| $$(dir $$(@D))
$(OBJS_NATIVE_DEBUG_DIR):	| $$(dir $$(@D))

$(OBJS_NATIVE_RELEASE_DIR)%.o:	$(SRCS_DIR)%.cpp | $$(@D)/
	$(CXX) $(CXX_NATIVE_FLAGS) $(CXX_RELEASE_FLAGS) -MMD -MF $(@:.o=.dep) -c $< -o $@

$(OBJS_NATIVE_DEBUG_DIR)%.o:	$(SRCS_DIR)%.cpp | $$(@D)/
	$(CXX) $(CXX_NATIVE_FLAGS) $(CXX_DEBUG_FLAGS) -MMD -MF $(@:.o=.dep) -c $< -o $@


$(OBJS_WEBASM_DIR):		| $$(dir $$(@D))
$(OBJS_WEBASM_RELEASE_DIR):	| $$(dir $$(@D))
$(OBJS_WEBASM_DEBUG_DIR):	| $$(dir $$(@D))

$(OBJS_WEBASM_RELEASE_DIR)%.o:	$(SRCS_DIR)%.cpp | $$(@D)/
	$(EMXX) $(EMXX_FLAGS) $(CXX_RELEASE_FLAGS) -MMD -MF $(@:.o=.dep) -c $< -o $@

$(OBJS_WEBASM_DEBUG_DIR)%.o:	$(SRCS_DIR)%.cpp | $$(@D)/
	$(EMXX) $(EMXX_FLAGS) $(CXX_DEBUG_FLAGS) -MMD -MF $(@:.o=.dep) -c $< -o $@


$(HATCHER_BINS):
	$(MAKE) $(@:$(HATCHER_DIR)%=%) -C $(HATCHER_DIR)


$(BIN_NATIVE_RELEASE):	$(HATCHER_NATIVE_RELEASE) $(OBJS_NATIVE_RELEASE) | $$(@D)/
	$(CXX) $(OBJS_NATIVE_RELEASE) -o $(BIN_NATIVE_RELEASE) $(LD_NATIVE_RELEASE_FLAGS)

$(BIN_NATIVE_DEBUG):	$(HATCHER_NATIVE_DEBUG) $(OBJS_NATIVE_DEBUG) | $$(@D)/
	$(CXX) $(OBJS_NATIVE_DEBUG) -o $(BIN_NATIVE_DEBUG) $(LD_NATIVE_DEBUG_FLAGS)


$(BIN_WEBASM_RELEASE):	$(HATCHER_WEBASM_RELEASE) $(OBJS_WEBASM_RELEASE) | $$(@D)/
	$(EMXX) $(OBJS_WEBASM_RELEASE) -o $(BIN_WEBASM_RELEASE) $(LD_WEBASM_RELEASE_FLAGS)

$(BIN_WEBASM_DEBUG):	$(HATCHER_WEBASM_DEBUG) $(OBJS_WEBASM_DEBUG) | $$(@D)/
	$(EMXX) $(OBJS_WEBASM_DEBUG) -o $(BIN_WEBASM_DEBUG) $(LD_WEBASM_DEBUG_FLAGS)


all:	$(BINS)

clean:
	$(MAKE) clean -C $(HATCHER_DIR)
	$(RM) $(OBJS) $(DEPS)
	$(RMDIR) $(OBJS_DIRS:%./=%)

fclean:
	$(MAKE) fclean -C $(HATCHER_DIR)
	$(RM) $(OBJS) $(DEPS)
	$(RMDIR) $(OBJS_DIRS:%./=%)
	$(RM) $(BINS)
	$(RM) $(RESIDUE_BINS)
	$(RMDIR) $(BIN_DIRS)

re:	fclean native_release

# aliases command
native_release:	$(BIN_NATIVE_RELEASE)
native_debug:	$(BIN_NATIVE_DEBUG)
webasm_release:	$(BIN_WEBASM_RELEASE)
webasm_debug:	$(BIN_WEBASM_DEBUG)

.DEFAULT_GOAL=	native_release
