
// SimpleObjViewerView.h : CSimpleObjViewerView クラスのインターフェイス
//

#pragma once

#include "./COMMON/OglForMFC.h"
#include "./COMMON/tmesh.h"
#include "./COMMON/OglImage.h"
#include "expmap.h"



class CSimpleObjViewerView : public CView
{
	OglForMFC		  m_ogl    ;
	TMesh			  m_mesh   ;
	OGLImage2D4       m_texture;
	vector<ExpMapVtx> m_expMap;

	bool m_bL, m_bR, m_bM;




protected: // シリアル化からのみ作成します。
	CSimpleObjViewerView();
	DECLARE_DYNCREATE(CSimpleObjViewerView)

// 属性
public:
	CSimpleObjViewerDoc* GetDocument() const;

// 操作
public:

// オーバーライド
public:
	virtual void OnDraw(CDC* pDC);  // このビューを描画するためにオーバーライドされます。
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 実装
public:
	virtual ~CSimpleObjViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成された、メッセージ割り当て関数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp  (UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp  (UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp  (UINT nFlags, CPoint point);
	afx_msg void OnMouseMove  (UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // SimpleObjViewerView.cpp のデバッグ バージョン
inline CSimpleObjViewerDoc* CSimpleObjViewerView::GetDocument() const
   { return reinterpret_cast<CSimpleObjViewerDoc*>(m_pDocument); }
#endif

