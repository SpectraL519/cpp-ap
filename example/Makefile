DIR_MAKEFILE := ./make
DIR_LOG      := ../log

ifeq (,$(wildcard $(DIR_LOG)/*.sys))
    $(info Syslog file not found...)

    ifndef SYSTEM
        ifeq ($(OS), Windows_NT)
            SYSTEM := windows
        else
            SYSTEM := $(shell uname -s)

            ifeq ($(filter $(SYSTEM),Linux Darwin),$(SYSTEM))
                SYSTEM := unix_like
            else
                SYSTEM := unknown
            endif

        endif
    endif

    $(shell echo $(SYSTEM) > $(DIR_LOG)/$(SYSTEM).sys)
    $(info Syslog file created!)
endif

$(info )
$(info Checking your syslog...)

ifndef SYSTEM
    FILES_SYS := $(wildcard $(DIR_LOG)/*.sys)
    FILES_COUNT := $(words $(FILES_SYS))

    ifneq (1,$(FILES_COUNT))
        $(error More than one syslog file found in $(DIR_LOG). Rejecting target...)
    endif

    SYSTEM := $(patsubst $(DIR_LOG)/%.sys,%,$(FILES_SYS))
endif

ifeq ($(SYSTEM), unknown)
    $(info This program does not support your operating system.)
else
    MAKEFILE := $(DIR_MAKEFILE)/$(SYSTEM).mk

    $(info Including: $(MAKEFILE))
    $(info )
    
    include $(MAKEFILE)
endif
