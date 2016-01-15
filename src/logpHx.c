/*******************************************************************************
lohpHx

Takes arguments r,x, and EPSL:
    r: The autocorrelation vector of size nn x 1 for a normal stochastic process.
    x: A column vector of size nnx x 1.
    EPSL: Used to define precision in control statements.

Changes vector y:
    y[0]: Equals to -0.5*log[det(R)] -
    - 0.5*(nnx-1)*log{[t(ones)*inv(R)*ones] * [t(x)*inv(R)*x] -
    [t(ones)*inv(R)*x]^2} + (0.5*nnx-1)*log[t(ones)*inv(R)*ones]
    where R is the autocorrelation matrix for a normal stochastic process,ones a
    column vector of size nnx x 1,with all elements equal to 1, and t() denotes
    the transpose of a matrix.
    y[1]: Determinant of the Toeplitz autocorrelation matrix nn x nn produced by
    a normal stochastic process.
    y[2]: Equals to [t(ones)*inv(R)*x]/[t(ones)*inv(R)*ones].
    y[3]: Equals to 1/[t(ones)*inv(R)*ones].
    y[4]: Equals to 0.5 *{[t(ones)*inv(R)*ones]*[t(x)*inv(R)*x]-
    [t(ones)*inv(R)*x]^2}/[t(ones)*inv(R)*ones].
    t(.): denotes the transpose of a matrix.
    inv(.): denotes the inverse of a matrix.

Possible values of fault and corresponding fault conditions are:
    0 The program is normally performed
    1 Error ("Singular Matrix")
    2 Error ("Input r[0] is not equal to 1.")
    3 Error ("The length(r) is not equal to the length(x))"

Uses the lev.c and levDet.c functions
*******************************************************************************/

#include "trenchR.h"

void logpHx(double *r,int *nn,
	 double *x,int *nnx,double *EPSL,double *y,int *fault)
{
	VECTOR y1, y2, e1, e2, e3;
	double EPS, s1, s2, s3;
	int n = *nn, nx = *nnx, i, k, _fault1;
if (n != nx)
	{
		for (i = 0; i < 2; i++)
			y[i] = 0.0;
		fault[0] = 3;//The length(r) is not equal to the length(x)
		return;
	}
	EPS = *EPSL;
	y1 = Vector(n);
	e1 = Vector(n-1);
	_fault1 = lev(r,n,x,y1,e1,EPS);// y1 solution of the system R * y1 = x
if (_fault1 != 0)
	{
		for (i = 0; i < 2; i++)
			y[i] = 0.0;
		fault[0] = _fault1;
		free_vector(y1);
		free_vector(e1);
		return;
	}
else
    fault[0] = 0;// The program is normally performed
    y2 = Vector(n);
    e2 = Vector(n-1);
	e3 = Vector(n);
	// e3: vector of size n x 1 with all elements equal to 1.
    for (k = 0; k < n; k++)
			e3[k] = 1.0;
    lev(r,n,e3,y2,e2,EPS);// y2 solution of the system R * y2 = e3
    y[1] = levDet(n-1,e2);// Computation of the logarithm of the determinant.
    s1 = sum(n,y2);// s1 equals to t(e3) * inv(R) * e3.
    s2 = sum(n,y1);// s2 equals to t(e3) * inv(R) * x.
    s3 = dot(n,x,y1);// s3 equals to t(x) * inv(R) * x.
	y[0] = -0.5 * y[1]- 0.5 * (n-1.0)*log(s1 * s3 - s2*s2) + (0.5*n-1.0) * log(s1);
	y[2] = s2/s1;
	y[3] = 1/s1;
	y[4] = 0.5 * (s3 - y[3]* s2* s2);
	free_vector(y1);
	free_vector(y2);
	free_vector(e1);
	free_vector(e2);
    free_vector(e3);
}
