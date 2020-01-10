#include "HeightMap.h"

void UHeightMap::Resize(int32 X, int32 Y, int32 Z)
{
	if (X <= 0 || Y <= 0 || Z <= 0)
	{
		return;
	}

	WidthX = X;
	WidthY = Y;
	MaxHeight = Z;

	MapData.SetNumZeroed(WidthX * WidthY, true);
}

float UHeightMap::BPGetHeight(int32 X, int32 Y) const
{
	if (X < 0 || Y < 0)
	{
		return 0.0f;
	}

	return GetHeight(X, Y);
}

float UHeightMap::GetHeight(uint32 X, uint32 Y) const
{
	return MapData[Y * WidthX + X];
}

void UHeightMap::SetHeight(uint32 X, uint32 Y, float Height)
{
	MapData[Y * WidthX + X] = Height;
}

int32 UHeightMap::GetWidthX() const
{
	return WidthX;
}

int32 UHeightMap::GetWidthY() const
{
	return WidthY;
}

void UHeightMap::CalculateNormalsAndTangents(TArray<FVector>& Normals, TArray<FProcMeshTangent>& Tangents) const
{
	// Resize the normal and tangent arrays
	Normals.SetNum(MapData.Num());
	Tangents.SetNum(MapData.Num());

	// Calculate normals
	for (int32 y = 1; y < WidthY - 1; ++y)
	{
		for (int32 x = 1; x < WidthX - 1; ++x)
		{
			float s01 = GetHeight(x - 1, y) * MaxHeight;
			float s21 = GetHeight(x + 1, y) * MaxHeight;
			float s10 = GetHeight(x, y - 1) * MaxHeight;
			float s12 = GetHeight(x, y + 1) * MaxHeight;

			// Get tangents in the x and y directions
			FVector vx(2.0f, 0, s21 - s01);
			FVector vy(0, 2.0f, s10 - s12);

			// Calculate the cross product of the two normals
			vx.Normalize();
			vy.Normalize();
			Normals[y * WidthX + x] = FVector::CrossProduct(vx, vy);
			Tangents[y * WidthX + x] = FProcMeshTangent(vx.X, vx.Y, vx.Z);
		}
	}

	// Calculate normals on the edges
	for (int32 x = 1; x < WidthX - 1; ++x)
	{
		// Top edge
		float s01 = GetHeight(x - 1, 0) * MaxHeight;
		float s21 = GetHeight(x + 1, 0) * MaxHeight;
		float s10 = GetHeight(x, 0) * MaxHeight;
		float s12 = GetHeight(x, 1) * MaxHeight;

		FVector vx(2.0f, 0, s21 - s01);
		FVector vy(0, 2.0f, s10 - s12);

		vx.Normalize();
		vy.Normalize();
		Normals[x] = FVector::CrossProduct(vx, vy);
		Tangents[x] = FProcMeshTangent(vx.X, vx.Y, vx.Z);

		// Bottom edge
		int32 y = WidthY - 1;
		s01 = GetHeight(x - 1, y) * MaxHeight;
		s21 = GetHeight(x + 1, y) * MaxHeight;
		s10 = GetHeight(x, y - 1) * MaxHeight;
		s12 = GetHeight(x, y) * MaxHeight;

		vx = { 2.0f, 0, s21 - s01 };
		vy = { 0, 2.0f, s10 - s12 };

		vx.Normalize();
		vy.Normalize();
		Normals[y * WidthX + x] = FVector::CrossProduct(vx, vy);
		Tangents[y * WidthX + x] = FProcMeshTangent(vx.X, vx.Y, vx.Z);
	}

	for (int32 y = 1; y < WidthY - 1; ++y)
	{
		// Left edge
		float s01 = GetHeight(0, y) * MaxHeight;
		float s21 = GetHeight(1, y) * MaxHeight;
		float s10 = GetHeight(0, y - 1) * MaxHeight;
		float s12 = GetHeight(0, y + 1) * MaxHeight;

		FVector vx(2.0f, 0, s21 - s01);
		FVector vy(0, 2.0f, s10 - s12);

		vx.Normalize();
		vy.Normalize();
		Normals[y * WidthX] = FVector::CrossProduct(vx, vy);
		Tangents[y * WidthX] = FProcMeshTangent(vx.X, vx.Y, vx.Z);

		// Right edge
		int32 x = WidthX - 1;
		s01 = GetHeight(x - 1, y) * MaxHeight;
		s21 = GetHeight(x, y) * MaxHeight;
		s10 = GetHeight(x, y - 1) * MaxHeight;
		s12 = GetHeight(x, y + 1) * MaxHeight;

		vx = { 2.0f, 0, s21 - s01 };
		vy = { 0, 2.0f, s10 - s12 };

		vx.Normalize();
		vy.Normalize();
		Normals[y * WidthX + x] = FVector::CrossProduct(vx, vy);
		Tangents[y * WidthX + x] = FProcMeshTangent(vx.X, vx.Y, vx.Z);
	}

	// Get the corners last

	// Top left corner
	int32 x = 0;
	int32 y = 0;
	float s01 = GetHeight(x, y) * MaxHeight;
	float s21 = GetHeight(x + 1, y) * MaxHeight;
	float s10 = GetHeight(x, y) * MaxHeight;
	float s12 = GetHeight(x, y + 1) * MaxHeight;

	FVector vx(2.0f, 0, s21 - s01);
	FVector vy(0, 2.0f, s10 - s12);

	vx.Normalize();
	vy.Normalize();
	Normals[y * WidthX + x] = FVector::CrossProduct(vx, vy);
	Tangents[y * WidthX + x] = FProcMeshTangent(vx.X, vx.Y, vx.Z);

	// Top right corner
	x = WidthX - 1;
	y = 0;
	s01 = GetHeight(x - 1, y) * MaxHeight;
	s21 = GetHeight(x, y) * MaxHeight;
	s10 = GetHeight(x, y) * MaxHeight;
	s12 = GetHeight(x, y + 1) * MaxHeight;

	vx = { 2.0f, 0, s21 - s01 };
	vy = { 0, 2.0f, s10 - s12 };

	vx.Normalize();
	vy.Normalize();
	Normals[y * WidthX + x] = FVector::CrossProduct(vx, vy);
	Tangents[y * WidthX + x] = FProcMeshTangent(vx.X, vx.Y, vx.Z);

	// Bottom left corner
	x = 0;
	y = WidthY - 1;
	s01 = GetHeight(x, y) * MaxHeight;
	s21 = GetHeight(x + 1, y) * MaxHeight;
	s10 = GetHeight(x, y - 1) * MaxHeight;
	s12 = GetHeight(x, y) * MaxHeight;

	vx = { 2.0f, 0, s21 - s01 };
	vy = { 0, 2.0f, s10 - s12 };

	vx.Normalize();
	vy.Normalize();
	Normals[y * WidthX + x] = FVector::CrossProduct(vx, vy);
	Tangents[y * WidthX + x] = FProcMeshTangent(vx.X, vx.Y, vx.Z);

	// Bottom right corner
	x = WidthX - 1;
	y = WidthY - 1;
	s01 = GetHeight(x - 1, y) * MaxHeight;
	s21 = GetHeight(x, y) * MaxHeight;
	s10 = GetHeight(x, y - 1) * MaxHeight;
	s12 = GetHeight(x, y) * MaxHeight;

	vx = { 2.0f, 0, s21 - s01 };
	vy = { 0, 2.0f, s10 - s12 };

	vx.Normalize();
	vy.Normalize();
	Normals[y * WidthX + x] = FVector::CrossProduct(vx, vy);
	Tangents[y * WidthX + x] = FProcMeshTangent(vx.X, vx.Y, vx.Z);
}