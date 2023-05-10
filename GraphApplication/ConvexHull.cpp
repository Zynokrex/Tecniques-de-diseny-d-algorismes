#include "pch.h"
#include "Graph.h"
#include "GraphApplicationDlg.h"
#include <set>

#define DBL_MAX numeric_limits<double>::max()



// =============================================================================
// CONVEX HULL =================================================================
// =============================================================================

// left ========================================================================
// Recta de p1 a p2. i posició del punt p respecte la recta
// resultat>0: p a la esquerra.
// resultat==0: p sobre la recta.
// resultat<0: p a la dreta

double PosicioRespeteRecta(CGPoint& a, CGPoint& b, CGPoint &c)
{
	return (a.m_Y - b.m_Y) * (c.m_X - b.m_X) - (a.m_X - b.m_X) * (c.m_Y - b.m_Y);
}

// AreaTriangle ================================================================

double AreaTriangle(CGPoint& a, CGPoint& b, CGPoint c)
{	
	return abs((a.m_Y - b.m_Y) * (c.m_X - b.m_X) - (a.m_X - b.m_X) * (c.m_Y - b.m_Y)) / 2.0;
}

// PuntsExtrems ================================================================

void PuntsExtrems(CGraph& graph, CVertex** esq_ret, CVertex** drt_ret)
{
	double min = DBL_MAX, max = 0, x;
	CVertex* esq = NULL, *drt = NULL;

	for (CVertex& v : graph.m_Vertices) {
		x = v.m_Point.m_X;
		if (x < min) {
			min = x;
			esq = &v;
		}
		if (x > max){
			max = x;
			drt = &v;
		}
	}

	//Evitem que siguin iguals, substituim pel segon

	if (min == max){
		unsigned i = 0;
		for (CVertex& v : graph.m_Vertices) {
			if (i == 1) {
				drt = &v;
				break;
			}
			i++;
		}
	}

	*esq_ret = esq;
	*drt_ret = drt;
}

// QuickHull ===================================================================

void RecursiveHull(list<CVertex*>* conjunt_final, list<CVertex*> ll, CGPoint esq, CGPoint drt)
{
	double area_max = 0, area;
	CVertex* obj = NULL;

	for (CVertex* p : ll) {
		area = AreaTriangle(esq, drt, p->m_Point);
		if (area > area_max) {
			area_max = area;
			obj = p;
		}
	}

	double pos1, pos2;
	list<CVertex*> subconjunt1, subconjunt2;

	for (CVertex* v : ll) {
		pos1 = PosicioRespeteRecta(esq, obj->m_Point, v->m_Point);
		pos2 = PosicioRespeteRecta(obj->m_Point, drt, v->m_Point);
		if (pos1 > 0) {
			subconjunt1.push_back(v);
		}
		if (pos2 > 0) {
			subconjunt2.push_back(v);
		}
	}

	if (!subconjunt1.empty()) RecursiveHull(conjunt_final, subconjunt1, esq, obj->m_Point);
	conjunt_final->push_back(obj);
	if (!subconjunt2.empty()) RecursiveHull(conjunt_final, subconjunt2, obj->m_Point, drt);
}

CConvexHull QuickHull(CGraph& graph)
{
	CConvexHull resultat = CConvexHull(&graph);

	switch (graph.m_Vertices.size())
	{
	case (0):
		return resultat;
		break;
	case (1):
		resultat.m_Vertices.push_back(&(graph.m_Vertices.front()));
		return resultat;
		break;
	case (2):
		if (graph.m_Vertices.front().m_Point == graph.m_Vertices.back().m_Point) {
			resultat.m_Vertices.push_back(&(graph.m_Vertices.front()));
		}
		else {
			resultat.m_Vertices.push_back(&(graph.m_Vertices.front()));
			resultat.m_Vertices.push_back(&(graph.m_Vertices.back()));
		}
		return resultat;
		break;
	default:
		break;
	}

	list<CVertex*> conjunt_final;

	CVertex* esq, *drt;
	PuntsExtrems(graph, &esq, &drt);

	conjunt_final.push_back(esq);

	list<CVertex*> subconjunt_esq, subconjunt_drt;
	double pos;

	for (CVertex& v : graph.m_Vertices) {
		pos = PosicioRespeteRecta(esq->m_Point, drt->m_Point, v.m_Point);
		if (pos < 0) {
			subconjunt_drt.push_back(&v);
		}
		else if (pos > 0) {
			subconjunt_esq.push_back(&v);
		}
	}
	
	RecursiveHull(&conjunt_final, subconjunt_esq, esq->m_Point, drt->m_Point);
	conjunt_final.push_back(drt);
	RecursiveHull(&conjunt_final, subconjunt_drt, drt->m_Point, esq->m_Point);

	resultat.m_Vertices = conjunt_final;

	return resultat;
}