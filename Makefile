################################################################################
############################### SM64PC Makefile ################################
################################################################################

## Default target ##
default: all

################################ Build Options #################################

## These options can either be changed by modifying the makefile, or
## by building with 'make SETTING=value'. 'make clean' may be required.

# Build debug version
DEBUG ?= 0
# Version of the game to build
VERSION ?= us
# Graphics microcode used
GRUCODE ?= f3dex2e
# Build for Nintendo Switch
TARGET_SWITCH ?= 0

# Makeflag to enable OSX fixes
OSX_BUILD ?= 0
# Specify the target you are building for, TARGET_BITS=0 means native
TARGET_ARCH ?= native
TARGET_BITS ?= 0
# Disable better camera
BETTERCAMERA = 0
# Enable no drawing distance by default
NODRAWINGDISTANCE ?= 1
# Disable texture fixes by default (helps with them purists)
TEXTURE_FIX ?= 0
# Enable Discord Rich Presence
DISCORDRPC ?= 0
# Various workarounds for weird toolchains
NO_BZERO_BCOPY ?= 0
NO_LDIV ?= 0

# BUILD_DIR is location where all build artifacts are placed
BUILD_DIR_BASE := build

## Backend selection

# Renderers: GL, GL_LEGACY, D3D11, D3D12
RENDER_API ?= GL
# Window managers: SDL2, DXGI (forced if D3D11 or D3D12 in RENDER_API)
WINDOW_API ?= SDL2
# Audio backends: SDL2
AUDIO_API ?= SDL2
# Controller backends (can have multiple, space separated): SDL2
CONTROLLER_API ?= SDL2

################################# OS Detection #################################

WINDOWS_BUILD ?= 0

ifeq ($(OS),Windows_NT)
  HOST_OS ?= Windows
else
  HOST_OS ?= $(shell uname -s 2>/dev/null || echo Unknown)
  # some weird MINGW/Cygwin env that doesn't define $OS
  ifneq (,$(findstring MINGW,HOST_OS))
    HOST_OS := Windows
  endif
endif

ifeq ($(TARGET_SWITCH),0)
  ifeq ($(HOST_OS),Windows)
    WINDOWS_BUILD := 1
  endif
endif

# MXE overrides
ifeq ($(WINDOWS_BUILD),1)
  ifeq ($(CROSS),i686-w64-mingw32.static-)
    TARGET_ARCH = i386pe
    TARGET_BITS = 32
    NO_BZERO_BCOPY := 1
  else ifeq ($(CROSS),x86_64-w64-mingw32.static-)
    TARGET_ARCH = i386pe
    TARGET_BITS = 64
    NO_BZERO_BCOPY := 1
  endif
endif

# macOS overrides
ifeq ($(HOST_OS),Darwin)
  OSX_BUILD := 1
  # Using MacPorts?
  ifeq ($(shell test -d /opt/local/lib && echo y),y)
    OSX_GCC_VER = $(shell find /opt/local/bin/gcc* | grep -oE '[[:digit:]]+' | sort -n | uniq | tail -1)
    CC := gcc-mp-$(OSX_GCC_VER)
    CXX := g++-mp-$(OSX_GCC_VER)
    CPP := cpp-mp-$(OSX_GCC_VER) -P
    PLATFORM_CFLAGS := -I /opt/local/include
    PLATFORM_LDFLAGS := -L /opt/local/lib
  else
    # Using Homebrew?
    ifeq ($(shell which brew >/dev/null 2>&1 && echo y),y)
      OSX_GCC_VER = $(shell find `brew --prefix`/bin/gcc* | grep -oE '[[:digit:]]+' | sort -n | uniq | tail -1)
      CC := gcc-$(OSX_GCC_VER)
      CXX := g++-$(OSX_GCC_VER)
      CPP := cpp-$(OSX_GCC_VER) -P
      PLATFORM_CFLAGS := -I /usr/local/include
      PLATFORM_LDFLAGS := -L /usr/local/lib
    else
      $(error No suitable macOS toolchain found, have you installed Homebrew?)
    endif
  endif
endif

ifneq ($(TARGET_BITS),0)
  BITS := -m$(TARGET_BITS)
endif

# Release (version) flag defs
VERSION_DEF := VERSION_US

TARGET := saturn.$(VERSION)
VERSION_CFLAGS := -D$(VERSION_DEF) -D_LANGUAGE_C

# Stuff for showing the git hash in the title bar
GIT_HASH := $(shell git rev-parse --short HEAD)
GIT_BRANCH := $(shell git rev-parse --abbrev-ref HEAD)
VERSION_CFLAGS += -DGIT_HASH="\"$(GIT_HASH)\""

ifeq ($(shell git rev-parse --abbrev-ref HEAD),nightly)
  VERSION_CFLAGS += -DNIGHTLY
endif

GRUCODE_DEF := F3DEX_GBI_2E
TARGET := $(TARGET).f3dex2e

GRUCODE_CFLAGS := -D$(GRUCODE_DEF)

# Default build is for PC now
VERSION_CFLAGS := $(VERSION_CFLAGS) -DNON_MATCHING -DAVOID_UB

ifeq ($(TARGET_SWITCH),1)
  VERSION_CFLAGS += -DTARGET_SWITCH -DLUA_USE_LINUX
endif
ifeq ($(OSX_BUILD),1) # Modify GFX & SDL2 for OSX GL
  VERSION_CFLAGS += -DOSX_BUILD
endif

# Check backends

ifneq (,$(filter $(RENDER_API),D3D11 D3D12))
  ifneq ($(WINDOWS_BUILD),1)
    $(error DirectX is only supported on Windows)
  endif
  ifneq ($(WINDOW_API),DXGI)
    $(warning DirectX renderers require DXGI, forcing WINDOW_API value)
    WINDOW_API := DXGI
  endif
else
  ifeq ($(WINDOW_API),DXGI)
    $(error DXGI can only be used with DirectX renderers)
  endif
endif

############################ Universal Dependencies ############################

# (This is a bit hacky, but a lot of rules implicitly depend
# on tools and assets, and we use directory globs further down
# in the makefile that we want should cover assets.)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),cleantools)
ifneq ($(MAKECMDGOALS),distclean)

# Make sure assets exist
NOEXTRACT ?= 0
ifeq ($(NOEXTRACT),0)
DUMMY != ./tools/extract_assets.py $(VERSION) >&2 || echo FAIL
ifeq ($(DUMMY),FAIL)
  $(error Failed to extract assets)
endif
endif

# Make tools if out of date
DUMMY != CC=$(CC) CXX=$(CXX) $(MAKE) -C tools >&2 || echo FAIL
ifeq ($(DUMMY),FAIL)
  $(error Failed to build tools)
endif

endif
endif
endif

######################### Target Executable and Sources ########################

ifeq ($(TARGET_SWITCH),1)
  BUILD_DIR := $(BUILD_DIR_BASE)/$(VERSION)_nx
else
  BUILD_DIR := $(BUILD_DIR_BASE)/$(VERSION)_pc
endif

ifeq ($(WINDOWS_BUILD),1)
  EXE := $(BUILD_DIR)/$(TARGET).exe
else # Linux builds/binary namer
  EXE := $(BUILD_DIR)/$(TARGET)
endif

ifeq ($(WINDOWS_BUILD),1)
  VERSION_CFLAGS += -DDISABLE_CURL_SUPPORT
endif

################################
#      Moon64 Source Code      #
################################

MOON_SRC := $(shell find src/ -type d -not -path "src/game/behaviors") $(shell find libs/ -type d)
SRC_DIRS :=  actors levels assets assets/anims $(MOON_SRC)

################################

ifeq ($(DISCORDRPC),1)
  ifneq ($(TARGET_SWITCH),0)
    $(echo Discord RPC does not work on this target)
    DISCORDRPC := 0
  endif
endif

ifeq ($(DEBUG),1)
  OPT_FLAGS := -g
else
  OPT_FLAGS := -O2
endif

# Set BITS (32/64) to compile for
OPT_FLAGS += $(BITS)

# Source code files
LEVEL_C_FILES := $(wildcard levels/*/leveldata.c) $(wildcard levels/*/script.c) $(wildcard levels/*/geo.c)
LEVEL_DIRS := $(patsubst levels/%,%,$(dir $(wildcard levels/*/header.h)))
C_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c)) $(LEVEL_C_FILES)
CXX_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
C_FILES := $(filter-out src/game/main.c,$(C_FILES))

# Object files
O_FILES :=  $(foreach file,$(C_FILES),$(BUILD_DIR)/$(file:.c=.o)) \
            $(foreach file,$(CXX_FILES),$(BUILD_DIR)/$(file:.cpp=.o)) \
            $(foreach file,$(S_FILES),$(BUILD_DIR)/$(file:.s=.o))

GODDARD_O_FILES := $(foreach file,$(GODDARD_C_FILES),$(BUILD_DIR)/$(file:.c=.o))

RPC_LIBS :=
ifeq ($(DISCORDRPC),1)
  ifeq ($(WINDOWS_BUILD),1)
    RPC_LIBS := libs/discord/libdiscord-rpc.dll
  else ifeq ($(OSX_BUILD),1)
    # needs testing
    RPC_LIBS := libs/discord/libdiscord-rpc.dylib
  else
    RPC_LIBS := libs/discord/libdiscord-rpc.so
  endif
endif

# Automatic dependency files
DEP_FILES := $(O_FILES:.o=.d)

############################ Compiler Options ##################################

INCLUDE_CFLAGS := -I include -I $(BUILD_DIR) -I $(BUILD_DIR)/include -I src -I .
ENDIAN_BITWIDTH := $(BUILD_DIR)/endian-and-bitwidth

# Huge deleted N64 section was here

ifeq ($(TARGET_SWITCH),1)
  ifeq ($(strip $(DEVKITPRO)),)
    $(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
  endif
  export PATH := $(DEVKITPRO)/devkitA64/bin:$(PATH)
  include $(DEVKITPRO)/devkitA64/base_tools
  NXPATH := $(DEVKITPRO)/portlibs/switch/bin
  PORTLIBS ?= $(DEVKITPRO)/portlibs/switch
  LIBNX ?= $(DEVKITPRO)/libnx
  CROSS ?= aarch64-none-elf-
  SDLCROSS := $(NXPATH)/
  CC := $(CROSS)gcc
  CXX := $(CROSS)g++
  STRIP := $(CROSS)strip
  NXARCH	:=	-march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE
  APP_TITLE := Saturn: Moon Edition
  APP_AUTHOR := Llennpie
  APP_VERSION := $(GIT_BRANCH) $(GIT_HASH)
  APP_ICON := $(CURDIR)/textures/logo/saturn-logo.jpg
  INCLUDE_CFLAGS += -I$(LIBNX)/include -I$(PORTLIBS)/include
  OPT_FLAGS := -O2
  LIBDIRS	:= $(PORTLIBS) $(LIBNX)
  export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)
endif

# for some reason sdl-config in dka64 is not prefixed, while pkg-config is
SDLCROSS ?=
CC ?= $(CROSS)gcc
CXX ?= $(CROSS)g++
LD := $(CC)

ifeq ($(DISCORDRPC),1)
  LD := $(CXX)
else ifeq ($(WINDOWS_BUILD),1)
  ifeq ($(CROSS),i686-w64-mingw32.static-) # fixes compilation in MXE on Linux and WSL
    LD := $(CC)
  else ifeq ($(CROSS),x86_64-w64-mingw32.static-)
    LD := $(CC)
  else
    LD := $(CXX)
  endif
endif

ifeq ($(WINDOWS_BUILD),1) # fixes compilation in MXE on Linux and WSL
  CPP := cpp -P
  OBJCOPY := objcopy
  OBJDUMP := $(CROSS)objdump
else ifeq ($(OSX_BUILD),1)
  OBJDUMP := i686-w64-mingw32-objdump
  OBJCOPY := i686-w64-mingw32-objcopy
else # Linux & other builds
  CPP := $(CROSS)cpp -P
  OBJCOPY := $(CROSS)objcopy
  OBJDUMP := $(CROSS)objdump
endif

PYTHON := python3
SDLCONFIG := $(SDLCROSS)sdl2-config

# configure backend flags

BACKEND_CFLAGS := -DRAPI_$(RENDER_API)=1 -DWAPI_$(WINDOW_API)=1 -DAAPI_$(AUDIO_API)=1
BACKEND_CFLAGS += $(foreach capi,$(CONTROLLER_API),-DCAPI_$(capi)=1)
BACKEND_LDFLAGS :=
SDL2_USED := 0

# for now, it's either SDL+GL or DXGI+DirectX, so choose based on WAPI
ifeq ($(WINDOW_API),DXGI)
  DXBITS := `cat $(ENDIAN_BITWIDTH) | tr ' ' '\n' | tail -1`
  ifeq ($(RENDER_API),D3D12)
    BACKEND_CFLAGS += -Iinclude/dxsdk
  endif
  BACKEND_LDFLAGS += -ld3dcompiler -ldxgi -ldxguid
  BACKEND_LDFLAGS += -lsetupapi -ldinput8 -luser32 -lgdi32 -limm32 -lole32 -loleaut32 -lshell32 -lwinmm -lversion -luuid -static
else ifeq ($(WINDOW_API),SDL2)
  ifeq ($(WINDOWS_BUILD),1)
    BACKEND_LDFLAGS += -lglew32 -lglu32 -lopengl32
  else ifneq ($(TARGET_SWITCH),0)
    BACKEND_LDFLAGS += -lglad -lglapi -ldrm_nouveau -lm
  else ifeq ($(OSX_BUILD),1)
    BACKEND_LDFLAGS += -framework OpenGL $(shell pkg-config --libs glew)
  else
    BACKEND_LDFLAGS += -lGL
  endif
  SDL_USED := 2
else ifeq ($(WINDOW_API),GLFW)
  BACKEND_LDFLAGS += -lGL `pkg-config --static --libs glfw3` $(shell pkg-config --libs glew)
endif

ifeq ($(AUDIO_API),SDL2)
  SDL_USED := 2
endif

ifneq (,$(findstring SDL,$(CONTROLLER_API)))
  SDL_USED := 2
endif

# SDL can be used by different systems, so we consolidate all of that shit into this
ifeq ($(SDL_USED),2)
  BACKEND_CFLAGS += -DHAVE_SDL2=1 $(shell $(SDLCONFIG) --cflags)
  ifeq ($(WINDOWS_BUILD),1)
    BACKEND_LDFLAGS += $(shell $(SDLCONFIG) --static-libs) -lsetupapi -luser32 -limm32 -lole32 -loleaut32 -lshell32 -lwinmm -lversion
  else
    BACKEND_LDFLAGS += $(shell $(SDLCONFIG) --libs)
  endif
endif

ifeq ($(WINDOWS_BUILD),1)
  CC_CHECK := $(CC) -fsyntax-only -fsigned-char $(BACKEND_CFLAGS) $(INCLUDE_CFLAGS) -Wall -Wextra -Wno-format-security $(VERSION_CFLAGS) $(GRUCODE_CFLAGS)
  CFLAGS := $(OPT_FLAGS) $(INCLUDE_CFLAGS) $(BACKEND_CFLAGS) $(VERSION_CFLAGS) $(GRUCODE_CFLAGS) -fno-strict-aliasing -fwrapv

else ifeq ($(TARGET_SWITCH),1)
  CC_CHECK := $(CC) $(NXARCH) -fsyntax-only -fsigned-char $(BACKEND_CFLAGS) $(INCLUDE_CFLAGS) -Wall -Wextra -Wno-format-security $(VERSION_CFLAGS) $(GRUCODE_CFLAGS) -D__SWITCH__=1
  CFLAGS := $(NXARCH) $(OPT_FLAGS) $(INCLUDE_CFLAGS) $(BACKEND_CFLAGS) $(VERSION_CFLAGS) $(GRUCODE_CFLAGS) -fno-strict-aliasing -ftls-model=local-exec -fPIC -fwrapv -D__SWITCH__=1

# Linux / Other builds below
else
  CC_CHECK := $(CC) -fsyntax-only -fsigned-char $(BACKEND_CFLAGS) $(PLATFORM_CFLAGS) $(INCLUDE_CFLAGS) -Wall -Wextra -Wno-format-security $(VERSION_CFLAGS) $(GRUCODE_CFLAGS)
  CFLAGS := $(OPT_FLAGS) $(PLATFORM_CFLAGS) $(INCLUDE_CFLAGS) $(BACKEND_CFLAGS) $(VERSION_CFLAGS) $(GRUCODE_CFLAGS) -fno-strict-aliasing -fwrapv

endif

################################
#     Moon64 Custom Flags      #
################################

# Moon64 Enable filesystem library and C++17
CXXFLAGS := -std=c++17
LDFLAGS += -lstdc++fs

CC_CHECK += -DGIT_HASH=\"$(GIT_HASH)\" -DGIT_BRANCH=\"$(GIT_BRANCH)\"
CFLAGS   += -DGIT_HASH=\"$(GIT_HASH)\" -DGIT_BRANCH=\"$(GIT_BRANCH)\"

ifeq ($(WINDOWS_BUILD),1)
  CC_CHECK += -DDISABLE_CURL_SUPPORT
  CFLAGS += -DDISABLE_CURL_SUPPORT
endif

ifeq ($(DEBUG),1)
  CC_CHECK += -DGAME_DEBUG
  CFLAGS += -DGAME_DEBUG
endif

ifeq ($(DEBUG),2)
  CC_CHECK += -DGAME_DEBUG -DSM64_DEBUG
  CFLAGS += -DGAME_DEBUG -DSM64_DEBUG
endif

################################

# Check for enhancement options

# Check for Puppycam option
ifeq ($(BETTERCAMERA),1)
  CC_CHECK += -DBETTERCAMERA
  CFLAGS += -DBETTERCAMERA
endif

# Check for no drawing distance option
ifeq ($(NODRAWINGDISTANCE),1)
  CC_CHECK += -DNODRAWINGDISTANCE
  CFLAGS += -DNODRAWINGDISTANCE
endif

# Check for Discord Rich Presence option
ifeq ($(DISCORDRPC),1)
  CC_CHECK += -DDISCORDRPC
  CFLAGS += -DDISCORDRPC
endif

# Check for texture fix option
ifeq ($(TEXTURE_FIX),1)
  CC_CHECK += -DTEXTURE_FIX
  CFLAGS += -DTEXTURE_FIX
endif

# Check for no bzero/bcopy workaround option
ifeq ($(NO_BZERO_BCOPY),1)
  CC_CHECK += -DNO_BZERO_BCOPY
  CFLAGS += -DNO_BZERO_BCOPY
endif

# Use internal ldiv()/lldiv()
ifeq ($(NO_LDIV),1)
  CC_CHECK += -DNO_LDIV
  CFLAGS += -DNO_LDIV
endif

ifeq ($(WINDOWS_BUILD),1)
  LDFLAGS := $(BITS) -march=$(TARGET_ARCH) -Llib -lpthread $(BACKEND_LDFLAGS) -static
  ifeq ($(CROSS),)
    LDFLAGS += -no-pie
  endif
  ifeq ($(WINDOWS_CONSOLE),1)
    LDFLAGS += -mconsole
  endif

else ifeq ($(OSX_BUILD),1)
  LDFLAGS := -lm $(PLATFORM_LDFLAGS) $(BACKEND_LDFLAGS) -lpthread -lcurl

else ifeq ($(TARGET_SWITCH),1)
   LDFLAGS := -specs=$(LIBNX)/switch.specs $(NXARCH) $(OPT_FLAGS) -no-pie -L$(LIBNX)/lib $(BACKEND_LDFLAGS) -lnx -lm ` $(NXPATH)/curl-config --libs`

else
  LDFLAGS := $(BITS) -march=$(TARGET_ARCH) -lm $(BACKEND_LDFLAGS) -no-pie -lpthread -lcurl
  ifeq ($(DISCORDRPC),1)
    LDFLAGS += -ldl -Wl,-rpath .
  endif

endif # End of LDFLAGS

LDFLAGS += -lstdc++

# Prevent a crash with -sopt
export LANG := C

#################################### Targets ###################################

ADDONS      := addons
ALL_DIRS := $(BUILD_DIR) $(addprefix $(BUILD_DIR)/,$(SRC_DIRS) $(addprefix levels/,$(LEVEL_DIRS)) include) $(ADDONS_PATH)
ADDON_FLAG := $(BUILD_DIR)/.addon
ADDONS_PATH := $(BUILD_DIR)/$(ADDONS)/
MEXT_PARAMETERS := ./baserom.us.z64 ./build/us_pc/addons/moon64 ./assets/fonts ./assets/textures/icons ./assets/textures/logo ./assets/textures/moon ./assets/textures/special ./assets/langs

all: $(EXE)
ifeq ($(TARGET_SWITCH),1)
all: $(EXE).nro
endif

clean:
	$(RM) -r $(BUILD_DIR_BASE)

cleantools:
	$(MAKE) -s -C tools clean

distclean:
	$(RM) -r $(BUILD_DIR_BASE)
	./extract_assets.py --clean

addon:
	./tools/rom/mext $(MEXT_PARAMETERS)

$(BUILD_DIR)/$(RPC_LIBS):
	mkdir -p $(@D)
	cp $(RPC_LIBS) $(@D)

# Make sure build directory exists before compiling anything
DUMMY != mkdir -p $(ALL_DIRS)

all:
	if [ ! -d "$(ADDONS_PATH)saturn" ]; then cp -R $(ADDONS)/saturn $(ADDONS_PATH); fi

$(BUILD_DIR)/levels/scripts.o: $(BUILD_DIR)/include/level_headers.h

$(BUILD_DIR)/include/level_headers.h: levels/level_headers.h.in
	$(CPP) -I . levels/level_headers.h.in | $(PYTHON) tools/output_level_headers.py > $(BUILD_DIR)/include/level_headers.h

$(BUILD_DIR)/%.o: %.cpp
	@$(CXX) -fsyntax-only $(CFLAGS) $(CXXFLAGS) -MMD -MP -MT $@ -MF $(BUILD_DIR)/$*.d $<
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) -o $@ $<

$(BUILD_DIR)/%.o: %.c
	@$(CC_CHECK) $(CC_CHECK_CFLAGS) -MMD -MP -MT $@ -MF $(BUILD_DIR)/$*.d $<
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/%.o: $(BUILD_DIR)/%.c
	@$(CC_CHECK) $(CC_CHECK_CFLAGS) -MMD -MP -MT $@ -MF $(BUILD_DIR)/$*.d $<
	$(CC) -c $(CFLAGS) -o $@ $<

$(ADDON_FLAG):
	./tools/rom/mext $(MEXT_PARAMETERS)
	$(shell touch $(ADDON_FLAG))

$(EXE): $(ADDON_FLAG) $(O_FILES) $(BUILD_DIR)/$(RPC_LIBS)
	$(LD) -L $(BUILD_DIR) -o $@ $(O_FILES) $(LDFLAGS)

ifeq ($(TARGET_SWITCH), 1)

# add `--icon=$(APP_ICON)` to this when we get a suitable icon
%.nro: %.stripped %.nacp
	@elf2nro $< $@ --nacp=$*.nacp --icon=$(APP_ICON)
	@echo built ... $(notdir $@)
	@echo $(APP_ICON)

%.nacp:
	@nacptool --create "$(APP_TITLE)" "$(APP_AUTHOR)" "$(APP_VERSION)" $@ $(NACPFLAGS)
	@echo built ... $(notdir $@)

%.stripped: %
	@$(STRIP) -o $@ $<
	@echo stripped ... $(notdir $<)

endif

.PHONY: addon all clean default
.SECONDARY: addon
.DELETE_ON_ERROR:

# Remove built-in rules, to improve performance
MAKEFLAGS += --no-builtin-rules

-include $(DEP_FILES)

print-% : ; $(info $* is a $(flavor $*) variable set to [$($*)]) @true