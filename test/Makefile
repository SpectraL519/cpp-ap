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

ifeq ($(SYSTEM), unknown)
    $(info This program does not support your operating system.)
else
    MAKEFILE := make/system_$(SYSTEM).mk
    $(info Including: $(MAKEFILE))
    include $(MAKEFILE)
endif
