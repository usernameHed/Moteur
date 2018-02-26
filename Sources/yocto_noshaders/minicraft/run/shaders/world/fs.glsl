#version 400

//Variables en entree
in vec3 normal;
in vec4 color;
in vec2 uv;
flat in int type;

out vec4 color_out;

#define CUBE_HERBE 0.0
#define CUBE_TERRE 1.0
#define CUBE_PIERRE 2.0f
#define CUBE_EAU 3.0
#define CUBE_BRANCHES 4.0f
#define CUBE_TRONC 5.0f
#define CUBE_SABLE 6.0f
#define CUBE_NUAGE 7.0f

void main()
{
	vec3 p_color = color.rgb * (max(0,normal.z+normal.y/2)+0.2f);
	color_out = vec4(p_color.rgb,color.a);
}