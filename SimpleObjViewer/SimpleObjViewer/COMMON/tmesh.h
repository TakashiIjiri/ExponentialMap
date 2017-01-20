#pragma once

#include <list>
#include <vector>
#include "tmath.h"
using namespace std;


#pragma warning(disable : 4996)

class TPoly
{
public:
	int idx[3];

	TPoly(int v0=0, int v1=0, int v2=0){
		idx[0] = v0; idx[1] = v1; idx[2] = v2;
	}
	TPoly(const TPoly &p ){ Set(p); }

	void Set( const TPoly &p){
		memcpy( idx, p.idx, sizeof( int ) * 3);
	}
	TPoly& operator= (const TPoly  &v){ Set(v); return *this; }
};






// very simple mesh representation 
// each vertex has 
// - position
// - TexCd
// - Normal 
// - one ring info 

class TMesh
{

public:
	//Vertex Info
	int          m_vSize  ;
	EVec3f      *m_vVerts ;
	EVec3f      *m_vTexCd ; //(u,v,w) 
	EVec3f      *m_vNorms ;
	vector<int> *m_vRingPs;
	vector<int> *m_vRingVs;

	//Polygon Info
	int          m_pSize  ;
	EVec3f      *m_pNorms ;
	TPoly       *m_pPolys ;

	TMesh()
	{
		m_vSize   = 0;
		m_vVerts  = 0;
		m_vNorms  = 0;
		m_vTexCd  = 0;
		m_vRingPs = 0;
		m_vRingVs = 0;
		m_pSize   = 0;
		m_pNorms  = 0;
		m_pPolys  = 0;
	}

	~TMesh()
	{
		clear();
	}

	void clear()
	{
		if ( m_vVerts != 0) delete[] m_vVerts;
		if ( m_vNorms != 0) delete[] m_vNorms;
		if ( m_vTexCd != 0) delete[] m_vTexCd;
		if ( m_pNorms != 0) delete[] m_pNorms;
		if ( m_pPolys != 0) delete[] m_pPolys;
		if ( m_vRingPs!= 0) delete[] m_vRingPs;
		if ( m_vRingVs!= 0) delete[] m_vRingVs;
		m_vSize   = 0;
		m_vVerts  = 0;
		m_vNorms  = 0;
		m_vTexCd  = 0;
		m_pSize   = 0;
		m_pNorms  = 0;
		m_pPolys  = 0;
		m_vRingPs = 0;
		m_vRingVs = 0;
	}


	void Set( const TMesh &v)
	{
		clear();
		m_vSize = v.m_vSize;
		m_pSize = v.m_pSize;

		if( m_vSize != 0 ) 
		{
			m_vVerts  = new EVec3f[ m_vSize ];
			m_vNorms  = new EVec3f[ m_vSize ];
			m_vTexCd  = new EVec3f[ m_vSize ];
			memcpy( m_vVerts, v.m_vVerts, sizeof(EVec3f) * m_vSize) ;
			memcpy( m_vNorms, v.m_vNorms, sizeof(EVec3f) * m_vSize) ;
			memcpy( m_vTexCd, v.m_vTexCd, sizeof(EVec3f) * m_vSize) ;

			m_vRingVs = new vector<int>[m_vSize];
			m_vRingPs = new vector<int>[m_vSize];
			for (int i = 0; i < m_vSize; ++i)
			{
				for (const auto &k : m_vRingVs[i]) m_vRingVs[i].push_back(k);
				for (const auto &k : m_vRingPs[i]) m_vRingPs[i].push_back(k);
			}
		}


		if( m_pSize != 0 )
		{
			m_pNorms  = new EVec3f[ m_pSize ];
			m_pPolys  = new TPoly [ m_pSize ];
			memcpy( m_pNorms , v.m_pNorms , sizeof(EVec3f) * m_pSize) ;
			memcpy( m_pPolys , v.m_pPolys , sizeof(TPoly ) * m_pSize) ;
		}
	}

	TMesh(const TMesh& src)
	{
		m_vSize   = 0;
		m_vVerts  = 0;
		m_vNorms  = 0;
		m_vTexCd  = 0;
		m_vRingPs = 0;
		m_vRingVs = 0;
		m_pSize   = 0;
		m_pNorms  = 0;
		m_pPolys  = 0;
		Set(src);
	}
	
	TMesh& operator=(const TMesh& src)
	{
		m_vSize   = 0;
		m_vVerts  = 0;
		m_vNorms  = 0;
		m_vTexCd  = 0;
		m_vRingPs = 0;
		m_vRingVs = 0;
		m_pSize   = 0;
		m_pNorms  = 0;
		m_pPolys  = 0;
		Set(src);
		return *this;
	}



	bool initialize(const char *fName)
	{	

		FILE* fp = fopen(fName,"r") ;
		if( !fp ) return false;

		list<EVec3f>  vList;
		list<EVec2f>  uvList;
		list<TPoly >  pList  ;
		list<TPoly >  pUvList;

		char buf[512] ;		
		while( fgets(buf,255,fp) )
		{
			char* bkup  = _strdup( buf        );
			char* token =  strtok( buf, " \t" );

			if( stricmp( token,"vt" ) == 0 )
			{ 
				double u,v;
				sscanf( bkup,"vt %lf %lf", &u, &v);
				uvList.push_back( EVec2f((float)u, (float)v) );
			} 
			else if( stricmp( token,"v" ) == 0 )
			{ 
				double x,y,z;
				sscanf( bkup,"v %lf %lf %lf",&x,&y,&z ) ;
				vList.push_back( EVec3f( (float)x, (float)y, (float)z ) );
			} 
			else if( stricmp( token,"f" ) == 0 )
			{
				//sscanfの返り値は正常に読めた数: / が入ったら2文字しか読めない

				int v[3], t[3], s;
				int vtxnum = sscanf( bkup,"f %d %d %d %d", &v[0], &v[1], &v[2], &s) ;
				if( vtxnum < 3 ) vtxnum = sscanf( bkup,"f %d/%d %d/%d %d/%d %d/%d" ,            &v[0], &t[0],    &v[1], &t[1],     &v[2], &t[2], &s, &s)/2 ;
				if( vtxnum < 3 ) vtxnum = sscanf( bkup,"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &v[0], &t[0],&s, &v[1], &t[1], &s, &v[2], &t[2], &s, &s, &s, &s)/3 ;
				if( vtxnum < 3 ) vtxnum = sscanf( bkup,"f %d//%d %d//%d %d//%d %d//%d" ,        &v[0], &s      , &v[1], &s       , &v[2]   , &s, &s, &s)/2 ;
				pList  .push_back( TPoly( v[0]-1, v[1]-1, v[2]-1 ) ) ;
				pUvList.push_back( TPoly( t[0]-1, t[1]-1, t[2]-1 ) ) ;

			}
			free(bkup) ;
		}
		fclose(fp) ;
 

		vector<EVec3f> Vs { std::begin(  vList), std::end(  vList) };
		vector<EVec2f> Ts { std::begin( uvList), std::end( uvList) };
		vector<TPoly>  Ps { std::begin(  pList), std::end(  pList) };
		vector<TPoly>  Puv{ std::begin(pUvList), std::end(pUvList) };

		initialize( Vs, Ps );


		//1頂点につき1 texCdのときのみ，入力されたTexCdを利用
		if( Ts.size() == Vs.size() && Ps.size() == Puv.size() && isSame( Ps, Puv ) )
		{
			for ( int i = 0; i < m_vSize; ++i ) m_vTexCd[i] << Ts[i][0], Ts[i][1], 0;	
		}

		fprintf(stderr, "loaded object file info : %d %d \n", m_vSize, m_pSize);
		return true;
	}



private:
	bool isSame(  const vector<TPoly> &Ps, const vector<TPoly> &Puv )
	{
		if( Ps.size() != Puv.size() ) return false;
		for( int i=0; i < (int) Ps.size(); ++i)
		{
			if( Ps[i].idx[0] != Puv[i].idx[0] || 
				Ps[i].idx[1] != Puv[i].idx[1] || 
				Ps[i].idx[2] != Puv[i].idx[2] ) return false;
		}
		return true;
	}


public:
	void initialize( const vector<EVec3f> &Vs, const vector<TPoly> &Ps )
	{
		clear();
		
		m_vSize = (int)Vs.size();
		if( m_vSize != 0 )
		{
			m_vVerts  = new EVec3f[m_vSize];
			m_vNorms  = new EVec3f[m_vSize];
			m_vTexCd  = new EVec3f[m_vSize];
			m_vRingVs = new vector<int>[m_vSize];
			m_vRingPs = new vector<int>[m_vSize];
			for ( int i = 0; i < m_vSize; ++i ) m_vVerts[i] = Vs[i];
		}

		m_pSize   = (int)Ps.size();
		if (m_pSize != 0)
		{
			m_pPolys  = new TPoly [m_pSize];
			m_pNorms  = new EVec3f[m_pSize];
			for ( int i = 0; i < m_pSize; ++i ) m_pPolys [i] = Ps [i];
		}

		updateNormal();
		updateRingInfo();


		//for debug
		fprintf( stderr, "check data\n");
		for (int i = 0; i < m_pSize; ++i)
		{
			int *idx = m_pPolys[i].idx;
			if( idx[0] < 0 || idx[0] >= m_vSize ) fprintf( stderr, "aaaaaaa");
			if( idx[1] < 0 || idx[1] >= m_vSize ) fprintf( stderr, "bbbbbb");
			if( idx[2] < 0 || idx[2] >= m_vSize ) fprintf( stderr, "cccccc");

		}

	}



	void smoothing(int n)
	{
		EVec3f *vs = new EVec3f[m_vSize];
		
		for (int k = 0; k < n; ++k)
		{
#pragma omp parallel for
			for( int i=0; i < m_vSize; ++i)
			{
				vs[i] << 0,0,0;
				for( const auto &it : m_vRingVs[i] ) vs[i] += m_vVerts[it];
				vs[i] /= (float)m_vRingVs[i].size();
			}
			swap( vs, m_vVerts );

		}
		delete[] vs;
		updateNormal();
	}



	void updateNormal()
	{
#pragma omp parallel for
		for( int i=0; i < m_vSize; ++i) m_vNorms[i].setZero();

		for( int i=0; i < m_pSize; ++i)
		{
			int *idx = m_pPolys[i].idx;
			m_pNorms[i] = ( m_vVerts[ idx[1] ]- m_vVerts[ idx[0] ]).cross( m_vVerts[idx[2]] - m_vVerts[idx[0]] ).normalized();

			m_vNorms[ idx[0] ] += m_pNorms[i];
			m_vNorms[ idx[1] ] += m_pNorms[i];
			m_vNorms[ idx[2] ] += m_pNorms[i];
		}

#pragma omp parallel for
		for(int i=0; i<m_vSize; ++i) m_vNorms[i].normalize();
	}



	void updateRingInfo()
	{
		for (int i = 0; i < m_vSize; ++i) m_vRingPs[i].clear();
		for (int i = 0; i < m_vSize; ++i) m_vRingVs[i].clear();

		for( int i =0; i < m_pSize; ++i)
		{
			int *idx = m_pPolys[i].idx;
			m_vRingPs[ idx[0] ].push_back(i);
			m_vRingPs[ idx[1] ].push_back(i);
			m_vRingPs[ idx[2] ].push_back(i);
			m_vRingVs[ idx[0] ].push_back(idx[1]);
			m_vRingVs[ idx[0] ].push_back(idx[2]);
			m_vRingVs[ idx[1] ].push_back(idx[0]);
			m_vRingVs[ idx[1] ].push_back(idx[2]);
			m_vRingVs[ idx[2] ].push_back(idx[0]);
			m_vRingVs[ idx[2] ].push_back(idx[1]);
		}

 		for (int i = 0; i < m_vSize; ++i) 
		{
			sort  (m_vRingVs[i].begin(), m_vRingVs[i].end());
			auto it = unique(m_vRingVs[i].begin(), m_vRingVs[i].end());
			m_vRingVs[i].erase( it, m_vRingVs[i].end());
		}
	}
		
	void Translate(const EVec3f t         ) { for( int i=0; i < m_vSize; ++i ) m_vVerts[i] += t;			  }
	void Scale    (const float  s         ) { for( int i=0; i < m_vSize; ++i ) m_vVerts[i] *= s;			  }
	void Rotate(Eigen::AngleAxis<float> &R) { for( int i=0; i < m_vSize; ++i ) m_vVerts[i] = R * m_vVerts[i]; }



	EVec3f getGravityCenter() const
	{
		EVec3f p;
		for( int i=0; i < m_vSize; ++i ) p += m_vVerts[i];
		return p / (float)m_vSize;
	}



	void getBoundBox(EVec3f &minV, EVec3f &maxV) const 
	{
		minV << FLT_MAX, FLT_MAX, FLT_MAX;
		maxV <<-FLT_MAX,-FLT_MAX,-FLT_MAX;
		for( int i=0; i < m_vSize; ++i )
		{
			minV[0] = min( minV[0], m_vVerts[i][0]);
			minV[1] = min( minV[1], m_vVerts[i][1]);
			minV[2] = min( minV[2], m_vVerts[i][2]);
			maxV[0] = max( maxV[0], m_vVerts[i][0]);
			maxV[1] = max( maxV[1], m_vVerts[i][1]);
			maxV[2] = max( maxV[2], m_vVerts[i][2]);
		}
	}



	void normalizeByUniformScaling()
	{
		EVec3f minV, maxV;
		getBoundBox(minV, maxV);
		EVec3f a = maxV - minV;
		float s = max( a[0], max(a[1], a[2]) );

		Translate( -minV );
		Scale( 1.0f/s );
	}


	void checkError() const
	{
		/*
		for (int i = 0; i < m_pSize; ++i)
		{
			int *idx = m_pPolys[i].idx;
			if( idx[0] < 0 || m_vSize <= idx[0] ) fprintf( stderr, "er1");
			if( idx[1] < 0 || m_vSize <= idx[1] ) fprintf( stderr, "er2");
			if( idx[2] < 0 || m_vSize <= idx[2] ) fprintf( stderr, "er3");
		}

		fprintf( stderr, "ch--");
		GLenum errcode=glGetError();
		if(errcode!=GL_NO_ERROR)
		{
			const GLubyte *errstring=gluErrorString(errcode);
			fprintf(stderr, "aaaaaa %d %s\n",errcode, errstring);
		}
		*/
	}

	void draw() const 
	{
			checkError();



		if( m_vSize == 0 || m_pSize == 0 ) return;

		glEnableClientState(GL_VERTEX_ARRAY       );
		glEnableClientState(GL_NORMAL_ARRAY       );
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glNormalPointer  (   GL_FLOAT , 0, m_vNorms );
		checkError();
		glTexCoordPointer(3, GL_FLOAT , 0, m_vTexCd );
		checkError();
		glVertexPointer  (3, GL_FLOAT , 0, m_vVerts );
		checkError();
		glDrawElements   (GL_TRIANGLES, m_pSize * 3, GL_UNSIGNED_INT, m_pPolys);
		checkError();
		glDisableClientState(GL_VERTEX_ARRAY       );
		glDisableClientState(GL_NORMAL_ARRAY       );
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		checkError();
	}



	//only for surface where each vertex has unique texture coordinate
	void draw(const float *diff, const float *ambi, const float *spec, const float *shin) const
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , spec);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE  , diff);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT  , ambi);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shin);
		draw();
	}



	void exportObjNoTexCd(const char *fname)
	{	
		FILE* fp = fopen(fname, "w") ;
		if( !fp ) return;

		fprintf(fp,"#Obj exported from tmesh\n") ;
		
		for( int i=0;i<m_vSize; ++i )
		{
			fprintf(fp,"v %f %f %f\n", m_vVerts[i][0], m_vVerts[i][1]  , m_vVerts[i][2]) ;
		}

		for( int i=0;i<m_pSize; ++i )
		{
			fprintf(fp,"f %d %d %d\n", m_pPolys[i].idx[0] + 1, m_pPolys[i].idx[1] + 1, m_pPolys[i].idx[2] + 1 ) ;
		}
		fclose(fp) ;
	}

	

	bool exportStl( const char *fname)
	{	
		FILE* fp = fopen(fname,"w") ;
		if( !fp ) return false ;

		fprintf(fp,"solid tmesh\n") ;
		for( int i=0; i < m_pSize; ++i )
		{
			fprintf(fp,"facet normal %f %f %f\n", m_pNorms[i][0], m_pNorms[i][1], m_pNorms[i][2]) ;
			fprintf(fp,"  outer loop\n") ;

			int *p = m_pPolys[i].idx;
			fprintf(fp,"    vertex %f %f %f\n", m_vVerts[p[0]][0], m_vVerts[p[0]][1], m_vVerts[p[0]][2]) ;
			fprintf(fp,"    vertex %f %f %f\n", m_vVerts[p[1]][0], m_vVerts[p[1]][1], m_vVerts[p[1]][2]) ;
			fprintf(fp,"    vertex %f %f %f\n", m_vVerts[p[2]][0], m_vVerts[p[2]][1], m_vVerts[p[2]][2]) ;

			fprintf(fp,"  endloop\n") ;
			fprintf(fp,"endfacet\n") ;
		}
		fprintf(fp,"endsolid tmesh\n") ;
		fclose(fp) ;

		return true;

	}



	void initializeIcosaHedron(const double r)
	{
		float a = (float)(r * 0.525731);
		float b = (float)(r * 0.850651);

		vector<EVec3f> Vs = {
			EVec3f(0, -a,  b), EVec3f( b, 0, a), EVec3f( b, 0,-a), EVec3f(-b, 0, -a), EVec3f(-b, 0, a), EVec3f(-a, b, 0),
			EVec3f( a, b, 0 ), EVec3f( a,-b, 0), EVec3f(-a,-b, 0), EVec3f( 0,-a, -b), EVec3f(0, a, -b), EVec3f(0,  a, b) };
		vector<TPoly> Ps = {
			TPoly( 1,  2,  6), TPoly(1,  7,  2), TPoly( 3,  4,  5), TPoly(4,  3,  8),
			TPoly( 6,  5, 11), TPoly(5,  6, 10), TPoly( 9, 10,  2), TPoly(10, 9,  3),
			TPoly( 7,  8,  9), TPoly(8,  7,  0), TPoly(11,  0,  1), TPoly( 0,11,  4),
			TPoly( 6,  2, 10), TPoly(1,  6, 11), TPoly( 3,  5, 10), TPoly( 5, 4, 11),
			TPoly( 2,  7,  9), TPoly(7,  1,  0), TPoly( 3,  9,  8), TPoly( 4, 8,  0) };
		initialize(Vs,Ps);
	}


	void initializeSphere(const double r, const int M, const int N)
	{
		//todo
	}



	bool pickByRay( const EVec3f &rayP, const EVec3f &rayD, EVec3f &pos, int &pIdx ) const
	{
		float depth = FLT_MAX;
		EVec3f tmpPos;
		pIdx  = -1;
		for (int pi = 0; pi < m_pSize; ++pi)
		{
			const int *p = m_pPolys[pi].idx;
			if (t_intersectRayToTriangle( rayP, rayD, m_vVerts[p[0]], m_vVerts[p[1]], m_vVerts[p[2]], tmpPos) )
			{
				float d = (tmpPos - rayP).norm();
				if (d < depth)
				{
					depth = d;
					pos   = tmpPos;
					pIdx  = pi;
				}
			}
		}
		return depth != FLT_MAX;
	}
};
