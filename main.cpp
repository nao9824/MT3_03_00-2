#include <Novice.h>
#include "cmath"
#include <assert.h>
#include "imgui.h"
#define _USE_MATH_DEFINES
#include<math.h>
#include <algorithm>

const char kWindowTitle[] = "LD2B_08_ワタナベ_ナオ_タイトル";

struct Vector3 {
	float x;
	float y;
	float z;
};

struct Matrix4x4 {
	float m[4][4];
};

struct Sphere {
	Vector3 center;//中心点
	float radius;
};

//X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian);
//Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian);
//Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian);
//3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
//逆行列
Matrix4x4 Inverse(const Matrix4x4& m);
//投資投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRadio, float nearClip, float farClip);
//ビューポート行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
//積
Matrix4x4 MatrixMultiply(const Matrix4x4& m1, const Matrix4x4& m2);
//スカラー倍
Vector3 Multiply(float scalar, const Vector3 v);
//座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
//正規化
Vector3 Normalize(Vector3 vector);
//長さ
float Length(const Vector3& v);
//内積
float Dot(const Vector3& v1, const Vector3& v2);
//クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2);
Vector3 Add(const Vector3& v1, const Vector3& v2);
Vector3 Subtract(const Vector3& v1, const Vector3& v2);

void DrawGrit(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

Vector3 Lerp(const Vector3& a, const Vector3& b, float t);
Vector3 Bezier(const Vector3& p0, const Vector3& p1, const Vector3& p2, float t);
void DrawBezier(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

Vector3 CatmullRom(const Vector3& p0, const Vector3& p1,
	const Vector3& p2, const Vector3& p3, float t);
void DrawCatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Vector3 cameraTranslate{ 0.0f,1.9f,-6.49f };
	Vector3 cameraRotate{ 0.26f,0.0f,0.0f };
	Vector3 cameraPosition = { 0.0f,0.0f,-300.0f };
	int kWindowWidth = 1280;
	int kWindowHeight = 720;

	Vector3 controlPoints[4] = {
		{-0.8f,0.58f,1.0f},
		{1.76f,1.0f,-0.3f},
		{0.94f,-0.7f,2.3f},
		{-0.53f,-0.26f,-0.15f}
	};

	float radius = 0.03f;



	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

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

		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = MatrixMultiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);


		ImGui::Begin("Window");
		ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("controlPoints[0]", &controlPoints[0].x, 0.01f);
		ImGui::DragFloat3("controlPoints[1]", &controlPoints[1].x, 0.01f);
		ImGui::DragFloat3("controlPoints[2]", &controlPoints[2].x, 0.01f);
		ImGui::DragFloat3("controlPoints[3]", &controlPoints[3].x, 0.01f);

		ImGui::End();


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawGrit(worldViewProjectionMatrix, viewportMatrix);

		DrawCatmullRom(controlPoints[0], controlPoints[1], controlPoints[2], controlPoints[3],
			worldViewProjectionMatrix, viewportMatrix, BLUE);

		Sphere sphere0 = {
	controlPoints[0],
	radius
		};
		Sphere sphere1 = {
		controlPoints[1],
		radius
		};
		Sphere sphere2 = {
		controlPoints[2],
		radius
		};
		Sphere sphere3 = {
		controlPoints[3],
		radius
		};

		DrawSphere(sphere0, worldViewProjectionMatrix, viewportMatrix, BLACK);
		DrawSphere(sphere1, worldViewProjectionMatrix, viewportMatrix, BLACK);
		DrawSphere(sphere2, worldViewProjectionMatrix, viewportMatrix, BLACK);
		DrawSphere(sphere3, worldViewProjectionMatrix, viewportMatrix, BLACK);


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

//積
Matrix4x4 MatrixMultiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;

	result.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
	result.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
	result.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
	result.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];
	result.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
	result.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
	result.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
	result.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];
	result.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
	result.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
	result.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
	result.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];
	result.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
	result.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
	result.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
	result.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];

	return result;
}
//スカラー倍
Vector3 Multiply(float scalar, const Vector3 v) {
	Vector3 result;

	result.x = v.x * scalar;
	result.y = v.y * scalar;
	result.z = v.z * scalar;

	return result;
}
//X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 result;

	result.m[0][0] = 1;
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;
	result.m[1][0] = 0;
	result.m[1][1] = std::cos(radian);
	result.m[1][2] = std::sin(radian);
	result.m[1][3] = 0;
	result.m[2][0] = 0;
	result.m[2][1] = -std::sin(radian);
	result.m[2][2] = std::cos(radian);
	result.m[2][3] = 0;
	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;

	return result;
}

//Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 result;

	result.m[0][0] = std::cos(radian);
	result.m[0][1] = 0;
	result.m[0][2] = -std::sin(radian);
	result.m[0][3] = 0;
	result.m[1][0] = 0;
	result.m[1][1] = 1;
	result.m[1][2] = 0;
	result.m[1][3] = 0;
	result.m[2][0] = std::sin(radian);
	result.m[2][1] = 0;
	result.m[2][2] = std::cos(radian);
	result.m[2][3] = 0;
	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;

	return result;
}

//Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 result;

	result.m[0][0] = std::cos(radian);
	result.m[0][1] = std::sin(radian);
	result.m[0][2] = 0;
	result.m[0][3] = 0;
	result.m[1][0] = -std::sin(radian);
	result.m[1][1] = std::cos(radian);
	result.m[1][2] = 0;
	result.m[1][3] = 0;
	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = 1;
	result.m[2][3] = 0;
	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;

	return result;
}

//3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 result;
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 rotateXYZMatrix = MatrixMultiply(rotateXMatrix, MatrixMultiply(rotateYMatrix, rotateZMatrix));

	result.m[0][0] = scale.x * rotateXYZMatrix.m[0][0];
	result.m[0][1] = scale.x * rotateXYZMatrix.m[0][1];
	result.m[0][2] = scale.x * rotateXYZMatrix.m[0][2];
	result.m[0][3] = 0;
	result.m[1][0] = scale.y * rotateXYZMatrix.m[1][0];
	result.m[1][1] = scale.y * rotateXYZMatrix.m[1][1];
	result.m[1][2] = scale.y * rotateXYZMatrix.m[1][2];
	result.m[1][3] = 0;
	result.m[2][0] = scale.z * rotateXYZMatrix.m[2][0];
	result.m[2][1] = scale.z * rotateXYZMatrix.m[2][1];
	result.m[2][2] = scale.z * rotateXYZMatrix.m[2][2];
	result.m[2][3] = 0;
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1;

	return result;
}

//逆行列
Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 result;

	float A = m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]
		- m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]
		- m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]
		+ m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]
		+ m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]
		- m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]
		- m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]
		+ m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

	result.m[0][0] = (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2] - (m.m[1][3] * m.m[2][2] * m.m[3][1]) - (m.m[1][2] * m.m[2][1] * m.m[3][3]) - (m.m[1][1] * m.m[2][3] * m.m[3][2])) / A;
	result.m[0][1] = (-(m.m[0][1] * m.m[2][2] * m.m[3][3]) - (m.m[0][2] * m.m[2][3] * m.m[3][1]) - (m.m[0][3] * m.m[2][1] * m.m[3][2]) + m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]) / A;
	result.m[0][2] = (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2] - (m.m[0][3] * m.m[1][2] * m.m[3][1]) - (m.m[0][2] * m.m[1][1] * m.m[3][3]) - (m.m[0][1] * m.m[1][3] * m.m[3][2])) / A;
	result.m[0][3] = (-(m.m[0][1] * m.m[1][2] * m.m[2][3]) - (m.m[0][2] * m.m[1][3] * m.m[2][1]) - (m.m[0][3] * m.m[1][1] * m.m[2][2]) + m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]) / A;
	result.m[1][0] = (-(m.m[1][0] * m.m[2][2] * m.m[3][3]) - (m.m[1][2] * m.m[2][3] * m.m[3][0]) - (m.m[1][3] * m.m[2][0] * m.m[3][2]) + m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]) / A;
	result.m[1][1] = (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2] - (m.m[0][3] * m.m[2][2] * m.m[3][0]) - (m.m[0][2] * m.m[2][0] * m.m[3][3]) - (m.m[0][0] * m.m[2][3] * m.m[3][2])) / A;
	result.m[1][2] = (-(m.m[0][0] * m.m[1][2] * m.m[3][3]) - (m.m[0][2] * m.m[1][3] * m.m[3][0]) - (m.m[0][3] * m.m[1][0] * m.m[3][2]) + m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]) / A;
	result.m[1][3] = (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2] - (m.m[0][3] * m.m[1][2] * m.m[2][0]) - (m.m[0][2] * m.m[1][0] * m.m[2][3]) - (m.m[0][0] * m.m[1][3] * m.m[2][2])) / A;
	result.m[2][0] = (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1] - (m.m[1][3] * m.m[2][1] * m.m[3][0]) - (m.m[1][1] * m.m[2][0] * m.m[3][3]) - (m.m[1][0] * m.m[2][3] * m.m[3][1])) / A;
	result.m[2][1] = (-(m.m[0][0] * m.m[2][1] * m.m[3][3]) - (m.m[0][1] * m.m[2][3] * m.m[3][0]) - (m.m[0][3] * m.m[2][0] * m.m[3][1]) + m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]) / A;
	result.m[2][2] = (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1] - (m.m[0][3] * m.m[1][1] * m.m[3][0]) - (m.m[0][1] * m.m[1][0] * m.m[3][3]) - (m.m[0][0] * m.m[1][3] * m.m[3][1])) / A;
	result.m[2][3] = (-(m.m[0][0] * m.m[1][1] * m.m[2][3]) - (m.m[0][1] * m.m[1][3] * m.m[2][0]) - (m.m[0][3] * m.m[1][0] * m.m[2][1]) + m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]) / A;
	result.m[3][0] = (-(m.m[1][0] * m.m[2][1] * m.m[3][2]) - (m.m[1][1] * m.m[2][2] * m.m[3][0]) - (m.m[1][2] * m.m[2][0] * m.m[3][1]) + m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]) / A;
	result.m[3][1] = (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1] - (m.m[0][2] * m.m[2][1] * m.m[3][0]) - (m.m[0][1] * m.m[2][0] * m.m[3][2]) - (m.m[0][0] * m.m[2][2] * m.m[3][1])) / A;
	result.m[3][2] = (-(m.m[0][0] * m.m[1][1] * m.m[3][2]) - (m.m[0][1] * m.m[1][2] * m.m[3][0]) - (m.m[0][2] * m.m[1][0] * m.m[3][1]) + m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]) / A;
	result.m[3][3] = (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1] - (m.m[0][2] * m.m[1][1] * m.m[2][0]) - (m.m[0][1] * m.m[1][0] * m.m[2][2]) - (m.m[0][0] * m.m[1][2] * m.m[2][1])) / A;

	return result;
}

//投資投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRadio, float nearClip, float farClip) {
	Matrix4x4 result;

	result.m[0][0] = 1 / aspectRadio * (1 / std::tan(fovY / 2));
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;
	result.m[1][0] = 0;
	result.m[1][1] = 1 / std::tan(fovY / 2);
	result.m[1][2] = 0;
	result.m[1][3] = 0;
	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1;
	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = -(nearClip * farClip) / (farClip - nearClip);
	result.m[3][3] = 0;

	return result;
}

//ビューポート行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result;

	result.m[0][0] = width / 2;
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;
	result.m[1][0] = 0;
	result.m[1][1] = -(height / 2);
	result.m[1][2] = 0;
	result.m[1][3] = 0;
	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = maxDepth - minDepth;
	result.m[2][3] = 0;
	result.m[3][0] = left + (width / 2);
	result.m[3][1] = top + (height / 2);
	result.m[3][2] = minDepth;
	result.m[3][3] = 1;

	return result;
}

//座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
}

//グリッド線の描画
void DrawGrit(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;//Gridの半分の幅
	const uint32_t kSubdivision = 10;//分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);//一つ分の長さ
	Vector3 zLineStart;
	Vector3 zLineEnd;
	Vector3 xLineStart;
	Vector3 xLineEnd;

	//奥から手前への線を順々に引いていく
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		//ワールド座標系上の始点と終点を求める
		zLineStart = Vector3(xIndex * kGridEvery - kGridHalfWidth, 0, kGridHalfWidth);
		zLineEnd = Vector3(xIndex * kGridEvery - kGridHalfWidth, 0, -kGridHalfWidth);
		//スクリーン座標系まで変換をかける
		Matrix4x4 zStartWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, Vector3{}, zLineStart);
		Matrix4x4 zStartworldViewProjectionMatrix = MatrixMultiply(zStartWorldMatrix, viewProjectionMatrix);
		Vector3 zStartNdcVertex = Transform(Vector3{}, zStartworldViewProjectionMatrix);
		Vector3 zStartScreenVertice = Transform(zStartNdcVertex, viewportMatrix);

		Matrix4x4 zEndWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, Vector3{}, zLineEnd);
		Matrix4x4 zEndworldViewProjectionMatrix = MatrixMultiply(zEndWorldMatrix, viewProjectionMatrix);
		Vector3 zEndNdcVertex = Transform(Vector3{}, zEndworldViewProjectionMatrix);
		Vector3 zEndScreenVertice = Transform(zEndNdcVertex, viewportMatrix);

		if (xIndex == 5) {
			Novice::DrawLine((int)zStartScreenVertice.x, (int)zStartScreenVertice.y, (int)zEndScreenVertice.x, (int)zEndScreenVertice.y, BLACK);
		}
		else {
			Novice::DrawLine((int)zStartScreenVertice.x, (int)zStartScreenVertice.y, (int)zEndScreenVertice.x, (int)zEndScreenVertice.y, 0xAAAAAAFF);
		}
	}

	//左から右も同じように順々に引いていく
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		//ワールド座標系上の始点と終点を求める
		xLineStart = Vector3(kGridHalfWidth, 0, zIndex * kGridEvery - kGridHalfWidth);
		xLineEnd = Vector3(-kGridHalfWidth, 0, zIndex * kGridEvery - kGridHalfWidth);
		//スクリーン座標系まで変換をかける
		Matrix4x4 xStartWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, Vector3{}, xLineStart);
		Matrix4x4 xStartworldViewProjectionMatrix = MatrixMultiply(xStartWorldMatrix, viewProjectionMatrix);
		Vector3 xStartNdcVertex = Transform(Vector3{}, xStartworldViewProjectionMatrix);
		Vector3 xStartScreenVertice = Transform(xStartNdcVertex, viewportMatrix);

		Matrix4x4 xEndWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, Vector3{}, xLineEnd);
		Matrix4x4 xEndworldViewProjectionMatrix = MatrixMultiply(xEndWorldMatrix, viewProjectionMatrix);
		Vector3 xEndNdcVertex = Transform(Vector3{}, xEndworldViewProjectionMatrix);
		Vector3 xEndScreenVertice = Transform(xEndNdcVertex, viewportMatrix);

		if (zIndex == 5) {
			Novice::DrawLine((int)xStartScreenVertice.x, (int)xStartScreenVertice.y, (int)xEndScreenVertice.x, (int)xEndScreenVertice.y, BLACK);
		}
		else {
			Novice::DrawLine((int)xStartScreenVertice.x, (int)xStartScreenVertice.y, (int)xEndScreenVertice.x, (int)xEndScreenVertice.y, 0xAAAAAAFF);
		}
	}
}

//正規化
Vector3 Normalize(Vector3 vector) {
	float lenght;
	Vector3 result{};

	lenght = sqrtf((vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z));
	if (lenght != 0) {
		result.x = vector.x / lenght;
		result.y = vector.y / lenght;
		result.z = vector.z / lenght;
	}
	return result;
}
//長さ（ノルム）
float Length(const Vector3& v) {
	float result;

	result = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);

	return result;
}
//内積
float Dot(const Vector3& v1, const Vector3& v2) {
	Vector3 v3;
	float result;

	v3.x = v1.x * v2.x;
	v3.y = v1.y * v2.y;
	v3.z = v1.z * v2.z;

	result = v3.x + v3.y + v3.z;

	return result;
}
//クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result;

	result = { v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x };

	return result;
}
//加算
Vector3 Add(const Vector3& v1, const Vector3& v2) {
	Vector3 result;

	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;

	return result;
}
//減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	Vector3 result;

	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;

	return result;
}

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	float pi = (float)M_PI;
	const uint32_t kSubdivision = 10; //分割数
	const float kLonEvery = pi / kSubdivision; //経度分割一つ分の角度
	const float kLatEvery = (2 * pi) / kSubdivision; //緯度分割一つ分の角度
	//ワールド座標系でのa,b,cを求める
	Vector3 a, b, c;

	//緯度の方向に分割 -π/2 ~ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -pi / 2.0f + kLatEvery * latIndex;//現在の緯度
		//経度の方向に分割 0~2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;//現在の経度
			//ワールド座標系での頂点を求める
			a = { (sphere.radius) * (cos(lat) * cos(lon)) + sphere.center.x,
				(sphere.radius) * (sin(lat)) + sphere.center.y,
				(sphere.radius) * (cos(lat) * sin(lon)) + sphere.center.z };
			b = { (sphere.radius) * (cos(lat + kLatEvery) * cos(lon)) + sphere.center.x,
				(sphere.radius) * (sin(lat + kLatEvery)) + sphere.center.y,
				(sphere.radius) * (cos(lat + kLatEvery) * sin(lon)) + sphere.center.z };
			c = { (sphere.radius) * (cos(lat) * cos(lon + kLonEvery)) + sphere.center.x,
				(sphere.radius) * (sin(lat)) + sphere.center.y,
				(sphere.radius) * (cos(lat) * sin(lon + kLonEvery)) + sphere.center.z };
			//a,b,cをスクリーン座標系まで変換
			Matrix4x4 aWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, Vector3{}, a);
			Matrix4x4 aworldViewProjectionMatrix = MatrixMultiply(aWorldMatrix, viewProjectionMatrix);
			Vector3 aNdcVertex = Transform(Vector3{}, aworldViewProjectionMatrix);
			Vector3 aScreenVertice = Transform(aNdcVertex, viewportMatrix);

			Matrix4x4 bWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, Vector3{}, b);
			Matrix4x4 bworldViewProjectionMatrix = MatrixMultiply(bWorldMatrix, viewProjectionMatrix);
			Vector3 bNdcVertex = Transform(Vector3{}, bworldViewProjectionMatrix);
			Vector3 bScreenVertice = Transform(bNdcVertex, viewportMatrix);

			Matrix4x4 cWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, Vector3{}, c);
			Matrix4x4 cworldViewProjectionMatrix = MatrixMultiply(cWorldMatrix, viewProjectionMatrix);
			Vector3 cNdcVertex = Transform(Vector3{}, cworldViewProjectionMatrix);
			Vector3 cScreenVertice = Transform(cNdcVertex, viewportMatrix);

			Novice::DrawLine((int)aScreenVertice.x, (int)aScreenVertice.y, (int)bScreenVertice.x, (int)bScreenVertice.y, color);
			Novice::DrawLine((int)aScreenVertice.x, (int)aScreenVertice.y, (int)cScreenVertice.x, (int)cScreenVertice.y, color);

		}
	}
}

Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
	Vector3 result;

	result.x = t * a.x + (1.0f - t) * b.x;
	result.y = t * a.y + (1.0f - t) * b.y;
	result.z = t * a.z + (1.0f - t) * b.z;

	return result;
}
Vector3 Bezier(const Vector3& p0, const Vector3& p1,
	const Vector3& p2, float t) {
	Vector3 p0p1 = Lerp(p0, p1, t);
	Vector3 p1p2 = Lerp(p1, p2, t);
	Vector3 p = Lerp(p0p1, p1p2, t);

	return p;
}
void DrawBezier(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {

	int div = 32;

	for (float i = 0.0f; i < div; i++) {
		float t0 = i / (float)div;
		float t1 = (i + 1) / (float)div;

		Vector3 bezier0 = Bezier(p0, p1, p2, t0);
		Vector3 bezier1 = Bezier(p0, p1, p2, t1);



		//座標変換
		Vector3 bezier0NdcVertex = Transform(bezier0, viewProjectionMatrix);
		Vector3 bezier0ScreenVertice = Transform(bezier0NdcVertex, viewportMatrix);

		Vector3 bezier1NdcVertex = Transform(bezier1, viewProjectionMatrix);
		Vector3 bezier1ScreenVertice = Transform(bezier1NdcVertex, viewportMatrix);

		Vector3 p0NdcVertex = Transform(p0, viewProjectionMatrix);
		Vector3 p0ScreenVertice = Transform(p0NdcVertex, viewportMatrix);

		Vector3 p1NdcVertex = Transform(p1, viewProjectionMatrix);
		Vector3 p1ScreenVertice = Transform(p1NdcVertex, viewportMatrix);

		Vector3 p2NdcVertex = Transform(p2, viewProjectionMatrix);
		Vector3 p2ScreenVertice = Transform(p2NdcVertex, viewportMatrix);


		Novice::DrawLine((int)bezier0ScreenVertice.x, (int)bezier0ScreenVertice.y, (int)bezier1ScreenVertice.x, (int)bezier1ScreenVertice.y, color);


	}
}
Vector3 CatmullRom(const Vector3& p0, const Vector3& p1,
	const Vector3& p2, const Vector3& p3, float t) {
	Vector3 result = {};

	result.x = { ((-p0.x + 3 * p1.x - 3 * p2.x + p3.x) * t * t * t +
	(2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x) * t * t +
	(-p0.x + p2.x) * t +
	2 * p1.x) / 2 };

	result.y = { ((-p0.y + 3 * p1.y - 3 * p2.y + p3.y) * t * t * t +
	(2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y) * t * t +
	(-p0.y + p2.y) * t +
	2 * p1.y) / 2 };

	result.z = { ((-p0.z + 3 * p1.z - 3 * p2.z + p3.z) * t * t * t +
	(2 * p0.z - 5 * p1.z + 4 * p2.z - p3.z) * t * t +
	(-p0.z + p2.z) * t +
	2 * p1.z) / 2 };

	return result;
}

void DrawCatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	int div = 16;
	for (float i = 0; i < div; i++) {
		float t0 = i / (float)div;
		float t1 = (i + 1) / (float)div;
		//float t2 = (i + 2) / (float)div;

		Vector3 catmullRom00 = CatmullRom(p0, p0, p1, p2, t0);
		Vector3 catmullRom01 = CatmullRom(p0, p0, p1, p2, t1);
		Vector3 catmullRom10 = CatmullRom(p0, p1, p2, p3, t0);
		Vector3 catmullRom11 = CatmullRom(p0, p1, p2, p3, t1);
		Vector3 catmullRom20 = CatmullRom(p1, p2, p3, p3, t0);
		Vector3 catmullRom21 = CatmullRom(p1, p2, p3, p3, t1);

		//座標変換
		Vector3 catmullRom00NdcVertex = Transform(catmullRom00, viewProjectionMatrix);
		Vector3 catmullRom00ScreenVertice = Transform(catmullRom00NdcVertex, viewportMatrix);

		Vector3 catmullRom01NdcVertex = Transform(catmullRom01, viewProjectionMatrix);
		Vector3 catmullRom01ScreenVertice = Transform(catmullRom01NdcVertex, viewportMatrix);

		Vector3 catmullRom10NdcVertex = Transform(catmullRom10, viewProjectionMatrix);
		Vector3 catmullRom10ScreenVertice = Transform(catmullRom10NdcVertex, viewportMatrix);

		Vector3 catmullRom11NdcVertex = Transform(catmullRom11, viewProjectionMatrix);
		Vector3 catmullRom11ScreenVertice = Transform(catmullRom11NdcVertex, viewportMatrix);

		Vector3 catmullRom20NdcVertex = Transform(catmullRom20, viewProjectionMatrix);
		Vector3 catmullRom20ScreenVertice = Transform(catmullRom20NdcVertex, viewportMatrix);

		Vector3 catmullRom21NdcVertex = Transform(catmullRom21, viewProjectionMatrix);
		Vector3 catmullRom21ScreenVertice = Transform(catmullRom21NdcVertex, viewportMatrix);

		Novice::DrawLine((int)catmullRom00ScreenVertice.x, (int)catmullRom00ScreenVertice.y,
			(int)catmullRom01ScreenVertice.x, (int)catmullRom01ScreenVertice.y, color);
		Novice::DrawLine((int)catmullRom10ScreenVertice.x, (int)catmullRom10ScreenVertice.y,
			(int)catmullRom11ScreenVertice.x, (int)catmullRom11ScreenVertice.y, color);
		Novice::DrawLine((int)catmullRom20ScreenVertice.x, (int)catmullRom20ScreenVertice.y,
			(int)catmullRom21ScreenVertice.x, (int)catmullRom21ScreenVertice.y, color);
	}
}