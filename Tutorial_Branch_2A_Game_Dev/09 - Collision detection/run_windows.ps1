
# this requires the file to be compiled to be received as an argument

# if no argument is provided, we echo an error message and exit
if ($args.count -eq 0) {
	echo "This script requires an argument."
	echo "Please provide the file you wish to be compiled as an argument."
	exit 1
}

# We silently move a.exe to .a.exe if a.exe exists
$pathOfExecutable = $PSScriptRoot + "\a.exe"
if( [System.IO.File]::Exists($pathOfExecutable) ) 
{
	#silent rm
	Get-ChildItem -Path .a.exe -Recurse | Remove-Item -force -recurse
	mv a.exe .a.exe
}

# compilation and linking
# -I..\..\Libraries\glad\include\KHR
gcc $args[0] -I..\..\Libraries\glad\include ..\..\Libraries\GLFW\Windows\libglfw3.a ..\..\Libraries\glad\glad.c -lopengl32 -lgdi32

# execute
.\a.exe

