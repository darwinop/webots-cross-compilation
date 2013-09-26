TARGETS=soccer symmetry visual_tracking walk motion_player

.PHONY: $(TARGETS)

release debug profile clean:
	@+for X in $(TARGETS); do \
	  echo make $@ $$X; \
	  make -s -C $$X $@ ; \
	done
