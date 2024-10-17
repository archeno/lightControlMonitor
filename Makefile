# cmake config

config:
	cmake -Bbuild -G "Ninja" -DCMAKE_BUILD_TYPE=Debug
	
build:config
	cmake --build build 

download:build
	openocd -f  ./03cmsis-dap.cfg -c  "program  build/rtthread.bin 0x08000000 verify reset  exit"
clean:
	rm -rf build
