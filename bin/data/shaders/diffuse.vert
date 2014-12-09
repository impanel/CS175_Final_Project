#version 150

uniform mat4 modelViewProjectionMatrix; // Projection * ModelView (provided in OF)
in vec4 position; //provided in OF
in vec3 normal; //provided in OF
in vec2 texcoord;

uniform vec4 uColor;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;

out     vec3 LightIntensity;
uniform vec4 LightPosition;// Light position in eye coords.
uniform vec3 Kd;           // Diffuse reflectivity
uniform vec3 Ld;           // Light source intensity
uniform float dispScale;        // scalar for displacement map

uniform sampler2DRect dispTex;



void main() {
    // convert normal and position to eye coordinates
    vec3 tnorm = normalize(NormalMatrix * normal);
    vec4 eyeCoords = ModelViewMatrix * position;
    vec3 s = normalize(vec3(LightPosition - eyeCoords));
    
    float displacementZ = texture(dispTex, texcoord).r;
    vec4 vPos = position;
    vPos.z = (vPos.z + 1.0) * displacementZ * dispScale;
    
    vec4 modifiedHeight = modelViewProjectionMatrix * vPos;
    
    //The diffuse shading equation
    LightIntensity = Ld * Kd * max(dot(s, tnorm), 0.0);
    
    //gl_Position = modelViewProjectionMatrix * position;
    gl_Position = modifiedHeight;
}