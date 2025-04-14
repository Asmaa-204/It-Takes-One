#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 3) in vec3 normal_coord;

out Varyings {
    vec4 color;
    vec3 normal_coord;
    vec3 pixel_coord;
} vs_out;

uniform mat4 model_transform;
uniform mat4 pv_transform; // this is projection view transform

void main(){
    //TODO: (Req 7) Change the next line to apply the transformation matrix
    vs_out.pixel_coord = vec3(model_transform * vec4(position, 1.0));
    gl_Position = pv_transform * vec4(vs_out.pixel_coord, 1.0);

    vs_out.color = color;
    vs_out.normal_coord = normal_coord;
}
