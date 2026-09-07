/*
*  fmap.h
*
*  Copyright (c) 2016 by Avans Hogeschool.
* 
*  Date:    28-may-2016
*  Author:  Roel Smeets
*  Revision: V2.0
*  Modified: 07-09-2026
*/

#ifndef _MAP_H
#define _MAP_H


///////////////////////////////////////////////////////////////////////////////
// function prototypes

long int map(long int x, long int in_min,  long int in_max, 
			 			 long int out_min, long int out_max);

float fmap(float x, float in_min, float in_max, float out_min, float out_max);


#endif  // _MAP_H
