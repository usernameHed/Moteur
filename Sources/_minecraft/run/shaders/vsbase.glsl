varying vec3 normal;
varying vec3 vertex_to_light_vector;
varying vec3 vertex_to_camera_vector;
varying vec4 color;

uniform float elapsed;
uniform vec3 cameraPos;
uniform mat4 viewMatrix;

mat4 modelMatrix;

void main()
{
	//modelMatrix = invertView * gl_ModelViewMatrix;
	//vec4 worldVertex = modelMatrix * gl_Vertex;
	vec4 viewVertex = gl_ModelViewMatrix * gl_Vertex;
	vertex_to_camera_vector = normalize(-viewVertex.xyz);
	
	// Transforming The Vertex
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;

	// Transforming The Normal To ModelView-Space
	normal = gl_NormalMatrix * gl_Normal; 

	//Direction lumiere
	vertex_to_light_vector = normalize(vec3(viewMatrix * gl_LightSource[0].position));

	//Couleur
	float test = max(0, dot(vertex_to_camera_vector, normal));
	color = vec4(1, 1, 1, 1);
}