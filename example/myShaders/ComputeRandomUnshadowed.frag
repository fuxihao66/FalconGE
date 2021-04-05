#version 450


#include "random.glsl"

layout(location = 0) in vec2 outUV;

layout(location = 0) out vec4 fragColor;

layout(set = 0, binding = 0) uniform sampler2D albedoTex;
layout(set = 0, binding = 1) uniform sampler2D metalicTex;
layout(set = 0, binding = 2) uniform sampler2D normalTex;


// constant
// lightPos, light radiance

#define LIGHT_NUM 8
#define PER_LIGHT_SP 20

vec3 evaluateShading(){

}

void main()
{
    vec2  uv    = outUV;
  

    uint seed = tea(gl_LaunchIDNV.y * gl_LaunchSizeNV.x + gl_LaunchIDNV.x, pushC.frame);
    

    fragColor = vec4(0.0, 0.0, 0.0, 1.0);

    vec3 normalValue = texture(normalTex, uv).rgb;
    vec3 albedoValue = texture(albedoTex, uv).rgb;
    vec3 metalicValue = texture(metalicTex, uv).rgb;

    for (uint lightIndex = 0; lightIndex < LIGHT_NUM; lightIndex++){
        // sample

        float lightArea = ;

        vec3 tempColor = vec3(0.0, 0.0, 0.0);
        for (uint spIndex = 0; spIndex < PER_LIGHT_SP; spIndex++){
            float r1 = rnd(seed);
            float r2 = rnd(seed);

            vec3 sampledPoint = lightPos[lightIndex][0] + r1*( lightPos[lightIndex][1] - lightPos[lightIndex][0] ) + r2*( lightPos[lightIndex][2] - lightPos[lightIndex][0] )

            

            tempColor += evaluateShading();
        }

        tempColor /= PER_LIGHT_SP;

        fragColor.rgb += tempColor;

    }

    
}
