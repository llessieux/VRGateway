struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VOut ColorVertexShader(float4 position : POSITION, float4 color : COLOR)
{
    VOut output;

    output.position = position;
    output.color = color;

    return output;
}


float4 ColorPixelShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
    return color;
}