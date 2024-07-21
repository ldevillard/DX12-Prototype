struct CameraPos
{
    float4 Pos;
};

ConstantBuffer<CameraPos> CameraPosCB : register(b2);

struct PixelShaderInput
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
    float4 Normal : NORMAL;
};

float3 ComputeDirectionalLighting(float3 position, float3 color, float3 normal)
{
    const float3 lightDirection = float3(0, -1, 0);
    const float lightIntensity = 1;
    const float3 lightColor = float3(1, 1, 1);
    
    // ambiant lighting
    float3 ambient = lightColor * color * 0.2f;

    // diffuse lighting
    float3 norm = normalize(normal);
    float3 lightDir = normalize(lightDirection);
    float diff = max(dot(norm, lightDir), 0.0);
    float3 diffuse = diff * lightColor * color;
    
    // specular lighting
    float3 viewDir = normalize(CameraPosCB.Pos.xyz - position);
    float3 reflectDir = reflect(lightDir, norm);
    float3 halfwayDir = normalize(-(lightDir + viewDir));
    float spec = pow(max(dot(halfwayDir, reflectDir), 0.0f), 0.8f * 128); // last is shininess (hard coded)
    float3 specular = spec * lightColor * color;
    
    return (ambient + diffuse + specular) * lightIntensity;
}

float4 main(PixelShaderInput IN) : SV_Target
{
    float3 computedLight = ComputeDirectionalLighting(IN.Position.xyz, IN.Color.xyz, -IN.Normal.xyz);
    return IN.Color * float4(computedLight, 1);
}  