TARGET = hellotexture
TYPE = ps-exe

SRCS = \
third_party/nugget/common/crt0/crt0.s \
src/main.cpp \
src/joypad.cpp \
src/globals.cpp \
src/display.cpp \
src/camera.cpp \
src/util.cpp \
src/object.cpp

CPPFLAGS += -Ithird_party/psyq-iwyu/include -Wno-write-strings
LDFLAGS += -Lthird_party/psyq/lib
LDFLAGS += -Wl,--start-group
LDFLAGS += -lapi
LDFLAGS += -lc
LDFLAGS += -lc2
LDFLAGS += -lcard
LDFLAGS += -lcd
LDFLAGS += -lcomb
LDFLAGS += -lds
LDFLAGS += -letc
LDFLAGS += -lgpu
LDFLAGS += -lgs
LDFLAGS += -lgte
LDFLAGS += -lgun
LDFLAGS += -lhmd
LDFLAGS += -lmath
LDFLAGS += -lmcrd
LDFLAGS += -lmcx
LDFLAGS += -lpad
LDFLAGS += -lpress
LDFLAGS += -lsio
LDFLAGS += -lsnd
LDFLAGS += -lspu
LDFLAGS += -ltap
LDFLAGS += -Wl,--end-group

include third_party/nugget/common.mk
