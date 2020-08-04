SUBDIRS=PRU_ADS8688_Controller PRU_ADS8688_Interface ARM_User_Space_App

all: $(SUBDIRS)

$(SUBDIRS):
	@$(MAKE) -C $@

clean:
	@for d in $(SUBDIRS); do (cd $$d; $(MAKE) clean ); done

.PHONY: all clean $(SUBDIRS)



