/*
 Copyright (C) 2005 Louis Bavoil <bavoil@cs.utah.edu>                 

 This program is free software; you can redistribute it and/or        
 modify it under the terms of the GNU Library General Public License  
 as published by the Free Software Foundation; either version 2       
 of the License, or (at your option) any later version.               

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of 
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 GNU Library General Public License for more details.
*/

#include "math.h"

void normalize(float v[3])
{
	float l = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
	l = 1 / (float)sqrt(l);

	v[0] *= l;
	v[1] *= l;
	v[2] *= l;
}

void crossproduct(float a[3], float b[3], float res[3])
{
	res[0] = (a[1] * b[2] - a[2] * b[1]);
	res[1] = (a[2] * b[0] - a[0] * b[2]);
	res[2] = (a[0] * b[1] - a[1] * b[0]);
}

float dotproduct(float a[3], float b[3])
{
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

float length(float v[3])
{
	return (float)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}
