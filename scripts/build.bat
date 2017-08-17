SET build=..\..\build-win

RMDIR /S /Q %build% & :: remove old build directory
MKDIR %build%       & :: create new build directory
CD %build%

cmake ../mrain -G "Visual Studio 15 2017 Win64"          & :: generate project in build dir
cmake --build . --target "ALL_BUILD" --config "Release"
cpack -G WIX -C Release