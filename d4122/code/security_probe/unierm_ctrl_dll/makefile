
# 通常编译一个新的工程时，只需要修改PATH_INCLUDE的内容，其他的都不需要修改
# 如果需要在这里链接某个输入库，则需要修改并打开PATH_LIBARAY和NAME_LIBARAY
# 在外部makefile定义的变量有TOPDIR、TOPDIR_OUT

#当前路径
CUR_DIR				:= $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)

#输入的头文件路径
PATH_INCLUDE	:=	-I.\
		-I$(ACE_ROOT)/	\
		-I$(TOPDIR_EXPORT)/	\
		-I$(CUR_DIR)/../MonitorManage	\
		-I$(CUR_DIR)/../ProbeSysMgr/export	\
		-I$(TOPDIR)/../comm_base_module/packetio_wrappers/packetio/source/etherio \


#需要链接的库路径和名字，需要链接的时候才打开，否则注释掉
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

EXCSRC	:= dllmain.cpp	#



#########################################################################################
SUBDIR_CPP :=	\
				$(wildcard *.cpp)  \

SUBDIR_C   :=	$(wildcard source/*.c)  \


LIBCPPOBJS	:=  $(patsubst %.cpp,%.o,$(filter-out $(EXCSRC),$(SUBDIR_CPP))) 
LIBCOBJS	:=  $(patsubst %.c,%.o,$(filter-out $(EXCSRC),$(SUBDIR_C)))

DYNALIBRARY	:= $(DYNADIR)/$(DYNAFILE)
STATICLIBRARY := $(STATICDIR)/$(STATICFILE)

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
	gcc  $(CCFLAGS) $(OS_CCDYNAFLAGS) $(OS_CCDEBUGFLAGS) $(PATH_INCLUDE) -c $< -o $@
#	$(CC)  $(CCFLAGS) $(OS_CCDYNAFLAGS) $(OS_CCDEBUGFLAGS) $(PATH_INCLUDE) -c $< -o $@

%.o:%.cpp
	$(CC)  $(CCFLAGS) $(OS_CCDYNAFLAGS) $(OS_CCDEBUGFLAGS) $(PATH_INCLUDE) -c $< -o $@

movefile:
	mv $(LIBCPPOBJS) $(OBJDIR)
