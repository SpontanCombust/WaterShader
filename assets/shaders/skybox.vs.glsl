#version 330 core

// ============= OUT ==============
out VS_OUT {
    vec3 texCoords;
} vs_out;
// ================================


// ============= IN ===============
layout(location = 0) in vec3 avPosition;

uniform mat4 uView;
uniform mat4 uProjection;
// ================================


// ============= CODE =============
void main()
{
    vec4 pos = uProjection * uView * vec4(avPosition, 1.0);
    gl_Position = pos.xyww;
    vs_out.texCoords = normalize(avPosition);
}
// ================================