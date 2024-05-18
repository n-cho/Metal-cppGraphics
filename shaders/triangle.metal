#include <metal_stdlib>
using namespace metal;

struct vertex_payload {
    float4 position [[position]];
    half3 color;
};

vertex_payload vertex vertexMain(
        uint vertexId [[vertex_id]],
        device const float3* positions [[buffer(0)]],
        device const float3* colors [[buffer(1)]]) {
    vertex_payload payload;
    payload.position = float4(positions[vertexId], 1.0);
    payload.color = half3(colors[vertexId]);
    return payload;
}

half4 fragment fragmentMain(vertex_payload in [[stage_in]]) {
    return half4(in.color, 1.0);
}
