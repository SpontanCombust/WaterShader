#version 330 core

// ============= OUT ==============
out VS_OUT {
    vec3 positionModelspace;
    vec4 positionClipspace;
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
    vs_out.positionModelspace = vec3(uModel * vec4(avPosition, 1.0));
    vs_out.positionClipspace = uProjection * uView * uModel * vec4(avPosition, 1.0);
    vs_out.normal = mat3(transpose(inverse(uModel))) * avNormal;
	vs_out.uv = avUV;

	gl_Position = vs_out.positionClipspace;
}
// ================================