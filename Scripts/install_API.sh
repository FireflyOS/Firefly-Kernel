mkdir -vp -- ./build/temp/arch/x86_64/api ./build/temp/include/x86_64/api ./build/temp/include/x86_64/drivers
cp ./include/x86_64/api/* ./build/temp/include/x86_64/api/
cp ./arch/x86_64/kernel/api/* ./build/temp/arch/x86_64/api/

cd build/temp

cd include
git clone https://github.com/FireflyOS/Firefly-STL stl && cd stl && git reset --hard edc3b8ba2
cd ../..

clang++ -c ./arch/x86_64/api/api.cpp \
    -I./include/		 		\
	-I./include/stl/			\
	-target x86_64-unknown-elf	\
	-m64						\
	-mcmodel=kernel				\
	-std=c++20   				\
	-Werror						\
	-Wall						\
	-Wextra 					\
	-pedantic 					\
	-g 							\
	-nostdlib 					\
	-fno-builtin 				\
	-fno-PIC 					\
	-fpie						\
	-mno-red-zone 				\
	-fno-stack-check 			\
	-fno-stack-protector 		\
	-fno-omit-frame-pointer 	\
	-ffreestanding 				\
	-fno-exceptions 			\
	-fno-rtti 					\
	-fms-extensions				\
	-Wno-zero-length-array 		\
	-mno-80387		            \
    -mno-mmx					\
    -mno-3dnow					\
	-mno-sse                    \
    -mno-sse2	                \
	-Wno-c99-extensions			\
	-Wno-gnu					\
	-fno-threadsafe-statics     \
    -o api.o
llvm-ar \
    rc api.a api.o
stat api.a

zip -rv Firefly_API.zip ./
mv Firefly_API.zip ../

cd ../
rm -rfv ./temp
zip -rv Firefly_Artifacts.zip ./
stat Firefly_Artifacts.zip

rm -rfv Firefly_API.zip Firefly_x86_64.iso

cd ..

#Upload