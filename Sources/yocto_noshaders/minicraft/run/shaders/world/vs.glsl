#version 400

uniform mat4 mvp;
uniform mat4 nmat;

layout(location=0) in vec3 vs_position_in;
layout(location=1) in vec3 vs_normal_in;
layout(location=2) in vec2 vs_uv_in;
layout(location=3) in float vs_type_in;

//Variables en sortie
out vec3 normal;
out vec4 color;
out vec2 uv;
flat out int type;

#define CUBE_HERBE 0.0
#define CUBE_TERRE 1.0
#define CUBE_PIERRE 2.0f
#define CUBE_EAU 3.0
#define CUBE_BRANCHES 4.0f
#define CUBE_TRONC 5.0f
#define CUBE_SABLE 6.0f
#define CUBE_NUAGE 7.0f

const vec4 CubeColors[8]=vec4[8](
	vec4(0.1,0.7,0.2,1.0), //CUBE_HERBE
	vec4(0.2,0.1,0.0,1.0), //CUBE_TERRE
	vec4(0.7,0.7,0.7,1.0), //CUBE_PIERRE
	vec4(0.0,0.0,1.0,0.8), //CUBE_EAU
	vec4(0.3,0.6,0.3,1.0), //CUBE_BRANCHES
	vec4(0.2,0.1,0.0,1.0), //CUBE_TRONC
	vec4(0.7,0.7,0.0,1.0), //CUBE_SABLE
	vec4(1.0,1.0,1.0,1.0)  //CUBE_NUAGE
);

void main()
{
	uv = vs_uv_in;
	type = int(vs_type_in);

	vec4 vecIn = vec4(vs_position_in,1.0);
	gl_Position =  mvp * vecIn;
			
	normal = (nmat * vec4(vs_normal_in,1.0)).xyz; 
	
	color = CubeColors[int(vs_type_in)];
}