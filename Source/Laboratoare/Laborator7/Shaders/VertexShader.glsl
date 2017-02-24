#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// TODO: get vertex attributes from each location

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform vec3 center;
uniform float amplitude;
uniform float wavelength;
uniform float speed;
uniform float time;
uniform int wave;

out vec3 world_position;
out vec3 world_normal;

void main()
{
	vec3 world_pos = (Model * vec4(v_position,1)).xyz;
	vec3 world_nor = normalize(mat3(Model) * v_normal);


	//calcularea deformarilor
	vec2 D = (center.xz - world_pos.xz)/(length(center.xz - world_pos.xz));

	float W = 2 * 3.14 / wavelength;
	float phi = speed * W;

	//deformarile se aplica doar valurilor nu si robinetului
	if(wave == 1) 
		world_pos.y = amplitude * (sin(dot(D, world_pos.xz - center.xz) * W + time * phi) + 1);
	
	world_position = world_pos;
	world_normal = world_nor;

	gl_Position = Projection * View * vec4(world_position, 1);
}