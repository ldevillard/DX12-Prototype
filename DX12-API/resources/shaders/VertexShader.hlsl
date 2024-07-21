struct Model
{
    matrix ModelMatrix;
};

struct View
{
    matrix ViewMatrix;
};

struct Projection
{
    matrix ProjectionMatrix;
};

ConstantBuffer<Model> ModelCB : register(b0);
ConstantBuffer<View> ViewCB : register(b1);
ConstantBuffer<Projection> ProjectionCB : register(b2);

struct VertexPosColor
{
    float3 Position : POSITION;
    float3 Color : COLOR;
    float3 Normal : NORMAL;
};

struct VertexShaderOutput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float4 Normal : NORMAL;
};

VertexShaderOutput main(VertexPosColor IN)
{
    VertexShaderOutput OUT;

    matrix MVPMatrix = mul(ProjectionCB.ProjectionMatrix, mul(ViewCB.ViewMatrix, ModelCB.ModelMatrix));
    
    OUT.Position = mul(MVPMatrix, float4(IN.Position, 1.0f));
    OUT.Color = float4(IN.Color, 1.0f);
    OUT.Normal = mul(ModelCB.ModelMatrix, float4(IN.Normal, 0.0f));

    return OUT;
}