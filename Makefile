all: build

configure:
	cd host-program && cmake -B build -S .
	cd pi-program && cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=$$PICO_SDK_PATH/cmake/preload/toolchains/pico_arm_gcc.cmake

build: configure
	cd host-program && cmake --build build -j8
	cd pi-program && cmake --build build -j8