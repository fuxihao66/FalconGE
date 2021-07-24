#version 450
layout(location = 0) in vec2 outUV;
layout(location = 1) in vec3 outWorldPos;
layout(location = 0) out vec4 fragColor[];

layout(set = 0, binding = 0) uniform sampler2D albedoTex;
layout(set = 0, binding = 1) uniform sampler2D metalicTex;
layout(set = 0, binding = 2) uniform sampler2D normalTex;


void main()
{
  vec2  uv    = outUV;
  float gamma = 1. / 2.2;
  fragColor[0]   = vec4(outWorldPos, 1.0f);
  fragColor[1]   = pow(texture(albedoTex, uv).rgba, vec4(gamma));
  fragColor[2]   = pow(texture(metalicTex, uv).rgba, vec4(gamma));
  fragColor[3]   = pow(texture(normalTex, uv).rgba, vec4(gamma));
}
