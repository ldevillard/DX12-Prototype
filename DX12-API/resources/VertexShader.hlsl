struct ModelViewProjection
{
    matrix MVP;
};

struct Model
{
    matrix M;
};

ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);
ConstantBuffer<Model> ModelCB : register(b1);

struct VertexPosColor
{
    float3 Position : POSITION;
    float3 Color : COLOR;
    float3 Normal : NORMAL;
};

struct VertexShaderOutput
{
    float4 Color : COLOR;
    float4 Normal : NORMAL;
    float4 Position : SV_Position;
};

VertexShaderOutput main(VertexPosColor IN)
{
    VertexShaderOutput OUT;

    OUT.Position = mul(ModelViewProjectionCB.MVP, float4(IN.Position, 1.0f));
    OUT.Color = float4(IN.Color, 1.0f);
    OUT.Normal = mul(ModelCB.M, float4(IN.Normal, 1.0f));

    return OUT;
}