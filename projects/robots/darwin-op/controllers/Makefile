TARGETS=sample

.PHONY: $(TARGETS)

compil clean:
	@for X in $(TARGETS); do \
	  echo $@ing $$X; \
	  make -s -C $$X $@ ; \
	done
