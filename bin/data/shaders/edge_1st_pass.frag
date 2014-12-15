#version 150

flat in vec4 pos;
flat in vec3 norm;

uniform float EdgeThreshold;

uniform vec4 LightPosition;
//uniform vec3 LightIntensity;
vec3 LightIntensity;

const vec3 lum = vec3(0.2126, 0.7152, 0.0722);
out vec4 fragColor;

//-------------------
//Phong stuff
uniform vec3 Kd; //Diffuse Reflectivity
uniform vec3 Ka; //Ambient reflectivity
uniform vec3 Ks;
uniform float Shininess;

uniform vec3 La;
uniform vec3 Ld;
uniform vec3 Ls;
//-------------------

vec3 phongModel(vec4 thePos, vec3 theNorm)
{
    vec3 s = normalize(vec3(LightPosition - thePos));
    vec3 v = normalize(- thePos.xyz);
    vec3 r = reflect(- s, theNorm);
    
    vec3 ambient = La * Ka;
    float sDotN = max(dot(s, theNorm), 0.0);
    vec3 diffuse = Ld * Kd * sDotN;
    
    vec3 spec = vec3(0.0);
    if(sDotN > 0)
    {
        spec = Ls * Ks * pow(max(dot(r, v), 0.0), Shininess);
    }
    
    LightIntensity = ambient + diffuse + spec;
    
    return LightIntensity;
}



float luminance (vec3 color)
{
    return dot(lum, color);
}

subroutine (RenderPassType)
vec4 pass1()
{
    return vec4 (phongModel(pos, norm), 1.0);
}

subroutine (RenderPassType)
vec4 pass2()
{
    ivec2 pix = ivec2(gl_FragCoord.xy);
    float s00 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(-1,1)).rgb);
    float s10 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(-1,0)).rgb);
    float s20 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(-1,-1)).rgb);
    float s01 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(0,1)).rgb);
    float s21 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(0,-1)).rgb);
    float s02 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(1,1)).rgb);
    float s12 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(1,0)).rgb);
    float s22 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(1,-1)).rgb);
    float sx = s00 + 2 * s10 + s20 - (s02 + 2 * s12 + s22);
    float sy = s00 + 2 * s01 + s02 - (s20 + 2 * s21 + s22);
    float g = sx * sx + sy * sy;
    if( g > EdgeThreshold ) return vec4(1.0);
    else return vec4(0.0, 0.0, 0.0, 1.0);
}

void main()
{
    fragColor = RenderPass();
    //fragColor = vec4(phongModel(pos, norm), 1.0);
}