DIR_MAKEFILE := ./make
DIR_LOG      := ../log
FILE_SYS     := system.log

ifeq (,$(wildcard $(DIR_LOG)/$(FILE_SYS)))
    $(info There is no log including your operating system...)
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

    $(shell echo $(SYSTEM) > $(DIR_LOG)/$(FILE_SYS))
    $(info Log including your operating system has been created!)
else
    $(info System.log found!)
endif

$(info )

$(info Checking your operating system...)
SYSTEM := $(shell cat $(DIR_LOG)/$(FILE_SYS))

ifeq ($(SYSTEM), unknown)
    $(info This program does not support your operating system.)
else
    MAKEFILE := $(DIR_MAKEFILE)/$(SYSTEM).mk
    $(info Including: $(MAKEFILE))
    $(info )
    include $(MAKEFILE)
endif

