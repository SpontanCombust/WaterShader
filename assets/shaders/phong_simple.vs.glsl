#version 330 core

// ============= OUT ==============
out VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;
} vs_out;
// ================================


// ============= IN ===============
layout(location = 0) in vec3 avPosition;
layout(location = 1) in vec3 avNormal;
layout(location = 2) in vec2 avUV;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
// ================================


// ============= CODE =============
void main()
{
    vs_out.position = vec3(uModel * vec4(avPosition, 1.0));
	vs_out.normal = avNormal;
	vs_out.uv = avUV;

	gl_Position = uProjection * uView * uModel * vec4(avPosition, 1.0);   
}
// ================================