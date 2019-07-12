
# ͨ������һ���µĹ���ʱ��ֻ��Ҫ�޸�PATH_INCLUDE�����ݣ������Ķ�����Ҫ�޸�
# �����Ҫ����������ĳ������⣬����Ҫ�޸Ĳ���PATH_LIBARAY��NAME_LIBARAY
# ���ⲿmakefile����ı�����TOPDIR��TOPDIR_OUT

#��ǰ·��
CUR_DIR				:= $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)

#�����ͷ�ļ�·��
PATH_INCLUDE	:= -I.	\
		-I$(ACE_ROOT)/	\
		-I$(TOPDIR_IMPORT)/	\
		-I$(TOPDIR_EXPORT)/	\
		-I$(TOPDIR)/tcpip_mgr/flux_control_common/export	\
		-I$(TOPDIR)/tcpip_mgr/tcpip_mgr_common/export	\
		-I$(TOPDIR)/tcpip_mgr/tcpip_mgr/export/	\
		-I$(CUR_DIR)/export/	\
		-I$(CUR_DIR)/source/	\
		-I$(CUR_DIR)/../config_center/export/	\
		-I$(TOPDIR)/unierm_ctrl_dll/ \
		-I$(TOPDIR)/ProbeSysMgr/export/ \

#��Ҫ���ӵĿ�·�������֣���Ҫ���ӵ�ʱ��Ŵ򿪣�����ע�͵�
#PATH_LIBARAY	:= -L$(TOPDIR_OUT)/bin/linux
#NAME_LIBARAY	:= -l
				


#######Please set the parameters for your certain project################################################################		

PROJECT_NAME := $(shell $(TOPDIR)/projectname.sh)
			

#The directory to store the object file
OBJDIR		:=$(OBJDIRBASE)/$(PROJECT_NAME)#End

#The directory to store the dynamic file 
#DYNADIR	:=$(TOPDIR_OUT)/bin/#End

#The directory to store the static file
#STATICDIR	:=$(TOPDIR_OUT)/bin/#End

#The dynamic file name
DYNAFILE	:= lib$(PROJECT_NAME).so#End

#The static file name
STATICFILE	:= lib$(PROJECT_NAME).a#End



#In this makefile the default settings will compile all the cpp files
#So if there is any cpp file which is not necessary to be complied 
#Please add the correct file name here(ie.It wont be complied)

EXCSRC	:= dllmain.cpp source/PAToDBTask.cpp	source/AuthPortServant.cpp source/DeviceIPMacToDBTask.cpp source/VLanIPMacToDBTask.cpp#



#########################################################################################
SUBDIR_CPP :=	$(wildcard source/*.cpp)  \
		$(wildcard source/app_stat/*.cpp)  \


LIBCPPOBJS	:=  $(patsubst %.cpp,%.o,$(filter-out $(EXCSRC),$(SUBDIR_CPP))) 
LIBCOBJS		:=  $(patsubst %.c,%.o,$(filter-out $(EXCSRC),$(wildcard *.c)))

DYNALIBRARY	:= $(DYNADIR)/$(DYNAFILE)
STATICLIBRARY := $(STATICDIR)/$(STATICFILE)

# ���ڱ��뾲̬��
all: mkobjdir mkinstalldir $(DYNALIBRARY) ###$(STATICLIBRARY)

.PHONY : clean
.PHONY : install
.PHONY : uninstall
.PHONY : mkobjdir
.PHONY : mkinstalldir
.PHONY : movefile



install: mkinstalldir
	cp $(DYNALIBRARY) $(DYNADIR)
	cp $(STATICLIBRARY) $(STATICDIR)

uninstall:
	rm -f $(DYNADIR)/$(DYNAFILE) $(STATICDIR)/$(STATICFILE)

clean:
	rm -f $(DYNALIBRARY) $(STATICLIBRARY) $(LIBCOBJS) $(LIBCPPOBJS)

mrproper distclean: clean
	rm -f .depend


mkobjdir:
	if [ ! -d $(OBJDIR) ]; then \
		mkdir -p $(OBJDIR); fi                                                                                                              

mkinstalldir:
	if [ ! -d $(DYNADIR) ]; then\
		mkdir -p $(DYNADIR); fi
	if [ ! -d $(STATICDIR) ]; then\
		mkdir -p $(STATICDIR); fi
		
#AA := $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)

$(DYNALIBRARY): .depend $(LIBCOBJS) $(LIBCPPOBJS)
	$(CC) $(LDFLAGS) -Wno-deprecated -o $@ $(LIBCOBJS) $(LIBCPPOBJS) $(PATH_LIBARAY) $(NAME_LIBARAY) -Wl,-rpath,'$$ORIGIN'
	
$(STATICLIBRARY): .depend $(LIBCOBJS) $(LIBCPPOBJS)
	$(AR) rs $@ $(LIBCOBJS) $(LIBCPPOBJS)

.depend: makefile $(LIBCOBJS:.o=.c) $(LIBCPPOBJS:.o=.cpp)
	$(CC) -MM $(CCFLAGS) $(OS_CCDYNAFLAGS) $(OS_CCDEBUGFLAGS) $(PATH_INCLUDE) $(LIBCOBJS:.o=.c) $(LIBCPPOBJS:.o=.cpp) > $@

ifeq (.depend, $(wildcard .depend))
include .depend
endif

%.o:%.c
	$(CC)  $(CCFLAGS) $(OS_CCDYNAFLAGS) $(OS_CCDEBUGFLAGS) $(PATH_INCLUDE) -c $< -o $@

%.o:%.cpp
	$(CC)  $(CCFLAGS) $(OS_CCDYNAFLAGS) $(OS_CCDEBUGFLAGS) $(PATH_INCLUDE) -c $< -o $@

movefile:
	mv $(LIBCPPOBJS) $(OBJDIR)