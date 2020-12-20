SHELL := /bin/bash

REPO_PATH := /Users/ttrippel/repos/bomberman

.PHONY: build-infra clean-infra run clean

build-infra:
	docker build -t bomberman/sim . 

clean-infra:
	docker rmi -f bomberman/sim && \
	docker ps -a -q | xargs -I {} docker rm {} && \
	docker images -q -f dangling=true | xargs -I {} docker rmi -f {} && \
	docker volume ls -qf dangling=true | xargs -I {} docker volume rm {}

run:
	docker run -it --rm \
		-v $(REPO_PATH)/circuits:/src/bomberman/circuits \
		-v $(REPO_PATH)/scripts:/src/bomberman/scripts \
		-v $(REPO_PATH)/tests:/src/bomberman/tests \
		-v $(REPO_PATH)/tgt-ttb:/src/bomberman/tgt-ttb \
		-v $(REPO_PATH)/third_party:/src/bomberman/third_party \
		-t bomberman/sim /bin/bash

clean:
	$(MAKE) -C circuits/aes clean
	$(MAKE) -C circuits/uart clean
	$(MAKE) -C circuits/or1200 clean
	$(MAKE) -C circuits/picorv32 clean


#TODO(ttrippel): create a master clean target
