
# takes an argument for compilation

# Exits and displays error if there is no argument provided
if [ $# -eq 0 ]; then
    echo "This script requires an argument."
    echo "Please provide the C file that you wish to compile."
    echo "Exiting."
    exit 1
fi

# if the a.out file already exists, move it to .a.out. Program should not execute if it does not compile properly.
FILE=a.out
if test -f "$FILE"; then
	mv a.out .a.out
fi

# If it doesn't work for you, try adding any of the libraries below.
#gcc $1 -lGL -lm -lX11 -lpthread -lXrandr -lXi -ldl -lglfw
#gcc $1 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -lfreetype -lGL -lm -ldl ../../Libraries/glad/glad.c ../../Libraries/GLFW/Linux/libglfw.so.3.3 

gcc $1 $(pkg-config --cflags freetype2) -lfreetype -lGL -lm -ldl ../../Libraries/glad/glad.c ../../Libraries/GLFW/Linux/libglfw.so.3.3 

# `freetype-config --cflags --libs`
./a.out

