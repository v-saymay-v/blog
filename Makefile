IGNORE_DIRS=CVS libblog

dumy:
	@(cd libblog; pwd; $(MAKE));
	@for dir in *; do \
		if [ ! -d "$$dir" ]; then continue; fi; \
		for ignore in $(IGNORE_DIRS); do \
			if [ "$$dir" = "$$ignore" ]; then continue 2; fi \
		done; \
		if [ -f "$$dir/Makefile" ]; then (cd $$dir; pwd; $(MAKE)); fi \
	done;

all:
	@(cd libblog; pwd; $(MAKE) $@);
	@for dir in *; do \
		if [ ! -d "$$dir" ]; then continue; fi; \
		for ignore in $(IGNORE_DIRS); do \
			if [ "$$dir" = "$$ignore" ]; then continue 2; fi \
		done; \
		if [ -f "$$dir/Makefile" ]; then (cd $$dir; pwd; $(MAKE) $@); fi \
	done;

clean:
	@(cd libblog; pwd; $(MAKE) $@);
	@for dir in *; do \
		if [ ! -d "$$dir" ]; then continue; fi; \
		for ignore in $(IGNORE_DIRS); do \
			if [ "$$dir" = "$$ignore" ]; then continue 2; fi \
		done; \
		if [ -f "$$dir/Makefile" ]; then (cd $$dir; pwd; $(MAKE) $@); fi \
	done;

instdbb:
	@(cd libblog; pwd; $(MAKE) $@);
	@for dir in *; do \
		if [ ! -d "$$dir" ]; then continue; fi; \
		for ignore in $(IGNORE_DIRS); do \
			if [ "$$dir" = "$$ignore" ]; then continue 2; fi \
		done; \
		if [ -f "$$dir/Makefile" ]; then (cd $$dir; pwd; $(MAKE) $@); fi \
	done;

instcart:
	@(cd libblog; pwd; $(MAKE) $@);
	@for dir in *; do \
		if [ ! -d "$$dir" ]; then continue; fi; \
		for ignore in $(IGNORE_DIRS); do \
			if [ "$$dir" = "$$ignore" ]; then continue 2; fi \
		done; \
		if [ -f "$$dir/Makefile" ]; then (cd $$dir; pwd; $(MAKE) $@); fi \
	done;

instrsv:
	@(cd libblog; pwd; $(MAKE) $@);
	@for dir in *; do \
		if [ ! -d "$$dir" ]; then continue; fi; \
		for ignore in $(IGNORE_DIRS); do \
			if [ "$$dir" = "$$ignore" ]; then continue 2; fi \
		done; \
		if [ -f "$$dir/Makefile" ]; then (cd $$dir; pwd; $(MAKE) $@); fi \
	done;

insthb:
	@(cd libblog; pwd; $(MAKE) $@);
	@for dir in *; do \
		if [ ! -d "$$dir" ]; then continue; fi; \
		for ignore in $(IGNORE_DIRS); do \
			if [ "$$dir" = "$$ignore" ]; then continue 2; fi \
		done; \
		if [ -f "$$dir/Makefile" ]; then (cd $$dir; pwd; $(MAKE) $@); fi \
	done;

hbsync:
	@(cd libblog; pwd; $(MAKE) $@);
	@for dir in *; do \
		if [ ! -d "$$dir" ]; then continue; fi; \
		for ignore in $(IGNORE_DIRS); do \
			if [ "$$dir" = "$$ignore" ]; then continue 2; fi \
		done; \
		if [ -f "$$dir/Makefile" ]; then (cd $$dir; pwd; $(MAKE) $@); fi \
	done;

cartsync:
	@(cd libblog; pwd; $(MAKE) $@);
	@for dir in *; do \
		if [ ! -d "$$dir" ]; then continue; fi; \
		for ignore in $(IGNORE_DIRS); do \
			if [ "$$dir" = "$$ignore" ]; then continue 2; fi \
		done; \
		if [ -f "$$dir/Makefile" ]; then (cd $$dir; pwd; $(MAKE) $@); fi \
	done;

rsvsync:
	@(cd libblog; pwd; $(MAKE) $@);
	@for dir in *; do \
		if [ ! -d "$$dir" ]; then continue; fi; \
		for ignore in $(IGNORE_DIRS); do \
			if [ "$$dir" = "$$ignore" ]; then continue 2; fi \
		done; \
		if [ -f "$$dir/Makefile" ]; then (cd $$dir; pwd; $(MAKE) $@); fi \
	done;

dbbsync:
	@(cd libblog; pwd; $(MAKE) $@);
	@for dir in *; do \
		if [ ! -d "$$dir" ]; then continue; fi; \
		for ignore in $(IGNORE_DIRS); do \
			if [ "$$dir" = "$$ignore" ]; then continue 2; fi \
		done; \
		if [ -f "$$dir/Makefile" ]; then (cd $$dir; pwd; $(MAKE) $@); fi \
	done;

depend:
	@(cd libblog; pwd; $(MAKE) $@);
	@for dir in *; do \
		if [ ! -d "$$dir" ]; then continue; fi; \
		for ignore in $(IGNORE_DIRS); do \
			if [ "$$dir" = "$$ignore" ]; then continue 2; fi \
		done; \
		if [ -f "$$dir/Makefile" ]; then (cd $$dir; pwd; $(MAKE) $@); fi \
	done;

