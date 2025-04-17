#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec3 normal;


//###########################################33
// Varyings
//###########################################33

out Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 normal;
    vec3 frag_pos;
} vs_out;


//###########################################33
// Uniforms
//###########################################33

uniform mat4 model;
uniform mat4 PV;

//###########################################33
// Functions
//###########################################33

void main(){
    gl_Position = PV * model * vec4(position, 1.0);

    vs_out.tex_coord = tex_coord;
    vs_out.normal = mat3(transpose(inverse(model))) * normal;
    vs_out.frag_pos = vec3(model * vec4(position, 1.0));
}
