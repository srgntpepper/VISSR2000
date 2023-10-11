#include "stdAfx.h"


/*******************************************************************************
	XV2S 2d integer vectors (extent Windows POINTS)
*******************************************************************************/
XV2S::XV2S()
	{
	x=0;y=0;
	}

XV2S::XV2S(const int x,const int y)
	{
	this->x=x;
	this->y=y;
	}

XV2S::XV2S(LPARAM lP) // MAKEPOINTS like conversion
	{
	x=((POINTS*)&(lP))->x;
	y=((POINTS*)&(lP))->y;
	}

XV2S::XV2S(const V2& v2)
	{
	x=(int)v2.v2[0];
	y=(int)v2.v2[1];
	}

XV2S XV2S::operator+(const XV2S &b)
	{
	return XV2S(x+b.x,y+b.y);
	}

void XV2S::glVertex()
	{
	glVertex2sv(&x);
	}

void XV2S::MoveTo(HDC hdc)
	{
	MoveToEx(hdc,x,y,NULL);
	}

void XV2S::LineTo(HDC hdc)
	{
	::LineTo(hdc,x,y);
	}

/*******************************************************************************
	XV2 2d vectors
*******************************************************************************/
XV2::XV2(const double x,const double y)
	{
	v[0]=x;
	v[1]=y;
	}

XV2::XV2(const XV3& v3)
	{
	v[0]=v3.x();
	v[1]=v3.y();
	}

double XV2::x() const {return v[0];}
double XV2::y() const {return v[1];}

XV2 XV2::operator-()
	{
	return XV2(-v[0],-v[1]); // unary
	}

XV2 XV2::operator-(XV2 &b)
	{
	return XV2(v[0]-b.x(),v[1]-b.y());
	}

void XV2::glVertex()
	{
	glVertex2dv(v);
	}
void XV2::MoveTo(HDC hdc)
	{
	MoveToEx(hdc,(int)v[0],(int)v[1],NULL);
	}
void XV2::LineTo(HDC hdc)
	{
	::LineTo(hdc,(int)v[0],(int)v[1]);
	}

XV3::XV3()
	{
	v[0]=0.0;
	v[1]=0.0;
	v[2]=0.0;
	}

XV3::XV3(const double x,const double y,const double z)
	{
	v[0]=x;
	v[1]=y;
	v[2]=z;
	}

XV3::XV3(const XV2S& v2s)
	{
	/// extend with 1 as homoginized
	v[0]=v2s.x;
	v[1]=v2s.y;
	v[2]=1.0;
	}

XV3::XV3(const V2& v2)
	{
	/// extend with 1 as homoginized
	v[0]=v2.v2[0];
	v[1]=v2.v2[1];
	v[2]=1.0;
	}

double XV3::x() const {return v[0]/v[2];}
double XV3::y() const {return v[1]/v[2];}

XV3 XV3::operator*(XM3 &b)
	{
	XV3 ans;
	int c,i;
	double acc;
	for(c=0;c<3;c++)
		{
		acc=0.0;
		for(i=0;i<3;i++)
			acc+=v[i]*b.m[i][c];
		ans.v[c]=acc;
		}
	return ans;
	}

/*******************************************************************************
	XM3 3x3 Matrix
*******************************************************************************/
XM3::XM3()
	{
	for(int r=0;r<3;r++)
		for(int c=0;c<3;c++)
			m[r][c]=r==c?1.0:0.0;
	}

XM3 XM3::operator*(XM3 &b)
	{
	XM3 ans;
	int r,c,i;
	double acc;
	for(r=0;r<3;r++)
		{
		for(c=0;c<3;c++)
			{
			acc=0.0;
			for(i=0;i<3;i++)
				acc+=m[r][i]*b.m[i][c];
			ans.m[r][c]=acc;
			}
		}
	return ans;
	}

XM3Trans::XM3Trans(const double tx,const double ty)
	{
	m[2][0]=tx;
	m[2][1]=ty;
	}

XM3Trans::XM3Trans(const XV2& v2)
	{
	m[2][0]=v2.x();
	m[2][1]=v2.y();
	}

XM3& XM3Trans::xm3() {
	return *this;
}

XM3Scale::XM3Scale(double sx,double sy)
	{
	m[0][0]=sx;
	m[1][1]=sy;
	}

XM3Shear::XM3Shear(double shx,double shy)
	{
	m[0][1]=shy;
	m[1][0]=shx;
	};

XM3PerspX::XM3PerspX(double f)
	{
	m[0][2]=1.0/f;
	};

XM3PerspY::XM3PerspY(double f)
	{
	m[1][2]=1.0/f;
	};
