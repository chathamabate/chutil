

include ./vars.mk

# Add Libraries here.
LIBS:=chutil chjson

.PHONY: all clean lib test run_tests uninstall install
.PHONY: uninstall_unity install_unity

all:
	true $(foreach lib,$(LIBS),&& make -C $(PROJ_DIR)/$(lib))

clean:
	true $(foreach lib,$(LIBS),&& make -C $(PROJ_DIR)/$(lib) clean)

lib:
	true $(foreach lib,$(LIBS),&& make -C $(PROJ_DIR)/$(lib) lib)

test:
	true $(foreach lib,$(LIBS),&& make -C $(PROJ_DIR)/$(lib) test)

run_tests:
	true $(foreach lib,$(LIBS),&& make -C $(PROJ_DIR)/$(lib) run_tests)

uninstall:
	true $(foreach lib,$(LIBS),&& make -C $(PROJ_DIR)/$(lib) uninstall)

install:
	true $(foreach lib,$(LIBS),&& make -C $(PROJ_DIR)/$(lib) install_headers)
	true $(foreach lib,$(LIBS),&& make -C $(PROJ_DIR)/$(lib) install_lib)

# Rules for helping to install unity!

UNITY_REPO:=https://github.com/ThrowTheSwitch/Unity.git
UNITY_HASH:=73237c5d224169c7b4d2ec8321f9ac92e8071708

UNITY_DIR:=$(PROJ_DIR)/_temp_unity_dir
UNITY_SRC_DIR:=$(UNITY_DIR)/src

uninstall_unity:
	rm -rf $(INSTALL_DIR)/include/unity
	rm -f $(INSTALL_DIR)/libunity.a

install_unity: uninstall_unity
	# Clone repo and build static library.
	git clone $(UNITY_REPO) $(UNITY_DIR)
	gcc -c $(UNITY_SRC_DIR)/unity.c -I$(UNITY_SRC_DIR) -o $(UNITY_SRC_DIR)/unity.o
	cd $(UNITY_DIR) && git checkout $(UNITY_HASH)
	ar rcs $(UNITY_SRC_DIR)/libunity.a $(UNITY_SRC_DIR)/unity.o
	# Copy headers and lib.
	mkdir -p $(INSTALL_DIR)/include/unity
	cp $(UNITY_SRC_DIR)/*.h $(INSTALL_DIR)/include/unity
	cp $(UNITY_SRC_DIR)/libunity.a $(INSTALL_DIR)
	# Finally, remove the temporary directory.
	rm -rf $(UNITY_DIR)

	



