.PHONY: compile test windows clean

compile:
	cmake -S . -B build/linux -DCMAKE_BUILD_TYPE=Release
	cmake --build build/linux --parallel

test: compile
	ctest --test-dir build/linux --output-on-failure

windows:
	cmake -S . -B build/windows -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=cmake/mingw-w64.cmake
	cmake --build build/windows --parallel

clean:
	cmake --build build/linux --target clean
