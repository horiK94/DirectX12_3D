struct PSInput
{
	//vertex shaderと同じ入力を指定する
	float4 Position : SV_POSITION;		//位置座標 . システム値セマンティクスとして設定する必要があるので、仕様の通りにfloat4型にし、セマンティクス名をSV_POSITIONとしている
	float4 Color : COLOR;		//頂点カラー。システムで定義されているものに設定する必要はないのでCOLORという名前にしている
};

//ピクセルシェーダーの出力先は出力先
struct PSOutput
{
	//RGBAの4要素で構成されるピクセル(での)値をレンダーターゲットのフォーマットとして指定しているので、出力はそれに合わせる
	//レンダーターゲットは1枚しか使用しないのでSV_TARGET0を指定する
	float4 Color : SV_TARGET0;
	//SV_TARGET: システム値セマンティクス. レンダーターゲット配列の0番目に書き込みをするのでSV_TARGET0を指定
	//1番目のれんだーターゲットに書き込みをする場合はSV_TARGET1
};

PSOutput main(PSInput input) : SV_TARGET
{
	//初期化
	PSOutput output = (PSOutput)input;
	output.Color = input.Color;
	return output;
}