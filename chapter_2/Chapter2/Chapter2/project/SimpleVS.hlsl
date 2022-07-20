//入力として送られるデータ
struct VSInput
{
	//COLOR, POSITIONはセマンティクス名
	//CPUから送られたデータをどう解釈するかを定義する. 「入力」セマンティクスは自由に名前を付けられる
	float3 position : POSITION;		//POSITION: 座標データであることを定義
	float4 Color : COLOR;		//COLOR: 頂点カラーデータであることを定義
};

//出力データに頂点カラーをそのまま渡すことを考えたとき、どの位置に表示するかの座標データとカラー情報が必要
struct VSOutput
{
	//出力のセマンティクスは「システム値セマンティクス」を使用するものがある。
	//システム値セマンティクスはSV_で始まり、どのシェーダーステージで使用するかが仕様で決まっている
	//https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics#semantics-supported-only-for-direct3d-10-and-newer
	float4 position : SV_POSITION;		//位置座標 . システム値セマンティクスとして設定する必要があるので、仕様の通りにfloat4型にし、セマンティクス名をSV_POSITIONとしている
	float4 color : COLOR;		//頂点カラー。システムで定義されているものに設定する必要はないのでCOLORという名前にしている
};

//頂点データを加工するのに必要な定数バッファを追加
//cbuffer: 定数バッファを表す
//Transform: 定数バッファ名
//register(b0): 定数バッファ用レジスタの0番目を使用
cbuffer Transform : register(b0)
{
	//packoffset()は定数バッファの先頭からのオフセット指定
	//c0が先頭データ, c1は先頭からfloatが4つ分の16byte先を表す
	//float1つ分を表したい場合は次のように表す
	//float hoge0: packoffset(c0.x):
	//float hoge1: packoffset(c0.y):
	//float hoge2: packoffset(c0.z):
	//float hoge3: packoffset(c0.w):

	float4x4 World: packoffset(c0);		//ワールド行列
	float4x4 View: packoffset(c4);		//ビュー行列
	float4x4 Proj: packoffset(c8);		//射影行列
}

VSOutput main(VSInput input)
{
	VSOutput output = (VSOutput)0;

	float4 localPos = float4(input.position, 1.0f);
	float4 worldPos = mul(World, localPos);
	float4 viewPos = mul(View, worldPos);
	float4 projPos = mul(Proj, viewPos);

	output.position = projPos;
	output.color = input.Color;
	return output;
}