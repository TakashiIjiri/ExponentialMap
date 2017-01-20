#include "stdafx.h"

#include "OglForMFC.h"
#include "expmap.h"
#include "tqueue.h"

#include <map>



static Eigen::AngleAxisf calcRotV1toV2
(
	const EVec3f &v1, //should be normalized 
	const EVec3f &v2  //should be normalized
)
{
	EVec3f axis = v1.cross( v2 ).normalized();
	float theta = acos(v1.dot(v2));
	return Eigen::AngleAxisf( theta, axis );
}


static void Trace(const EVec3f p)
{
fprintf( stderr, "%f %f %f\n", p[0], p[1], p[2]);
}


static void Trace(const EVec2f p)
{
fprintf( stderr, "%f %f \n", p[0], p[1]);
}





//compute u_q in Tp
static EVec2f calcPosInNormalCoord
(
	const EVec3f &coordO,
	const EVec3f &coordN,
	const EVec3f &coordX,
	const EVec3f &coordY,
	const EVec3f &q
)
{
	EVec3f v   =  q - coordO;
	float len  = v.norm();
	EVec3f dir = (v - v.dot( coordN ) * coordN).normalized();

	v = len * dir; 

	return EVec2f( v.dot(coordX), v.dot(coordY));
}


void expnentialMapping
(
	const TMesh  &mesh,
	const EVec3f &startP,
	const int    &polyIdx,
	vector<ExpMapVtx> &expMap
)
{
	fprintf( stderr, "start...   \n");

	const EVec3f *verts = mesh.m_vVerts;
	const TPoly  *polys = mesh.m_pPolys;

	//initialization (flg:0, dist:Inf, from:-2)
	EVec3f Xdir(1,0,0);
	const EVec3f baseN  = mesh.m_pNorms[polyIdx];
	const EVec3f baseX  = (Xdir - Xdir.dot(baseN)*baseN).normalized(); //(verts[polys[polyIdx].idx[0]] - startP).normalized();
	const EVec3f baseY  = Eigen::AngleAxisf((float)M_PI * 0.5f, baseN ) * baseX;
	
	expMap.clear();
	expMap.resize(mesh.m_vSize);

	multimap<float,int> Q; 
	
	for (int i = 0; i < 3; ++i)
	{
		int vIdx = polys[polyIdx].idx[i];

		EVec2f u2D = calcPosInNormalCoord( startP, baseN, baseX, baseY, verts[vIdx]);
		float d = u2D.norm();

		Q.insert( make_pair(d, vIdx) );
		expMap[vIdx].Set( 1, -1, d, u2D);
	}




	//Dijikstra Growth
	while (!Q.empty())
	{
		//pivot vertex
		int   pivI = Q.begin()->second;
		Q.erase( Q.begin () );

		vector<int> &Nei = mesh.m_vRingVs[pivI];

		EVec3f localO  =  verts[pivI];
		EVec3f tmp     =  verts[Nei[0]] - localO;
		EVec3f localN  =  mesh.m_vNorms[pivI];
		EVec3f localX  =  (tmp - tmp.dot(localN) * localN ).normalized();
		EVec3f localY  =  Eigen::AngleAxisf((float)M_PI * 0.5f, localN ) * localX;
		expMap[pivI].flg = 2;

		//compute coordinate transformation
		Eigen::AngleAxisf localToBase = calcRotV1toV2( localN, baseN);
		//EVec3f localN_rot = localToBase * localN;
		EVec3f localX_rot = localToBase * localX;
		//EVec3f localY_rot = localToBase * localY;


		float theta = acos(localX_rot.dot(baseX) );
		if( localX_rot.cross( baseX ).dot( baseN ) < 0 ) theta *= -1;
		Eigen::Rotation2Df R2d( -theta );

		for (const auto& vi : Nei ) 
		{
			if( expMap[vi].flg == 2 ) continue;

			//Dijikstra法の距離は、graphのedge lengthを利用する実装（たぶん論文はこっちで書いてある。）
			float d = expMap[pivI].dist + (localO - verts[vi]).norm();

			//new coordinate on Tp
			EVec2f u2D = expMap[pivI].pos + R2d * calcPosInNormalCoord( localO, localN, localX, localY, verts[vi]);
		

			if(      expMap[vi].flg == 1 && d < expMap[vi].dist )
			{
				//objects in multimap may share the same key 
				multimap<float,int>::iterator it;
				for( it = Q.find(expMap[vi].dist); it != Q.end(); ++it) if( it->second == vi ) break;

				if (it == Q.end())
				{
					fprintf( stderr, "never comes here");
					expMap[vi].Set( 1, pivI, d, u2D);
				}
				else
				{
					//remove and re-add this neighbor vtx from&to Q
					Q.erase( it );
					expMap[vi].Set( 1, pivI, d, u2D);
					Q.insert( make_pair(d, vi) );
				}
			}
			else if (expMap[vi].flg  == 0)
			{
				expMap[vi].Set( 1, pivI, d, u2D);
				Q.insert( make_pair(d, vi) );
			}
		}
	}
	fprintf( stderr, "done...\n");
}




/*

void expnentialMapping
(
	const TMesh  &mesh,
	const EVec3f &startP,
	const int    &polyIdx,
	vector<ExpMapVtx> &expMap
)
{
	fprintf( stderr, "start...   \n");

	const EVec3f *verts = mesh.m_vVerts;
	const TPoly  *polys = mesh.m_pPolys;

	//initialization (flg:0, dist:Inf, from:-2)
	EVec3f Xdir(1,0,0);
	const EVec3f baseN  = mesh.m_pNorms[polyIdx];
	const EVec3f baseX  = (Xdir - Xdir.dot(baseN)*baseN).normalized(); //(verts[polys[polyIdx].idx[0]] - startP).normalized();
	const EVec3f baseY  = Eigen::AngleAxisf((float)M_PI * 0.5f, baseN ) * baseX;
	
	expMap.clear();
	expMap.resize(mesh.m_vSize);

	multimap<float,int> Q; 
	
	for (int i = 0; i < 3; ++i)
	{
		int vIdx = polys[polyIdx].idx[i];

		EVec2f u2D = calcPosInNormalCoord( startP, baseN, baseX, baseY, verts[vIdx]);
		float d = u2D.norm();

		Q.insert( make_pair(d, vIdx) );
		expMap[vIdx].Set( 1, -1, d, u2D);
	}


	DEBUG_count = 0;


	//Dijikstra Growth
	while (!Q.empty())
	{
		//pivot vertex
		int   pivI = Q.begin()->second;
		Q.erase( Q.begin () );

		vector<int> &Nei = mesh.m_vRingVs[pivI];

		EVec3f localO  =  verts[pivI];
		EVec3f tmp     =  verts[Nei[0]] - localO;
		EVec3f localN  =  mesh.m_vNorms[pivI];
		EVec3f localX  =  (tmp - tmp.dot(localN) * localN ).normalized();
		EVec3f localY  =  Eigen::AngleAxisf((float)M_PI * 0.5f, localN ) * localX;
		expMap[pivI].flg = 2;

		//compute coordinate transformation
		Eigen::AngleAxisf localToBase = calcRotV1toV2( localN, baseN);
		EVec3f localN_rot = localToBase * localN;
		EVec3f localX_rot = localToBase * localX;
		EVec3f localY_rot = localToBase * localY;


		float theta = acos(localX_rot.dot(baseX) );
		if( localX_rot.cross( baseX ).dot( baseN ) < 0 ) theta *= -1;
		//theta *= -1;
		Eigen::Rotation2Df R2d( -theta );

		for (const auto& vi : Nei ) 
		{
			if( expMap[vi].flg == 2 ) continue;
			EVec2f u2D = expMap[pivI].pos + R2d * calcPosInNormalCoord( localO, localN, localX, localY, verts[vi]);
			float d = u2D.norm();
						

			if(      expMap[vi].flg == 1 && d < expMap[vi].dist )
			{
				//objects in multimap may share the same key 
				multimap<float,int>::iterator it;
				for( it = Q.find(expMap[vi].dist); it != Q.end(); ++it) if( it->second == vi ) break;

				if (it == Q.end())
				{
					fprintf( stderr, "never comes here");
					expMap[vi].Set( 1, pivI, d, u2D);
				}
				else
				{
					//remove and re-add this neighbor vtx from&to Q
					Q.erase( it );
					expMap[vi].Set( 1, pivI, d, u2D);
					Q.insert( make_pair(d, vi) );
				}
			}
			else if (expMap[vi].flg  == 0)
			{
				expMap[vi].Set( 1, pivI, d, u2D);
				Q.insert( make_pair(d, vi) );
			}
		}
	}

	fprintf( stderr, "done...\n");

}


*/




void DijikstraMapping
(
	const TMesh  &mesh,
	const EVec3f &startP,
	const int    &polyIdx,
	vector<ExpMapVtx> &expMap
)
{
	fprintf( stderr, "start...\n");

	const int vSize = mesh.m_vSize;

	expMap.clear();
	expMap.resize(vSize);

	multimap<float,int> Q; //(dist from startP, vertex idx)

	//initialization 

	for (int i = 0; i < 3; ++i)
	{
		int vIdx = mesh.m_pPolys[polyIdx].idx[i];

		float d = (startP - mesh.m_vVerts[vIdx]).norm();
		expMap[vIdx].Set( 1, -1, d);
		Q.insert( make_pair(d, vIdx) );
	}


	while (!Q.empty())
	{
		int   pivI = Q.begin()->second;
		
		//fix piv
		Q.erase( Q.begin () );
		expMap[pivI].flg = 2;


		for (const auto& vi : mesh.m_vRingVs[pivI]) 
		{
			if( expMap[vi].flg == 2 ) continue;

			float d = (mesh.m_vVerts[vi] - mesh.m_vVerts[pivI]).norm() + expMap[pivI].dist;

			if(      expMap[vi].flg == 1 && d < expMap[vi].dist )
			{
				//objects in multimap may share the same key 
				multimap<float,int>::iterator it;
				for( it = Q.find(expMap[vi].dist); it != Q.end(); ++it) if( it->second == vi ) break;

				if (it == Q.end())
				{
					fprintf( stderr, "never comes here");
					expMap[vi].Set( 1, pivI, d);
				}
				else
				{
					//remove and re-add this neighbor vtx from&to Q
					Q.erase( it );
					expMap[vi].Set( 1, pivI, d);
					Q.insert( make_pair(d, vi) );
				}
			}
			else if (expMap[vi].flg  == 0)
			{
				expMap[vi].Set( 1, pivI, d);
				Q.insert( make_pair(d, vi) );
			}
		}
	}

	fprintf( stderr, "done...\n");

}



/*

	{
		//map test 
		multimap<int,int> Q;
		Q.insert( make_pair(0,0));
		Q.insert( make_pair(0,1));
		Q.insert( make_pair(0,2));
		Q.insert( make_pair(1,3));
		Q.insert( make_pair(1,4));
		Q.insert( make_pair(2,5));
		Q.insert( make_pair(2,6));
		Q.insert( make_pair(0,7));
		Q.insert( make_pair(0,8));
		Q.insert( make_pair(0,9));
	
		auto it = Q.find(1);
		while( it != Q.end() )
		{
			fprintf( stderr, "AAA %d %d\n", it->first, it->second);
			++it;
		}

		for (auto it : Q)
		{
			fprintf( stderr, "BBB %d %d\n", it.first, it.second);
		}
	}



*/