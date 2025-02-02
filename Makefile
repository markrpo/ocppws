USER_ID := $(shell id -u)
SUDO := $(if $(filter 0,$(USER_ID)),,sudo)

bootstrap_lib:
	@$(SUDO) apt-get update
	@$(SUDO) apt-get install -y libwebsockets-dev
	@$(SUDO) apt-get install -y cmake g++
	@$(SUDO) apt-get install -y python3-pip
	# @$(SUDO) apt-get install -y yaml-cpp 

.PHONY: bootstrap_lib

