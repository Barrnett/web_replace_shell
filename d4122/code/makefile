UNIERM_DIRS = 	\
	comm_base_module	\
	security_probe	\


.PHONY: $(UNIERM_DIRS)

all: $(UNIERM_DIRS)

$(UNIERM_DIRS):
	find -name "*.sh" -print | xargs chmod +x
#	$(MAKE) -C $@ all
	$(MAKE) -j2 -C $@ all


clean:
	for dir in $(UNIERM_DIRS) ; do $(MAKE) -C $$dir clean ; done

