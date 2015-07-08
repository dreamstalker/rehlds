#include "precompiled.h"

double CalculateDeterminant(matrix33_t &matrix) {
	return
		matrix[0][0] * matrix[1][1] * matrix[2][2]
		- matrix[0][0] * matrix[1][2] * matrix[2][1]
		+ matrix[1][0] * matrix[2][1] * matrix[0][2]
		- matrix[1][0] * matrix[2][2] * matrix[0][1]
		+ matrix[2][0] * matrix[0][1] * matrix[1][2]
		- matrix[2][0] * matrix[0][2] * matrix[1][1];
}

equation_sys_resolve_res ResolveEq3Sys(equation3_t* eqs, double* res) {
	matrix33_t mainDtMtx = {
		{ eqs[0].a, eqs[0].b, eqs[0].c },
		{ eqs[1].a, eqs[1].b, eqs[1].c },
		{ eqs[2].a, eqs[2].b, eqs[2].c }
	};

	matrix33_t dtxMtx = {
		{ eqs[0].h, eqs[0].b, eqs[0].c },
		{ eqs[1].h, eqs[1].b, eqs[1].c },
		{ eqs[2].h, eqs[2].b, eqs[2].c }
	};

	matrix33_t dtyMtx = {
		{ eqs[0].a, eqs[0].h, eqs[0].c },
		{ eqs[1].a, eqs[1].h, eqs[1].c },
		{ eqs[2].a, eqs[2].h, eqs[2].c }
	};

	matrix33_t dtzMtx = {
		{ eqs[0].a, eqs[0].b, eqs[0].h },
		{ eqs[1].a, eqs[1].b, eqs[1].h },
		{ eqs[2].a, eqs[2].b, eqs[2].h }
	};

	double mainDt = CalculateDeterminant(mainDtMtx);
	double dtx = CalculateDeterminant(dtxMtx);
	double dty = CalculateDeterminant(dtyMtx);
	double dtz = CalculateDeterminant(dtzMtx);

	if (abs(mainDt) > 0.000001) {
		res[0] = dtx / mainDt;
		res[1] = dty / mainDt;
		res[2] = dtz / mainDt;

		return EQSR_1;
	}

	return (abs(dtx) > 0.000001) ? EQSR_NONE : EQSR_INF;
}

plane_itx_res CalcPlanesIntersection(mplane_t* p1, mplane_t* p2, mplane_t* p3, float* res) {
	equation3_t eqs[] = {
		{ p1->normal[0], p1->normal[1], p1->normal[2], p1->dist },
		{ p2->normal[0], p2->normal[1], p2->normal[2], p2->dist },
		{ p3->normal[0], p3->normal[1], p3->normal[2], p3->dist },
	};

	double dblRes[3];

	auto resKind = ResolveEq3Sys(eqs, dblRes);
	switch (resKind) {
	case EQSR_1:
		res[0] = dblRes[0];
		res[1] = dblRes[1];
		res[2] = dblRes[2];
		return PIR_1;

	case EQSR_INF:
		return PIR_INF;

	case EQSR_NONE:
		return PIR_NONE;

	default:
		rehlds_syserror(__FUNCTION__": invalid resKind %d", resKind);
		return PIR_NONE;
	}
}
