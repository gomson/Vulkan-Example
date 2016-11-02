# Vulkan Example

This repository wants to be a base to use Vulkan with C++ Implementation.

It uses vulkan-hpp from Nvidia and I create some objects to have a RAII idiom
and simplify some of their class

All "pure Vulkan Object" are in the Vulkan/{System,Pipeline} dirent
Shader should be compiler through glslangvalidator

## Hello Triangle
It simply shows how to create a simple triangle without any buffer.

It use Secondary Command Buffer to perform the draw.

## Triangle With VBO

It is the same triangle.
However it use a vertex buffer and a "good" memory management with the "DeviceAllocator" class

## Triangle With Uniform Buffer

This part had a little change. I removed the "not copyable" things from a lot of classes.
The PipelineLayoutTriangle manages how to bind the uniform buffer to the pipeline.

## Texture

This part show how to display one texture without mipmapping

## Mipmap

This part show how to build mipmap from one image.
It uses vkCmdBlitImage, barrier transition, there is not a lot of code which differs from Texture, but you need to understand barrier more clearly than in Texture example
The code is within the bufferImageTransferer class.
