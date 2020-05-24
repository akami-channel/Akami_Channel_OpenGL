
# takes an argument for compilation

# Exits and displays error if there is no argument provided
if [ $# -eq 0 ]; then
    echo "This script requires an argument."
    echo "Please provide the C file that you wish to compile."
    echo "Exiting."
    exit 1
fi

FILE=a.out
if test -f "$FILE"; then
	mv a.out .a.out
fi

clang -Wall -framework OpenGL $1 ../../Libraries/GLFW/MacOS/libglfw.3.3.dylib

./a.out

