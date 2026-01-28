#pragma once
namespace nsTMEngine2
{
	class Vector3
	{
	public:
		union
		{
			XMFLOAT3 vec;
			float v[3];
			struct { float x, y, z; };
		};

		static const Vector3 Zero;
		static const Vector3 One;
		static const Vector3 Right;
		static const Vector3 Left;
		static const Vector3 Up;
		static const Vector3 Down;
		static const Vector3 Front;
		static const Vector3 Back;
		static const Vector3 AxisX;
		static const Vector3 AxisY;
		static const Vector3 AxisZ;
	
	public:
		operator XMVECTOR() const {
			return XMLoadFloat3(&vec);
		}

		explicit Vector3() {
			x = y = x = 0.0f;
		}

		Vector3(float x, float y, float z)
		{
			vec.x = x;
			vec.y = y;
			vec.z = z;
		}


		Vector3& operator=(const Vector3& _v) {
			vec = _v.vec;
			return *this;
		}

		Vector3& operator+=(const Vector3& _v) {
			XMVECTOR xmv0 = XMLoadFloat3(&vec);
			XMVECTOR xmv1 = XMLoadFloat3(&_v.vec);
			XMVECTOR xmvr = XMVectorAdd(xmv0, xmv1);
			XMStoreFloat3(&vec, xmvr);
		}

		Vector3& operator-=(const Vector3& _v) {
			XMVECTOR xmv0 = XMLoadFloat3(&vec);
			XMVECTOR xmv1 = XMLoadFloat3(&_v.vec);
			XMVECTOR xmvr = XMVectorSubtract(xmv0, xmv1);
			XMStoreFloat3(&vec, xmvr);
		}

		Vector3& operator*=(const float scale) {
			XMVECTOR xmv = XMLoadFloat3(&vec);
			xmv = XMVectorScale(xmv, scale);
			XMStoreFloat3(&vec, xmv);
		}

		Vector3& operator*=(const float div) {
			float scale = 1.0f / div;
			XMVECTOR xmv = XMLoadFloat3(&vec);
			xmv = XMVectorScale(xmv, scale);
			XMStoreFloat3(&vec, xmv);
		}

		float Lerp(float t, const Vector3& _v0, const Vector3& _v1) {
			XMVECTOR xmv = XMVectorLerp(
				XMLoadFloat3(&_v0.vec), 
				XMLoadFloat3(&_v1.vec), 
				t
			);
			XMStoreFloat3(&vec, xmv);
		}

		float Dot(const Vector3& _v) {
			XMVECTOR xmv0 = XMLoadFloat3(&vec);
			XMVECTOR xmv1 = XMLoadFloat3(&_v.vec);
			return XMVector3Dot(xmv0, xmv1).m128_f32[0];
		}

		float Cross(const Vector3& _v) {
			XMVECTOR xmv0 = XMLoadFloat3(&vec);
			XMVECTOR xmv1 = XMLoadFloat3(&_v.vec);
			XMVECTOR xmvr = XMVector3Cross(xmv0, xmv1);
			XMStoreFloat3(&vec, xmvr);
		}

		float Cross(const Vector3& _v0, const Vector3& _v1) {
			XMVECTOR xmv0 = XMLoadFloat3(&_v0.vec);
			XMVECTOR xmv1 = XMLoadFloat3(&_v1.vec);
			XMVECTOR xmvr = XMVector3Cross(xmv0, xmv1);
			XMStoreFloat3(&vec, xmvr);
		}

		float Length() {
			XMVECTOR xmv = XMLoadFloat3(&vec);
			return XMVector3Length(xmv).m128_f32[0];
		}

		float Normalize() {
			XMVECTOR xmv = XMLoadFloat3(&vec);
			xmv = XMVector3Normalize(xmv);
			XMStoreFloat3(&vec, xmv);
		}
	};

	class Vector4 {
	public:
		union {
			DirectX::XMFLOAT4 vec;
			struct { float x, y, z, w; };
			struct { float r, g, b, a; };
			float v[4];
		};
		static const Vector4 White;
		static const Vector4 Black;
		static const Vector4 Yellow;
		static const Vector4 Gray;

	public:
		operator XMVECTOR() const
		{
			return XMLoadFloat4(&vec);
		}

		explicit Vector4()
		{
			x = y = z = 0.0f;
			w = 1.0f;
		}

		Vector4(float x, float y, float z, float w)
		{
			vec.x = x;
			vec.y = y;
			vec.z = z;
			vec.w = w;
		}

		Vector4& operator=(const Vector4& _v) {
			vec = _v.vec;
			return *this;
		}

		Vector4& operator+=(const Vector4& _v) {
			XMVECTOR xmv0 = XMLoadFloat4(&vec);
			XMVECTOR xmv1 = XMLoadFloat4(&_v.vec);
			XMVECTOR xmvr = XMVectorAdd(xmv0, xmv1);
			XMStoreFloat4(&vec, xmvr);
		}

		Vector4& operator-=(const Vector4& _v) {
			XMVECTOR xmv0 = XMLoadFloat4(&vec);
			XMVECTOR xmv1 = XMLoadFloat4(&_v.vec);
			XMVECTOR xmvr = XMVectorSubtract(xmv0, xmv1);
			XMStoreFloat4(&vec, xmvr);
		}

		Vector4& operator*=(const float scale) {
			XMVECTOR xmv = XMLoadFloat4(&vec);
			xmv = XMVectorScale(xmv, scale);
			XMStoreFloat4(&vec, xmv);
		}

		Vector4& operator*=(const float div) {
			float scale = 1.0f / div;
			XMVECTOR xmv = XMLoadFloat4(&vec);
			xmv = XMVectorScale(xmv, scale);
			XMStoreFloat4(&vec, xmv);
		}

		float Lerp(float t, const Vector4& _v0, const Vector4& _v1) {
			XMVECTOR xmv = XMVectorLerp(
				XMLoadFloat4(&_v0.vec),
				XMLoadFloat4(&_v1.vec),
				t
			);
			XMStoreFloat4(&vec, xmv);
		}

		float Dot(const Vector4& _v) {
			XMVECTOR xmv0 = XMLoadFloat4(&vec);
			XMVECTOR xmv1 = XMLoadFloat4(&_v.vec);
			return XMVector4Dot(xmv0, xmv1).m128_f32[0];
		}

		float Length() {
			XMVECTOR xmv = XMLoadFloat4(&vec);
			return XMVector4Length(xmv).m128_f32[0];
		}

	};
}