glslangValidator -V final.vert
glslangValidator -V final.frag
mv vert.spv final_vert.spv
mv frag.spv final_frag.spv

glslangValidator -V geometry_static.vert
glslangValidator -V geometry_static.frag
mv vert.spv geometry_static_vert.spv
mv frag.spv geometry_static_frag.spv
