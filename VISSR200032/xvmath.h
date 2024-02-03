//#include "stdAfx.h"
#pragma once

struct XV2S;
struct XV2;
struct XV3;
struct XM3;

struct XV2S : public POINTS
	{
public:
	XV2S();
	XV2S(const int x,const int y);
	XV2S(LPARAM lP);
	XV2S(const V2& v2);
	XV2S operator+(const XV2S &b);
	void glVertex();
	void MoveTo(HDC hdc);
	void LineTo(HDC hdc);
	};

struct XV2
	{
	double v[2];
	XV2(const double x,const double y);
	XV2(const XV3& v3);
	double x() const;
	double y() const;
	XV2 operator-(); // unary
	XV2 operator-(XV2& other);
	void glVertex();
	void MoveTo(HDC hdc);
	void LineTo(HDC hdc);
	};

struct XV3
	{
	double v[3];
	XV3();
	XV3(const double x,const double y,const double z);
	XV3(const XV2S& v2);
	XV3(const V2& v2);
	double x() const;
	double y() const;
	XV3 operator*(XM3 &b);
	};

///can you subclass the dll?
struct XM3
	{
	double m[3][3];
	XM3();
	XM3 operator*(XM3 &b);
	};

struct XM3Trans : public XM3
	{
	XM3Trans(const double tx,const double ty);
	XM3Trans(const XV2& v2);
	XM3& xm3();
	};

struct XM3Scale : public XM3
	{
public:
	XM3Scale(double sx,double sy);
	};

struct XM3Shear : public XM3
	{
public:
	XM3Shear(double shx,double shy);
	};

struct XM3PerspX : public XM3
	{
public:
	XM3PerspX(double f);
	};

struct XM3PerspY : public XM3
	{
public:
	XM3PerspY(double f);
	};
