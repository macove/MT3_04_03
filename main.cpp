#include <Novice.h>
#include "GeometryUtility.h"
#include <imgui.h>

const char kWindowTitle[] = "学籍番号";

GeometryUtility geometryUtility;

struct  Pendulum
{
	Vector3 anchor;
	float length;
	float halfApexAngle;
	float angle;
	float angularVelocity;
};



// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Pendulum conicalPendulum;
	conicalPendulum.anchor = { 0.0f,1.0f,0.0f };
	conicalPendulum.length = 0.8f;
	conicalPendulum.halfApexAngle = 0.7f;
	conicalPendulum.angle = 0.0f;
	conicalPendulum.angularVelocity = 0.0f;

	int kWindowWidth = 1280;
	int kWindowHeight = 720;

	float radius = 10.0f;
	float theta = -3.46f;
	float phi = 1.5f;

	Vector3 rotate{ 0.0f,0.0f,0.0f };

	Vector3 translate{ 0.0f,0.0f,0.0f };
	Vector3 startLine{ 0.0f,1.5f,0.0f };

	float deltaTime = 1.0f / 60.0f;

	bool isMoving = false;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///


		Vector3 p;

		if (isMoving) {
			conicalPendulum.angularVelocity = std::sqrt(9.8f / (conicalPendulum.length * std::cos(conicalPendulum.halfApexAngle)));
			conicalPendulum.angle += conicalPendulum.angularVelocity * deltaTime;
		}

		float conicalRadius = std::sin(conicalPendulum.halfApexAngle) * conicalPendulum.length;
		float conicalHeight = std::sin(conicalPendulum.halfApexAngle) * conicalPendulum.length;

		p.x = conicalPendulum.anchor.x + std::cos(conicalPendulum.angle) * conicalRadius;
		p.y = conicalPendulum.anchor.y - conicalHeight;
		p.z = conicalPendulum.anchor.z - std::sin(conicalPendulum.angle) * conicalRadius;




		Vector3 cameraPosition = geometryUtility.SphericalToCartesian(radius, theta, phi);
		Vector3 cameraTarget = { 0.0f, 0.0f, 0.0f };
		Vector3 cameraUp = { 0.0f, 1.0f, 0.0f };

		Matrix4x4 viewMatrix = geometryUtility.MakeLookAtMatrix4x4(cameraPosition, cameraTarget, cameraUp);
		Matrix4x4 worldMatrix = geometryUtility.MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotate, translate);
		Matrix4x4 projectionMatrix = geometryUtility.MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = geometryUtility.Multiply(worldMatrix, geometryUtility.Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewProjectionMatrix = geometryUtility.Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = geometryUtility.MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		Vector3 startLineP = geometryUtility.Transform(geometryUtility.Transform(startLine, viewProjectionMatrix), viewportMatrix);

		Vector3 endLine = geometryUtility.Transform(geometryUtility.Transform(p, viewProjectionMatrix), viewportMatrix);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		geometryUtility.DrawGrid(viewProjectionMatrix, viewportMatrix);

		geometryUtility.DrawSphere({ p,0.1f }, viewProjectionMatrix, viewportMatrix, 0xFFFFFFFF);


		Novice::DrawLine(int(startLineP.x), int(startLineP.y), int(endLine.x), int(endLine.y), 0xFFFFFFFF);

		ImGui::Begin("Window");
		ImGui::DragFloat("theta", &theta, 0.01f);
		ImGui::DragFloat("phi", &phi, 0.01f);
		ImGui::Text("Uniform Circular Motion");
		if (ImGui::Button("Start")) {
			isMoving = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop")) {
			isMoving = false;
		}
		ImGui::SliderFloat("length", &conicalPendulum.length, 0.00f,1.0f);
		ImGui::SliderFloat("halfApexAngle", &conicalPendulum.halfApexAngle, 0.0f,1.0f);
		ImGui::End();


		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
