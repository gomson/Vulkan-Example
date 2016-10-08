# Vulkan Example

This repository wants to be a base to use Vulkan with C++ Implementation.
It uses vulkan-hpp from Nvidia and I create some objects to have a RAII idiom
and simplify some of their class
All "pure Vulkan Object" are in the Vulkan/{System,Pipeline} dirent
Shader should be compiler through glslangvalidator

## Hello Triangle
It simply shows how to create a simple triangle without any buffer.
It use Secondary Command Buffer to perform the draw.
