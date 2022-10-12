all:
	@echo nothing special

clean:
	rm -rf build
	rm -f test_*.svg
force_clean:
	docker run --rm -v `pwd`:`pwd` -w `pwd` -it alpine/make make clean
.PHONY: clean force_clean

build:
	mkdir -p build && cd build && cmake .. && make
.PHONY: build

DOCKER_TAG_WINDOWS ?= ghcr.io/cubao/build-env-windows-x64:v0.0.1
DOCKER_TAG_LINUX ?= ghcr.io/cubao/build-env-manylinux2014-x64:v0.0.1
DOCKER_TAG_MACOS ?= ghcr.io/cubao/build-env-macos-arm64:v0.0.1

TEST_CMD ?= bash
test_in_win:
	docker run --rm -w `pwd` -v `pwd`:`pwd` -v `pwd`/build/win:`pwd`/build -it $(DOCKER_TAG_WINDOWS) $(TEST_CMD)
test_in_mac:
	docker run --rm -w `pwd` -v `pwd`:`pwd` -v `pwd`/build/mac:`pwd`/build -it $(DOCKER_TAG_MACOS) $(TEST_CMD)
test_in_linux:
	docker run --rm -w `pwd` -v `pwd`:`pwd` -v `pwd`/build/linux:`pwd`/build -it $(DOCKER_TAG_LINUX) $(TEST_CMD)

build_all_in_docker:
	TEST_CMD="make build" make test_in_linux test_in_mac test_in_win
