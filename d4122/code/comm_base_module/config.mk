
OS := OS_LINUX
#OS := OS_SUN
#OS := OS_HP
#OS := OS_WINDOWS

#
# OS_LINUX: for linux operating system
#
ifeq ($(OS), OS_LINUX)
OS_CC           := g++
OS_CCFLAGS      := -w  -Wno-deprecated -DOS_LINUX -DFLEX_IN_USE -fpermissive
# as5.0中不能使用 -Wstrict-prototypes 参数
OS_CFLAGS      := -Wall -Wno-deprecated -DOS_LINUX -DFLEX_IN_USE -fpermissive
OS_CCDYNAFLAGS  := -fPIC
OS_CCDEBUGFLAGS := -g
OS_CPP          := $(OS_CC) -E
OS_LD           := g++ 
#OS_LDDYNAFLAGS  := -shared  -nostartfiles
OS_LDDYNAFLAGS  := -shared  -fPIC
OS_AR           := ar
OS_RM           := rm -f
OS_CP	        := cp
OS_LEX          := flex
OS_LEXFLAGS     :=
OS_YACC         := bison
OS_YACCFLAGS    := -b y -d -v
OS_LATEX        := latex
OS_DVI2PS       := dvips
OS_DVI2PDF      := dvipdfm
OS_METAPOST     := mpost
endif

#
# OS_SUN: for sun operating system, Solaris/SunOs
#
ifeq ($(OS), OS_SUN)
OS_CC          = CC
OS_LD          = ld
OS_AR          = ar
OS_RM          = rm -f
OS_LEX         = lex
OS_LEXFLAGS    =
OS_YACC        = yacc
OS_YACCFLAGS   = -d
OS_CCFLAGS     = -w -DOS_SUN
OS_LDDYNAFLAGS = -dy -G
#OS_LLIB        = -lsocket -lxnet -lnsl
OS_LATEX       = latex
OS_DVI2PS      = dvips
OS_DVI2PDF     = dvipdfm
OS_METAPOST    = mpost
endif

#
# OS_HP: for hp operating system, HP-UX
#
ifeq ($(OS), OS_HP)
OS_CC          = CC
OS_LD          = ld
OS_AR          = ar
OS_RM          = rm -f
OS_LEX         = lex
OS_LEXFLAGS    =
OS_YACC        = yacc
OS_YACCFLAGS   = -d
OS_CCFLAGS     = -g +w +a1 -Wl,+nocompatwarnings -DOS_HP
OS_LDDYNAFLAGS =
#OS_LLIB        =
OS_LATEX       = latex
OS_DVI2PS      = dvips
OS_DVI2PDF     = dvipdfm
OS_METAPOST    = mpost
endif

#
# OS_WINDOWS: for Windows operating system
#
ifeq ($(OS), OS_WINDOWS)
OS_CC          = g++
OS_LD          = ld
OS_AR          = ar
OS_RM          = del
OS_CP	       = copy
OS_LEX         = flex
OS_LEXFLAGS    =
OS_YACC        = bison
OS_YACCFLAGS   = -b y -d -v
OS_CCFLAGS     = -fPIC -g -Wall -DOS_WINDOWS -DFLEX_IN_USE
OS_LDDYNAFLAGS = -shared
#OS_LLIB        = -lpthread
OS_LATEX       = latex
OS_DVI2PS      = dvips
OS_DVI2PDF     = dvipdfmx
OS_METAPOST    = mpost
endif
