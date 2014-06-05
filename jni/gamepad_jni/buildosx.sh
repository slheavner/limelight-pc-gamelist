rm libgamepad_jni.dylib
gcc -shared -o libgamepad_jni.dylib -I /Library/Java/JavaVirtualMachines/jdk1.8.0.jdk/Contents/Home/include/ -I /Library/Java/JavaVirtualMachines/jdk1.8.0.jdk/Contents/Home/include/darwin -fPIC *.c -L./osx -lstem_gamepad -lpthread -framework IOKit -framework CoreFoundation

