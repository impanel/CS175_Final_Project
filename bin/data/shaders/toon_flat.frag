#version 150

flat in vec4 pos;
flat in vec3 norm;

//struct LightInfo
//{
    //vec4 position;
    //vec3 intensity;
//};
uniform vec4 LightPosition;
uniform vec3 LightIntensity;

uniform vec3 Kd; //Diffuse Reflectivity
uniform vec3 Ka; //Ambient reflectivity

const int levels = 12;
const float scaleFactor = 1.0 / levels;

out vec4 fragColor;

vec3 toonShade()
{
    vec3 s = normalize(LightPosition.xyz - pos.xyz);
    float cosine = max(0.0, dot(s, norm));
    vec3 diffuse = Kd * floor(cosine * levels) * scaleFactor;
    return LightIntensity * (Ka + diffuse);
}

void main()
{
    fragColor = vec4(toonShade(), 1.0);
}


//in vec3 LightIntensity;
//
//out vec4 fragColor;
//
//void main()
//{
//    fragColor = vec4(LightIntensity, 1.0);
//}
