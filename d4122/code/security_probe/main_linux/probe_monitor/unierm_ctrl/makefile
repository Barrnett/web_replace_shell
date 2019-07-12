

	
#当前路径
CUR_DIR				:= $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)


#输入的头文件路径
PATH_INCLUDE :=	-I.			\
		-I$(ACE_ROOT)/	\
		-I$(TOPDIR_EXPORT)/	\
		-I$(TOPDIR)/unierm_ctrl_dll	\
		-I$(TOPDIR)/MonitorManage	\
	



#需要链接的库路径和名字，需要链接的时候才打开，否则注释掉
PATH_LIBARAY	:= \
		-L$(TOPDIR_OUT)/bin_$(ARCH)/	\
		-L$(ACE_ROOT)/lib/	\

NAME_LIBARAY	:= \
		-lEtherIO	\
		-lprobesysmgr	\
		-lmonitormanage	\
		-lflux_control_common	-ltcpip_mgr_common\
		-lunierm_ctrl_dll		\
		-lcpf	-lpacketio	\
		-lACE   -lpthread	-liconv	-lpcap -lflux_control -lflux_control_base 	\
		## -lconfig_center -lcpf2 -lefilterio

ifeq ($(ARCH),64)
        NAME_LIBARAY := $(NAME_LIBARAY) \
                -lpfring \

endif


#######Please set the parameters for your certain project################################################################		

PROJECT_NAME := $(shell $(TOPDIR)/projectname.sh)

#The directory to store the object file
OBJDIR		:=$(OBJDIRBASE)/$(PROJECT_NAME)#End

#The directory to store the dynamic file 
#DYNADIR	:=$(TOPDIR_OUT)/bin/#End

#The exe file name
DYNAFILE	:=$(PROJECT_NAME)#End

EXCSRC	:=# 

#如果有子目录，则需要在这里列出所有的子目录，类似于   	$(wildcard sub_dir_name/*.cpp) 
SUBDIR_CPP := $(wildcard *.cpp)  

LIBCPPOBJS    :=  $(patsubst %.cpp,%.o,$(filter-out $(EXCSRC),$(SUBDIR_CPP)))

DYNALIBRARY	:= $(DYNADIR)/$(DYNAFILE)

all: mkobjdir mkinstalldir $(DYNALIBRARY)

.PHONY : clean
.PHONY : install
.PHONY : uninstall
.PHONY : mkobjdir
.PHONY : mkinstalldir
.PHONY : movefile

install: mkinstalldir
	cp $(DYNALIBRARY) $(DYNADIR)

uninstall:
	rm -f $(DYNADIR)/$(DYNAFILE)

clean:
	rm -f $(DYNALIBRARY) $(LIBCPPOBJS)

mrproper distclean: clean
	rm -f .depend


mkobjdir:
	if [ ! -d $(OBJDIR) ]; then \
		mkdir -p $(OBJDIR); fi 

mkinstalldir:
	if [ ! -d $(DYNADIR) ]; then\
		mkdir -p $(DYNADIR); fi

$(DYNALIBRARY): .depend $(LIBCPPOBJS)
	$(CC) -Wno-deprecated -o $@ $(LIBCPPOBJS) $(PATH_LIBARAY) $(NAME_LIBARAY) -Wl,-rpath,'$$ORIGIN'

.depend: makefile $(LIBCPPOBJS:.o=.cpp)
	$(CC) -MM $(CCFLAGS) $(OS_CCDEBUGFLAGS) $(PATH_INCLUDE) $(LIBCPPOBJS:.o=.cpp) > $@

ifeq (.depend, $(wildcard .depend))
include .depend
endif

%.o:%.cpp
	$(CC) $(CCFLAGS) $(OS_CCDEBUGFLAGS) $(PATH_INCLUDE) -c $< -o $@ 

movefile:
	mv $(LIBCPPOBJS) $(OBJDIR)



