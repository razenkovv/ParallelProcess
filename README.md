Building project in VS Code on Windows:

Run VS Code from Developer Command Prompt for VS ****

1) mkdir build
2) Terminal -> Run Task -> CMake Debug / CMake Release
3) ctrl + shift + B (or Terminal -> Run Build Task)
4) сd .\build\
5) .\run.exe

This is simple study project. Main task is to count amount of pixels with prevalence of red/blue/green color in bmp image
using multiprocessing. 
For example, if we have 2 processes, we have to divide image that way: 1 2 1 2. But this is not very necessary now, because error is not 
connected with it. 

The main thing is going on in the win_process_color_prevalence function in methods.cpp. I write some comments there and in child.cpp. Hope someone can help...