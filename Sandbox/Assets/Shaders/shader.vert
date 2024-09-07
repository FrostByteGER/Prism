#version 460
#extension GL_EXT_debug_printf : enable

layout(binding = 0) uniform UniformBufferObject {
    
    mat4 view;
    mat4 projection;
} ubo;

layout(push_constant) uniform PushConstants {
	mat4 model;
    vec3 color;
    vec3 lightPos;
} pcs;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;


layout(location = 0) flat out vec3 fragColor;
layout(location = 1) flat out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragLightPos;

void main() {
    //debugPrintfEXT("fragNormal: %v3f", inNormal);
    gl_Position = ubo.projection * ubo.view * pcs.model * vec4(inPosition, 1.0);
    fragColor = pcs.color;
    fragNormal = inNormal;
    fragTexCoord = inTexCoord;
    fragLightPos = pcs.lightPos;
}