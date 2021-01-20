# Copyright © 2020, Timothy Trippel <trippel@umich.edu>
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

SHELL := /bin/bash

REPO_PATH := $(shell pwd)

.PHONY: build-infra clean-infra all dev clean

all:
	docker run -it --rm \
		-v $(REPO_PATH)/circuits:/src/bomberman/circuits \
		-v $(REPO_PATH)/bomberman:/src/bomberman/bomberman \
		-v $(REPO_PATH)/tests:/src/bomberman/tests \
		-v $(REPO_PATH)/tgt-dfg:/src/bomberman/tgt-dfg \
		-v $(REPO_PATH)/third_party:/src/bomberman/third_party \
		-t bomberman/sim ./analyze_all.sh

dev:
	docker run -it --rm \
		-v $(REPO_PATH)/circuits:/src/bomberman/circuits \
		-v $(REPO_PATH)/bomberman:/src/bomberman/bomberman \
		-v $(REPO_PATH)/tests:/src/bomberman/tests \
		-v $(REPO_PATH)/tgt-dfg:/src/bomberman/tgt-dfg \
		-v $(REPO_PATH)/third_party:/src/bomberman/third_party \
		-t bomberman/sim /bin/bash

build-infra:
	docker build -t bomberman/sim . 

clean-infra:
	docker rmi -f bomberman/sim && \
	docker ps -a -q | xargs -I {} docker rm {} && \
	docker images -q -f dangling=true | xargs -I {} docker rmi -f {} && \
	docker volume ls -qf dangling=true | xargs -I {} docker volume rm {}

clean:
	$(MAKE) -C circuits/aes clean
	$(MAKE) -C circuits/uart clean
	$(MAKE) -C circuits/or1200 clean
	$(MAKE) -C circuits/picorv32 clean
	$(MAKE) -C tests/bomberman cleanall
	$(MAKE) -C tests/tgt-dfg clean
