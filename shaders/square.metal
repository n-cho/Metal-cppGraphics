#include <metal_stdlib>
using namespace metal;

struct vertexOut {
    float4 pos [[position]];
    float2 textureCoord;
};

vertexOut vertex vertexMain(
        uint vertexId [[vertex_id]],
        device const float4* positions [[buffer(0)]],
        device const float2* textureCoordinates [[buffer(1)]]) {
    vertexOut out;

    out.pos = positions[vertexId];
    out.textureCoord = textureCoordinates[vertexId];
    return out;
}

float4 fragment fragmentMain(vertexOut in [[stage_in]], texture2d<float> colorTexture [[texture(0)]]) {
    constexpr sampler textureSampler (mag_filter::linear, min_filter::linear);

    const float4 colorSample = colorTexture.sample(textureSampler, in.textureCoord);
    return colorSample;
}
