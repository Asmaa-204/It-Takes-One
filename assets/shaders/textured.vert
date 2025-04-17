#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec3 normal;

out Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 normal;
    vec3 frag_pos;
} vs_out;

uniform mat4 model;
uniform mat4 PV;
uniform vec3 cameraPos;

void main(){
    mat4 transform = PV * model;

    gl_Position = transform * vec4(position, 1.0);
    
    vs_out.color = color;
    vs_out.tex_coord = tex_coord;
    vs_out.normal = mat3(transpose(inverse(transform))) * normal;
    vs_out.frag_pos = vec3(transform * vec4(position, 1.0));
}
