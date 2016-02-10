#include "precompiled.h"
#include "rehlds_tests_shared.h"
#include "cppunitlite/TestHarness.h"
#include <iostream>

TEST(AngleVectorsTest, MathLib, 1000) {
	struct testdata_t {
		vec3_t angles;
		vec3_t forward, right, up;
	};

	testdata_t testdata[2] = {
			{ { 21.0f, 65.0f, 162.0f }, { 0.39455f, 0.84611f, -0.35837f }, { -0.90875f, 0.30156f, -0.28849f }, { 0.13602f, -0.43949f, -0.88789f } },
			{ { 106.0f, 142.0f, 62.0f }, { 0.21721f, -0.16970f, -0.96126f }, { 0.95785f, -0.15259f, 0.24337f }, { 0.18798f, 0.97361f, -0.12940f } }
	};

	vec3_t forward, right, up;

	for (int i = 0; i < ARRAYSIZE(testdata); i++) {
		AngleVectors(testdata[i].angles, forward, right, up);

		DOUBLES_EQUAL("forward[0] mismatch", testdata[i].forward[0], forward[0], 0.00001);
		DOUBLES_EQUAL("forward[1] mismatch", testdata[i].forward[1], forward[1], 0.00001);
		DOUBLES_EQUAL("forward[2] mismatch", testdata[i].forward[2], forward[2], 0.00001);

		DOUBLES_EQUAL("right[0] mismatch", testdata[i].right[0], right[0], 0.00001);
		DOUBLES_EQUAL("right[1] mismatch", testdata[i].right[1], right[1], 0.00001);
		DOUBLES_EQUAL("right[2] mismatch", testdata[i].right[2], right[2], 0.00001);

		DOUBLES_EQUAL("up[0] mismatch", testdata[i].up[0], up[0], 0.00001);
		DOUBLES_EQUAL("up[1] mismatch", testdata[i].up[1], up[1], 0.00001);
		DOUBLES_EQUAL("up[2] mismatch", testdata[i].up[2], up[2], 0.00001);
	}
}

TEST(AngleVectorsTransposeTest, MathLib, 1000) {
	struct testdata_t {
		vec3_t angles;
		vec3_t forward, right, up;
	};

	testdata_t testdata[2] = {
			{ { 21.0f, 65.0f, 162.0f }, { 0.39455f, 0.90875f, 0.13602f }, { 0.84611f, -0.30157f, -0.43949f }, { -0.35837f, 0.28849f, -0.88789f } },
			{ { 106.0f, 142.0f, 62.0f }, { 0.21721f, -0.95785f, 0.18798f }, { -0.16970f, 0.15259f, 0.97361f }, { -0.96126f, -0.24337f, -0.12940f } }
	};

	vec3_t forward, right, up;

	for (int i = 0; i < ARRAYSIZE(testdata); i++) {
		AngleVectorsTranspose(testdata[i].angles, forward, right, up);

		DOUBLES_EQUAL("forward[0] mismatch", testdata[i].forward[0], forward[0], 0.00001);
		DOUBLES_EQUAL("forward[1] mismatch", testdata[i].forward[1], forward[1], 0.00001);
		DOUBLES_EQUAL("forward[2] mismatch", testdata[i].forward[2], forward[2], 0.00001);

		DOUBLES_EQUAL("right[0] mismatch", testdata[i].right[0], right[0], 0.00001);
		DOUBLES_EQUAL("right[1] mismatch", testdata[i].right[1], right[1], 0.00001);
		DOUBLES_EQUAL("right[2] mismatch", testdata[i].right[2], right[2], 0.00001);

		DOUBLES_EQUAL("up[0] mismatch", testdata[i].up[0], up[0], 0.00001);
		DOUBLES_EQUAL("up[1] mismatch", testdata[i].up[1], up[1], 0.00001);
		DOUBLES_EQUAL("up[2] mismatch", testdata[i].up[2], up[2], 0.00001);
	}
}

TEST(AngleMatrixTest, MathLib, 1000) {
	struct testdata_t {
		vec3_t angles;
		vec_t matrix0[4];
		vec_t matrix1[4];
		vec_t matrix2[4];
	};

	testdata_t testdata[2] = {
			{ { 21.0f, 65.0f, 162.0f }, { 0.39455f, 0.90875f, 0.13602f, 0.0f }, { 0.84611f, -0.30157f, -0.43949f, 0.0f }, { -0.35837f, 0.28849f, -0.88789f, 0.0f } },
			{ { 106.0f, 142.0f, 62.0f }, { 0.21721f, -0.95785f, 0.18798f, 0.0f }, { -0.16970f, 0.15259f, 0.97361f, 0.0f }, { -0.96126f, -0.24337f, -0.12940f, 0.0f } }
	};

	float rotation_matrix[3][4];

	for (int i = 0; i < ARRAYSIZE(testdata); i++) {
		AngleMatrix(testdata[i].angles, rotation_matrix);

		DOUBLES_EQUAL("rotationmatrix[0][0] mismatch", testdata[i].matrix0[0], rotation_matrix[0][0], 0.00001);
		DOUBLES_EQUAL("rotationmatrix[0][1] mismatch", testdata[i].matrix0[1], rotation_matrix[0][1], 0.00001);
		DOUBLES_EQUAL("rotationmatrix[0][2] mismatch", testdata[i].matrix0[2], rotation_matrix[0][2], 0.00001);
		DOUBLES_EQUAL("rotationmatrix[0][3] mismatch", testdata[i].matrix0[3], rotation_matrix[0][3], 0.00001);

		DOUBLES_EQUAL("rotationmatrix[1][0] mismatch", testdata[i].matrix1[0], rotation_matrix[1][0], 0.00001);
		DOUBLES_EQUAL("rotationmatrix[1][1] mismatch", testdata[i].matrix1[1], rotation_matrix[1][1], 0.00001);
		DOUBLES_EQUAL("rotationmatrix[1][2] mismatch", testdata[i].matrix1[2], rotation_matrix[1][2], 0.00001);
		DOUBLES_EQUAL("rotationmatrix[1][3] mismatch", testdata[i].matrix1[3], rotation_matrix[1][3], 0.00001);

		DOUBLES_EQUAL("rotationmatrix[2][0] mismatch", testdata[i].matrix2[0], rotation_matrix[2][0], 0.00001);
		DOUBLES_EQUAL("rotationmatrix[2][1] mismatch", testdata[i].matrix2[1], rotation_matrix[2][1], 0.00001);
		DOUBLES_EQUAL("rotationmatrix[2][2] mismatch", testdata[i].matrix2[2], rotation_matrix[2][2], 0.00001);
		DOUBLES_EQUAL("rotationmatrix[2][3] mismatch", testdata[i].matrix2[3], rotation_matrix[2][3], 0.00001);
	}
}

TEST(DotProductTest, MathLib, 1000) {
	Sys_CheckCpuInstructionsSupport();
	CHECK_WARNING_OUT("SSE4.1 Support", cpuinfo.sse4_1);

	struct testdata_t {
		vec3_t v1;
		vec3_t v2;
		float res;
	};

	testdata_t testdata[2] = {
			{ { 41.5f, 7.32f, -9.22f }, { 13.3f, -0.5f, 8.09f }, 473.70020f },
			{ { -16.1f, -0.09f, 1.2f }, { 8.2f, 1.2f, -6.66f }, -140.12000f },
	};

	for (int sse = 0; sse <= 1; sse++) {

		for (int i = 0; i < ARRAYSIZE(testdata); i++) {
			double res = _DotProduct(testdata[i].v1, testdata[i].v2);
			DOUBLES_EQUAL("_DotProduct mismatch", testdata[i].res, res, 0.0001);
		}

		cpuinfo.sse4_1 = 0;
	}
}

TEST(CrossProductTest, MathLib, 1000) {
	struct testdata_t {
		vec3_t v1;
		vec3_t v2;
		vec3_t res;
	};

	testdata_t testdata[2] = {
			{ { 41.5f, 7.32f, -9.22f }, { 13.3f, -0.5f, 8.09f }, { 54.60880f, -458.36102f, -118.10600f } },
			{ { -16.1f, -0.09f, 1.2f }, { 8.2f, 1.2f, -6.66f }, { -0.84060f, -97.38600f, -18.58200f } },
	};

	vec3_t res;

	for (int i = 0; i < ARRAYSIZE(testdata); i++) {
		CrossProduct(testdata[i].v1, testdata[i].v2, res);

		DOUBLES_EQUAL("CrossProduct[0] mismatch", testdata[i].res[0], res[0], 0.00001);
		DOUBLES_EQUAL("CrossProduct[1] mismatch", testdata[i].res[1], res[1], 0.00001);
		DOUBLES_EQUAL("CrossProduct[2] mismatch", testdata[i].res[2], res[2], 0.00001);
	}
}

TEST(LengthTest, MathLib, 1000) {
	Sys_CheckCpuInstructionsSupport();
	CHECK_WARNING_OUT("SSE4.1 Support", cpuinfo.sse4_1);

	struct testdata_t {
		vec3_t v;
		float l;
	};

	testdata_t testdata[2] = {
			{ { 41.5f, 7.32f, -9.22f }, 43.13746f },
			{ { -16.1f, -0.09f, 1.2f }, 16.14491f },
	};

	for (int sse = 0; sse <= 1; sse++) {
		for (int i = 0; i < ARRAYSIZE(testdata); i++) {
			double res = Length(testdata[i].v);

			DOUBLES_EQUAL("Length mismatch", testdata[i].l, res, 0.00001);
		}

		cpuinfo.sse4_1 = 0;
	}
}

TEST(Length2DTest, MathLib, 1000) {
	Sys_CheckCpuInstructionsSupport();
	CHECK_WARNING_OUT("SSE4.1 Support", cpuinfo.sse4_1);

	struct testdata_t {
		vec3_t v;
		float l;
	};

	testdata_t testdata[2] = {
			{ { 41.5f, 7.32f, -9.22f }, 42.14063f },
			{ { -16.1f, -0.09f, 1.2f }, 16.10025f },
	};

	for (int sse = 0; sse <= 1; sse++) {
		for (int i = 0; i < ARRAYSIZE(testdata); i++) {
			double res = Length2D(testdata[i].v);

			DOUBLES_EQUAL("Length mismatch", testdata[i].l, res, 0.00001);
		}

		cpuinfo.sse4_1 = 0;
	}
}

TEST(VectorNormalizeTest, MathLib, 1000) {
	Sys_CheckCpuInstructionsSupport();
	CHECK_WARNING_OUT("SSE4.1 Support", cpuinfo.sse4_1);

	struct testdata_t {
		vec3_t vecIn;
		vec3_t vecOut;
		float l;
	};

	testdata_t testdata[2] = {
			{ { 41.5f, 7.32f, -9.22f }, { 0.96204f, 0.16969f, -0.21374f }, 43.13746f },
			{ { -16.1f, -0.09f, 1.2f }, { -0.99722f, -0.00557f, 0.07433f }, 16.14491f },
	};

	for (int sse = 0; sse <= 1; sse++) {
		for (int i = 0; i < ARRAYSIZE(testdata); i++) {
			vec3_t v;
			memcpy(&v[0], &testdata[i].vecIn[0], sizeof(v));
			double res = VectorNormalize(v);

			DOUBLES_EQUAL("VectorNormalize.len mismatch", testdata[i].l, res, 0.00001);
			DOUBLES_EQUAL("VectorNormalize[0] mismatch", testdata[i].vecOut[0], v[0], 0.00001);
			DOUBLES_EQUAL("VectorNormalize[1] mismatch", testdata[i].vecOut[1], v[1], 0.00001);
			DOUBLES_EQUAL("VectorNormalize[2] mismatch", testdata[i].vecOut[2], v[2], 0.00001);
		}

		cpuinfo.sse4_1 = 0;
	}
}

TEST(VectorAnglesTest, MathLib, 1000) {
	Sys_CheckCpuInstructionsSupport();
	CHECK_WARNING_OUT("SSE4.1 Support", cpuinfo.sse4_1);

	struct testdata_t {
		vec3_t forward;
		vec3_t angles;
	};

	testdata_t testdata[2] = {
			{ { 0.96204f, 0.16969f, -0.21374f }, { 347.65839f, 10.00326f, 0.0f } },
			{ { -0.99722f, -0.00557f, 0.07433f }, { 4.26272f, 180.32002f, 0.0f } },
	};

	for (int sse = 0; sse <= 1; sse++) {
		for (int i = 0; i < ARRAYSIZE(testdata); i++) {
			vec3_t angles;
			VectorAngles(testdata[i].forward, angles);

			DOUBLES_EQUAL("VectorAngles[0] mismatch", testdata[i].angles[0], angles[0], 0.00001);
			DOUBLES_EQUAL("VectorAngles[1] mismatch", testdata[i].angles[1], angles[1], 0.00001);
			DOUBLES_EQUAL("VectorAngles[2] mismatch", testdata[i].angles[2], angles[2], 0.00001);
		}

		cpuinfo.sse4_1 = 0;
	}
}

TEST(VectorCompareTest, MathLib, 1000)
{
	struct testdata_t {
		vec4_t v1;
		vec4_t v2;
		int res;
	};

	testdata_t testdata[4] = {
		{{41.5f, 7.32f, -9.22f, -16.1f}, {41.5f, 7.32f, -9.22f, -16.1009f}, 1},
		{{41.5f, 7.32f, -9.22f, -16.1f}, {41.5f, 7.32f, -9.220002f, -16.1f}, 0},
		{{41.5f, 7.32f, -9.22f, -16.1f}, {41.49f, 7.32f, -9.22f, -16.1f}, 0},
		{{41.5f, 7.32001f, -9.22f, -16.1f}, {41.5f, 7.32f, -9.22f, -16.1f}, 0}
	};

	for (int i = 0; i < ARRAYSIZE(testdata); i++) {
		int res = VectorCompare(testdata[i].v1, testdata[i].v2);

		LONGS_EQUAL("VectorCompare mismatch", testdata[i].res, res);
	}
}

TEST(VectorMATest, MathLib, 1000) {
	struct testdata_t {
		vec3_t a;
		vec3_t m;
		float s;
		vec3_t res;
	};

	testdata_t testdata[2] = {
		{{41.5f, 7.32f, -9.22f}, {-16.1f, -0.09f, 1.2f}, 42.14063f, {-636.964111f, 3.527344f, 41.348755f}},
		{{0.96204f, 0.16969f, -0.21374f}, {347.65839f, 10.00326f, 0.0f}, 16.10025f, {5598.349121f, 161.224670f, -0.213740f}}
	};

	for (int i = 0; i < ARRAYSIZE(testdata); i++) {
		vec3_t out;
		VectorMA(testdata[i].a, testdata[i].s, testdata[i].m, out);

		DOUBLES_EQUAL("VectorMA[0] mismatch", testdata[i].res[0], out[0], 0.00001);
		DOUBLES_EQUAL("VectorMA[1] mismatch", testdata[i].res[1], out[1], 0.00001);
		DOUBLES_EQUAL("VectorMA[2] mismatch", testdata[i].res[2], out[2], 0.00001);
	}
}

TEST(R_ConcatTransformsTest, MathLib, 1000) {
	struct testdata_t {
		vec4_t in1[3];
		vec4_t in2[3];
		vec4_t res[3];
	};

	testdata_t testdata = {
		{{0.41f, 13.34f, 41.69f, 14.78f}, {34.67f, 15.00f, 7.24f, 43.58f}, {19.62f, 7.05f, 32.81f, 49.61f}},
		{{44.64f, 31.45f, 18.27f, 4.91f}, {29.95f, 48.27f, 23.91f, 39.02f}, {19.42f, 4.36f, 46.04f, 1.53f}},
		{{1227.455200f, 838.584717f, 2245.857666f, 601.105591f},	{2137.519531f, 1845.987793f, 1325.400513f, 810.186890f},	{1724.154541f, 1100.404175f, 2037.595459f, 471.234528f}}
	};

	vec4_t out[3];
	R_ConcatTransforms(testdata.in1, testdata.in2, out);

	DOUBLES_EQUAL("R_ConcatTransformsTest[0][0] mismatch", testdata.res[0][0], out[0][0], 0.001);
	DOUBLES_EQUAL("R_ConcatTransformsTest[0][1] mismatch", testdata.res[0][1], out[0][1], 0.001);
	DOUBLES_EQUAL("R_ConcatTransformsTest[0][2] mismatch", testdata.res[0][2], out[0][2], 0.001);
	DOUBLES_EQUAL("R_ConcatTransformsTest[0][3] mismatch", testdata.res[0][3], out[0][3], 0.001);

	DOUBLES_EQUAL("R_ConcatTransformsTest[1][0] mismatch", testdata.res[1][0], out[1][0], 0.001);
	DOUBLES_EQUAL("R_ConcatTransformsTest[1][1] mismatch", testdata.res[1][1], out[1][1], 0.001);
	DOUBLES_EQUAL("R_ConcatTransformsTest[1][2] mismatch", testdata.res[1][2], out[1][2], 0.001);
	DOUBLES_EQUAL("R_ConcatTransformsTest[1][3] mismatch", testdata.res[1][3], out[1][3], 0.001);

	DOUBLES_EQUAL("R_ConcatTransformsTest[2][0] mismatch", testdata.res[2][0], out[2][0], 0.001);
	DOUBLES_EQUAL("R_ConcatTransformsTest[2][1] mismatch", testdata.res[2][1], out[2][1], 0.001);
	DOUBLES_EQUAL("R_ConcatTransformsTest[2][2] mismatch", testdata.res[2][2], out[2][2], 0.001);
	DOUBLES_EQUAL("R_ConcatTransformsTest[2][3] mismatch", testdata.res[2][3], out[2][3], 0.001);
}
