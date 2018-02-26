varying vec3 normal;
varying vec3 vertex_to_light_vector;
varying vec3 vertex_to_eye_vector;
varying vec4 color;

uniform float ambientLevel;

float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}

float noise(vec3 p){
    vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 * (1.0 / 41.0));
    vec4 o2 = fract(k4 * (1.0 / 41.0));

    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return ((clamp(o4.y * d.y + o4.x * (1.0 - d.y), 0, 0.4) * 2) * 0.2) - 1;
}

void main()
{
	// Scaling The Input Vector To Length 1
	vec3 normalized_normal = normalize(normal * noise(normal));
	vec3 normalized_vertex_to_light_vector = normalize(vertex_to_light_vector);

	// Calculating The Diffuse Term And Clamping It To [0;1]
	float DiffuseTerm = clamp(dot(normal, vertex_to_light_vector), 0.0, 1.0);

	//Specular
	vec3 half = normalize(vertex_to_light_vector + vertex_to_eye_vector);
	float spec = pow(clamp(dot(half, normal), 0, 1), 10);

	// Calculating The Final Color
	gl_FragColor = color * (DiffuseTerm*(1-ambientLevel) + ambientLevel) + spec * color;
	gl_FragColor.a = color.a;
}