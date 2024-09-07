#version 460
#extension GL_EXT_debug_printf : enable
layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) flat in vec3 fragColor;
layout(location = 1) flat in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 lightPos;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 lightDirection = normalize(lightPos);

    float brightness = max(dot(abs(normalize(fragNormal)), abs(lightDirection)), 0.0);

    // Clamp the brightness to a minimum so it 's not completely black
    brightness = max(brightness, 0.2);
    //debugPrintfEXT("fragNormal: %v3f, lightDirection: %v3f", fragNormal, lightDirection);
   
    // Set the final color of the fragment
    outColor = vec4(fragColor * brightness, 1.0);
}