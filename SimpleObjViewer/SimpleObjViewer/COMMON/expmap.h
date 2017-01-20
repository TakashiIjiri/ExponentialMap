#pragma once

#include "tmesh.h"


	
class ExpMapVtx
{
public:
	byte   flg   ; // 0:yet visited, 1:in Q, 2:fixed
	int    from  ; // vertex inded from which the path comes from (-1:startP, -2,yet)
	float  dist  ; // dist from start P
	EVec2f pos   ; // position in 2D normal coordinate

	ExpMapVtx()
	{
		flg  = 0;
		from = -2;
		dist = FLT_MAX;
		pos << 0,0;
	}

	void Set(byte _flg, int _from, float _dist)
	{
		flg  = _flg;
		from = _from;
		dist = _dist;
	}
	
	void Set(byte _flg, int _from, float _dist, EVec2f &_pos)
	{
		flg    = _flg;
		from   = _from;
		dist   = _dist;
		pos    = _pos ;
	}
};




/* ----------------------------------
const TMesh    &mesh    // surface mesh model
const EVec3f   &startP  // center point of exponential map
const int      &polyIdx // polygon idx of the mesh on which the startP exist
const float    &scaleC  // scaling coefficients for 2D to 3D 
vector<EVec2f> &expMap  //computed exponential map ([0,1]x[0,1]) startP = (0,5,0.5)
---------------------------------- */


void DijikstraMapping
(
	const TMesh       &mesh   ,
	const EVec3f      &startP ,
	const int         &polyIdx,
	vector<ExpMapVtx> &expMap

);




void expnentialMapping
(
	const TMesh       &mesh   ,
	const EVec3f      &startP ,
	const int         &polyIdx,
	vector<ExpMapVtx> &expMap

);
