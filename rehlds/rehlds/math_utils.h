#pragma once

#include "engine.h"

typedef float matrix33_t[3][3];

//ax + by + cz = h
struct equation3_t {
	double a, b, c, h;
};

enum equation_sys_resolve_res {
	EQSR_1,
	EQSR_NONE,
	EQSR_INF,
};

enum plane_itx_res {
	PIR_1,
	PIR_NONE,
	PIR_INF,
};

extern double CalculateDeterminant(matrix33_t &matrix);
extern equation_sys_resolve_res ResolveEq3Sys(equation3_t* eqs, double* res);
extern plane_itx_res CalcPlanesIntersection(mplane_t* p1, mplane_t* p2, mplane_t* p3, float* res);


