
CCFLAGS      := $(CCFLAGS) -DTMP_NO_PROC

#��ǰ·��
CUR_DIR				:= $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)


#�����ͷ�ļ�·��
PATH_INCLUDE :=	-I.			\
		-I$(ACE_ROOT)/	\
		-I$(TOPDIR_EXPORT)/	\
		-I$(TOPDIR)/MonitorManage	\
		



#��Ҫ���ӵĿ�·�������֣���Ҫ���ӵ�ʱ��Ŵ򿪣�����ע�͵�
PATH_LIBARAY	:= \
		-L$(TOPDIR_OUT)/bin_$(ARCH)/	\
		-L$(ACE_ROOT)/lib/	\
#		-L/usr/local/lib/			\

NAME_LIBARAY	:= \
		-lmonitormanage	\
		-lcpf -lACE	\
		-lprobesysmgr	\
		-lflux_control_common	\
		-lACE   -lpthread	-liconv	-lpcap -lwiretap -lwsutil -lwireshark  	\


#######Please set the parameters for your certain project################################################################		

PROJECT_NAME := $(shell $(TOPDIR)/projectname.sh)

#The directory to store the object file
OBJDIR		:=$(OBJDIRBASE)/$(PROJECT_NAME)#End

#The directory to store the dynamic file 
#DYNADIR	:=$(TOPDIR_OUT)/bin/#End

#The exe file name
DYNAFILE	:=$(PROJECT_NAME)#End

EXCSRC	:=# 

#�������Ŀ¼������Ҫ�������г����е���Ŀ¼��������   	$(wildcard sub_dir_name/*.cpp) 
SUBDIR_CPP := $(wildcard *.cpp)  \

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


