#version 330 core

in Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 normal_coord;
    vec3 pixel_coord;
} fs_in;

out vec4 frag_color;

uniform vec4 tint;
uniform sampler2D tex;
uniform vec3 camera_eye;

void main(){
    //TODO: (Req 7) Modify the following line to compute the fragment color
    // by multiplying the tint with the vertex color and with the texture color 
    float ambient = 0.2f;

    vec3 normal = normalize(fs_in.normal_coord);
    vec3 sight_vector = normalize(camera_eye - fs_in.pixel_coord);

    float diffuse = max(dot(sight_vector, normal), 0.0f);

    vec4 texture_color = texture(tex, fs_in.tex_coord);
    frag_color = tint * fs_in.color * texture_color * (diffuse + ambient);
}
