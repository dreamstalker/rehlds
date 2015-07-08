#include "precompiled.h"
#include "rehlds_tests_shared.h"
#include "cppunitlite/TestHarness.h"

TEST(CalculateDeterminant, MathUtils, 1000) {

	struct testdata_t {
		matrix33_t mtx;
		double d;
	};

	testdata_t testdata[] = {
			{
				{
					{ 2.0, 5.0, -2.0 },
					{ 3.0, 8.0, 0.0 },
					{ 1.0, 3.0, 5.0 },
				},
				3.0
			},
			{
				{
					{ 4.0, -3.0, 2.0 },
					{ 6.0, 11.0, 1.0 },
					{ 0.0, 3.0, 0.0 },
				},
				24.0
			}
	};

	for (int i = 0; i < ARRAYSIZE(testdata); i++) {
		testdata_t* t = &testdata[i];

		double dt = CalculateDeterminant(t->mtx);
		DOUBLES_EQUAL("Determinant mismatch", t->d, dt, 0.001);
	}
}

TEST(ResolveEq3Sys, MathUtils, 1000) {

	struct testdata_t {
		equation3_t eqs[3];
		equation_sys_resolve_res resKind;
		double res[3];
	};
	
	testdata_t testdata[] = {
		{
			{
				{ 3.0, 4.0, 2.0, 5.0 },
				{ 5.0, -6.0, -4.0, -3.0 },
				{ -4.0, 5.0, 3.0, 1.0 }
			},
			EQSR_1,
			{ 1.0, -2.0, 5.0 }
		},
		{
			{
				{ 1.0, 1.0, 1.0, 5.0 },
				{ 1.0, -1.0, 1.0, 1.0 },
				{ 1.0, 0.0, 1.0, 2.0 }
			},
			EQSR_NONE,
			{ 0.0, 0.0, 0.0 }
		},
		{
			{
				{ 1.0, 1.0, 1.0, 5.0 },
				{ 1.0, -1.0, 1.0, 1.0 },
				{ 1.0, 0.0, 1.0, 3.0 }
			},
			EQSR_INF,
			{ 0.0, 0.0, 0.0 }
		}
	};

	for (int i = 0; i < ARRAYSIZE(testdata); i++) {
		testdata_t* t = &testdata[i];

		double res[3];
		equation_sys_resolve_res resKind = ResolveEq3Sys(t->eqs, res);
		UINT32_EQUALS("resKind mismatch", (uint32)t->resKind, (uint32)resKind);

		if (resKind == EQSR_1) {
			DOUBLES_EQUAL("res[0] mismatch", t->res[0], res[0], 0.00001);
			DOUBLES_EQUAL("res[1] mismatch", t->res[1], res[1], 0.00001);
			DOUBLES_EQUAL("res[2] mismatch", t->res[2], res[2], 0.00001);
		}
	}

}