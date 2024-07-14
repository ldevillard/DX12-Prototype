struct PixelShaderInput
{
    float4 Color : COLOR;
    float4 Normal : NORMAL;
};

float3 ComputeDirectionalLighting(float3 color, float3 normal)
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
    
    return (ambient + diffuse) * lightIntensity;
}

float4 main(PixelShaderInput IN) : SV_Target
{
    float3 computedLight = ComputeDirectionalLighting(IN.Color.xyz, -IN.Normal.xyz);
    return IN.Color * float4(computedLight, 1);
}  