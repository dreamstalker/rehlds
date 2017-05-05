/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/

#include "precompiled.h"

int idum;

void SeedRandomNumberGenerator()
{
	idum = -(int)time(nullptr);
	if (idum > 1000)
	{
		idum = -idum;
	}
	else if (idum > -1000)
	{
		idum -= 22261048;
	}
}

#define IA 16807
#define IM 2147483647
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1 + (IM - 1) / NTAB)

int ran1()
{
	int j;
	long k;
	static long iy = 0;
	static long iv[NTAB];

	if (idum <= 0 || !iy)
	{
		if (-(idum) < 1)
			idum = 1;
		else
			idum = -(idum);

		for (j = NTAB + 7; j >= 0; j--)
		{
			k = (idum) / IQ;
			idum = IA * (idum - k * IQ) - IR * k;

			if (idum < 0)
				idum += IM;

			if (j < NTAB)
				iv[j] = idum;
		}
		iy = iv[0];
	}

	k = (idum) / IQ;
	idum = IA * (idum - k * IQ) - IR * k;
	if (idum < 0)
		idum += IM;

	j = iy / NDIV;
	iy = iv[j];
	iv[j] = idum;

	return iy;
}

#define AM (1.0 / IM)
#define EPS 1.2e-7
#define RNMX (1.0 - EPS)

float fran1()
{
	float temp = (float)AM * ran1();
	if (temp > RNMX) {
		return (float)RNMX;
	}
	else
		return temp;
}

float RandomFloat(float flLow, float flHigh)
{
	SeedRandomNumberGenerator();

	float fl = fran1(); // float in (0, 1)
	return (fl * (flHigh - flLow)) + flLow; // float in (low, high)
}

#define MAX_RANDOM_RANGE 0x7FFFFFFFUL

int RandomLong(int lLow, int lHigh)
{
	SeedRandomNumberGenerator();

	unsigned long maxAcceptable;
	unsigned long x = lHigh - lLow + 1;
	unsigned long n;
	if (x <= 0 || MAX_RANDOM_RANGE < x - 1)
	{
		return lLow;
	}

	// The following maps a uniform distribution on the interval [0,MAX_RANDOM_RANGE]
	// to a smaller, client-specified range of [0,x-1] in a way that doesn't bias
	// the uniform distribution unfavorably. Even for a worst case x, the loop is
	// guaranteed to be taken no more than half the time, so for that worst case x,
	// the average number of times through the loop is 2. For cases where x is
	// much smaller than MAX_RANDOM_RANGE, the average number of times through the
	// loop is very close to 1.
	maxAcceptable = MAX_RANDOM_RANGE - ((MAX_RANDOM_RANGE + 1) % x);
	do
	{
		n = ran1();
	}
	while (n > maxAcceptable);

	return lLow + (n % x);
}
