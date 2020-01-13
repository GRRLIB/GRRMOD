#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro")
endif

ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif

include $(DEVKITPPC)/base_rules
LIBOGC_INC	:=	$(DEVKITPRO)/libogc/include
LIBOGC_LIB	:=	$(DEVKITPRO)/libogc/lib/wii

INSTALL_INC	:=	$(DEVKITPRO)/portlibs/ppc/include
INSTALL_LIB	:=	$(DEVKITPRO)/portlibs/ppc/lib

#---------------------------------------------------------------------------------
# options for building the library, set to yes to include, anything else to exclude
#---------------------------------------------------------------------------------
USE_MOD		:=	yes
USE_MP3		:=	yes

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
# HDR is a list of header files needed for installation
# CFILES is a list of c files for our project
#---------------------------------------------------------------------------------
TARGET		:=	libgrrmod
BUILD		:=	build
SOURCES		:=	
INCLUDES	:=	
HDR			:=	grrmod.h
CFILES		:=	GRRMOD_core.c

#---------------------------------------------------------------------------------
# conditional operation
#---------------------------------------------------------------------------------
CFLAGS		+=	-DHAVE_FCNTL_H -DHAVE_INTTYPES_H -DHAVE_LIMITS_H -DHAVE_MALLOC_H \
				-DHAVE_MEMCMP -DHAVE_SETENV -DHAVE_SNPRINTF -DHAVE_STDINT_H \
				-DHAVE_STDLIB_H -DHAVE_STRCASECMP -DHAVE_STRDUP -DHAVE_STRINGS_H \
				-DHAVE_STRING_H -DHAVE_STRSTR -DHAVE_SYS_STAT_H -DHAVE_SYS_TYPES_H \
				-DHAVE_SYS_WAIT_H -DHAVE_UNISTD_H -DHAVE_ATOLL -DHAVE_LANGINFO_H \
				-DHAVE_LIBM -DHAVE_LOCALE_H -DHAVE_NL_LANGINFO -DHAVE_SIGNAL_H \
				-DHAVE_STDIO_H -DHAVE_SYS_SIGNAL_H -DHAVE_SYS_PARAM_H -DHAVE_STRERROR \
				-DHAVE_SYS_RESOURCE_H
ifeq ($(USE_MOD),yes)
	CFILES		+=	GRRMOD_MOD.c
	SOURCES		+=	mikmod/drivers mikmod/loaders mikmod/mmio mikmod/playercode mikmod/depackers mikmod/posix
	INCLUDES	+=	mikmod/include
	CFLAGS		+=	
endif
ifeq ($(USE_MP3),yes)
	CFILES		+=	GRRMOD_MP3.c
	SOURCES		+=	mpg123
	INCLUDES	+=	mpg123
	CFLAGS		+=	-DOPT_GENERIC -DREAL_IS_FLOAT
endif

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
MACHDEP		:=	-DGEKKO -mcpu=750 -meabi -msdata=eabi -mhard-float -fmodulo-sched -ffunction-sections -fdata-sections
CFLAGS		+=	-O2 $(MACHDEP) $(INCLUDE)
CXXFLAGS	:=	$(CFLAGS)

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:=	-laesnd

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(CURDIR) $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
CFILES		+=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))

export OFILES	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o)

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES), -iquote $(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD) \
					-I$(LIBOGC_INC)

#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib) \
					-L$(LIBOGC_LIB)

export OUTPUT	:=	$(CURDIR)/$(TARGET)
.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTPUT).a

#---------------------------------------------------------------------------------
install :
	mkdir -p  $(INSTALL_LIB)  $(INSTALL_INC)
	cp -f $(TARGET).a $(INSTALL_LIB)/
	cp -f $(HDR) $(INSTALL_INC)/


#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).a: $(OFILES)


-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
