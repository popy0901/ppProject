#include "Pfc.hlsli"



[maxvertexcount(4)]

void main(point GS_OUT input[1], inout TriangleStream<PS_OUT> output)
{
    // ワールド　→　ビュー
    float4 pos = mul(View, float4(input[0].Position, 1.0));
        
	// 点を面にする
    float w = input[0].Tex.x;
    float h = input[0].Tex.y;
    
    float4 pos_left_top = pos + float4(-w, h, 0, 0);
    float4 pos_left_bottom = pos + float4(-w, -h, 0, 0);
    float4 pos_right_top = pos + float4(w, h, 0, 0);
    float4 pos_right_bottom = pos + float4(w, -h, 0, 0);
        
    // スケール, アングル
    {
        float sa, ca;
        sincos(input[0].Normal.x, sa, ca);
        float2 plt = pos_left_top.xy - pos.xy;
        float2 plb = pos_left_bottom.xy - pos.xy;
        float2 prt = pos_right_top.xy - pos.xy;
        float2 prb = pos_right_bottom.xy - pos.xy;
        
        // スケール
        //plt *= input[0].Normal.y;
        //plb *= input[0].Normal.y;
        //prt *= input[0].Normal.y;
        //prb *= input[0].Normal.y;
    
        float2 plt2 = plt;
        float2 plb2 = plb;
        float2 prt2 = prt;
        float2 prb2 = prb;
        
        plt2.x = (plt.x * ca) - (plt.y * sa);
        plt2.y = (plt.x * sa) + (plt.y * ca);
        plb2.x = (plb.x * ca) - (plb.y * sa);
        plb2.y = (plb.x * sa) + (plb.y * ca);
        prt2.x = (prt.x * ca) - (prt.y * sa);
        prt2.y = (prt.x * sa) + (prt.y * ca);
        prb2.x = (prb.x * ca) - (prb.y * sa);
        prb2.y = (prb.x * sa) + (prb.y * ca);
        
        pos_left_top.xy = plt2 + pos.xy;
        pos_left_bottom.xy = plb2 + pos.xy;
        pos_right_top.xy = prt2 + pos.xy;
        pos_right_bottom.xy = prb2 + pos.xy;
    }
    

	// 左上の点の位置・UV・色を計算して出力
    PS_OUT Out = (PS_OUT) 0;
    Out.Color = input[0].Color;
    Out.Position = mul(Projection, pos_left_top);
    Out.Tex = float2(0, 0);
    output.Append(Out);
	// 右上の点の位置(射影座標系) とテクスチャ座標の計算をして出力
    Out.Color = input[0].Color;
    Out.Position = mul(Projection, pos_right_top);
    Out.Tex = float2(1, 0);
    output.Append(Out);
	// 左下の点の位置(射影座標系) とテクスチャ座標の計算をして出力
    Out.Color = input[0].Color;
    Out.Position = mul(Projection, pos_left_bottom);
    Out.Tex = float2(0, 1);
    output.Append(Out);
	// 右下の点の位置(射影座標系) とテクスチャ座標の計算をして出力
    Out.Color = input[0].Color;
    Out.Position = mul(Projection, pos_right_bottom);
    Out.Tex = float2(1, 1);
    output.Append(Out);

    output.RestartStrip();
}

