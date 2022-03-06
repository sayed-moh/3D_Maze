#version 330 core

in Varyings {
    vec4 color;
    vec2 tex_coord;
} fs_in;

out vec4 frag_color;

uniform vec4 tint;
uniform sampler2D tex;
uniform float alpha_threshold;


void main(){
    //TODO: Modify the following line to compute the fragment color
    // by multiplying the tint with the vertex color and with the texture color 
    vec4 color = tint * fs_in.color * texture(tex,fs_in.tex_coord);
    if(color.a < alpha_threshold) discard;
    frag_color = color;
}