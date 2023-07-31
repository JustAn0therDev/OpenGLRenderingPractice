@ECHO OFF

g++ -o program.exe -g -Wall -IVendor/include -LVendor/lib Main.cpp glad.c Shader.cpp -lopengl32 -lglfw3