#include "pch.h"
#include "Graph.h"
#include <set>


struct Pas {
	double longitud;
	CVertex* Inici;
	CVertex* Final;
	bool usat;
};

struct Caminito {
	double longitud=0;
	list<CVertex*> cami;
};

struct NodeCami {
	CEdge* m_pEdge;
	NodeCami* m_pAnterior;
};

//CTrack* CamiMinim = NULL, * cami_actual = NULL;
Caminito CamiMinim;
CTrack CamiOptim(NULL);
double LongitudCamiOptim, LongitudActual;
CVisits visits_greedy = NULL;
CVertex* pOrigen = NULL, * pActual = NULL, * pObjectiu = NULL,* pDesti1 = NULL;
double dist_actual;
unsigned j, i_v, i_b,jndex, nvisits;

void BackTrackingGreedyRec(Pas**, Caminito*, unsigned);
unsigned VertextoIndex(CVertex*, Pas**, unsigned);

/*void copy(Pas* M, Pas path) {
	M->cami.Append(path.cami);
	M->longitud = path.longitud;
	M->usat = path.usat;
}*/

// =============================================================================
// SalesmanTrackBacktracking ===================================================
// =============================================================================

void TrobaCami4(NodeCami* pAnterior, CVertex* pActual)
{
	if (pActual == pDesti1) {
		if (LongitudActual < LongitudCamiOptim) {
			CamiOptim.Clear();
			while (pAnterior) {
				CamiOptim.m_Edges.push_front(pAnterior->m_pEdge);
				pAnterior = pAnterior->m_pAnterior;
			}
			LongitudCamiOptim = LongitudActual;
		}
	}
	else if (LongitudActual < LongitudCamiOptim) {
		pActual->m_JaHePassat = true;
		NodeCami node;
		node.m_pAnterior = pAnterior;
		for (CEdge* pE : pActual->m_Edges) {
			if (!pE->m_pDestination->m_JaHePassat) {
				node.m_pEdge = pE;
				LongitudActual += pE->m_Length;
				TrobaCami4(&node, pE->m_pDestination);
				LongitudActual -= pE->m_Length;
			}
		}
		pActual->m_JaHePassat = false;
	}
}

CTrack TrobaCami4(CVertex* inici, CVertex* desti, CGraph& g)
{
	CVertex* pInici1 = inici;
	pDesti1 = desti;
	CamiOptim.Clear();
	LongitudCamiOptim = numeric_limits<double>::max();
	LongitudActual = 0.0;
	for (CVertex& v : g.m_Vertices) v.m_JaHePassat = false;
	TrobaCami4((NodeCami*) NULL, pInici1);
	return CamiOptim;
}

CTrack SalesmanTrackBacktracking(CGraph &graph, CVisits &visits)
{
	CTrack cami_minim(&graph);
	CTrack cami(&graph);
	Caminito solucio;

	Pas** M;
	i_v = 0;
	i_b = 0;
	nvisits = visits.GetNVertices();

	//Creem i omplim matriu amb greedy i array de visites

	M = new Pas * [nvisits];
	for (unsigned i = 0; i < nvisits; i++) {
		M[i] = new Pas[nvisits];
	}

	pOrigen = visits.m_Vertices.front();
	pObjectiu = visits.m_Vertices.back();

	for (CVertex* v : visits.m_Vertices) {
		if (v == pObjectiu) break;
		for (CVertex* b : visits.m_Vertices) {
			if (i_v == 1) pActual = v;
			if (v == b) {
				M[i_v][i_b] = { 0.0,v,b,false };
				i_b++;
				continue;
			}
			cami = TrobaCami4(v, b, graph);
			M[i_v][i_b] = { cami.Length(),v,b, false };
			i_b++;
		}
		i_b = 0;
		i_v++;
	}


	//Apliquem backtracking sobre la matriu
	dist_actual = 0;
	LongitudCamiOptim = numeric_limits<double>::max();
	Caminito cami_actual;
	//cami_actual.cami.push_back(pOrigen);

	BackTrackingGreedyRec(M, &cami_actual, nvisits);

	//Transformar la solucio a un cami de CTrack amb els greedy;
	visits_greedy.Add(CamiMinim.cami.front());
	CamiMinim.cami.pop_front();
	for (CVertex* v : CamiMinim.cami) {
		visits_greedy.Add(v);
		cami_minim.Append(SalesmanTrackGreedy(graph, visits_greedy));
		visits_greedy.Add(v);
	}

	return cami_minim;
}


// =============================================================================
// SalesmanTrackBacktrackingGreedy =============================================
// =============================================================================


CTrack SalesmanTrackBacktrackingGreedy(CGraph& graph, CVisits& visits)
{
	CTrack cami_minim(&graph);
	CTrack cami(&graph);
	Caminito solucio;

	Pas** M;
	i_v = 0;
	i_b = 0;
	nvisits = visits.GetNVertices();

	//Creem i omplim matriu amb greedy i array de visites

	M = new Pas * [nvisits];
	for (unsigned i = 0; i < nvisits; i++) {
		M[i] = new Pas[nvisits];
	}

	pOrigen = visits.m_Vertices.front();
	pObjectiu = visits.m_Vertices.back();

	for (CVertex* v : visits.m_Vertices){
		if (v == pObjectiu) break;
		for (CVertex* b : visits.m_Vertices) {
			visits_greedy.m_Vertices.push_back(v);
			if (i_v == 1) pActual = v;
			if (v == b) {
				M[i_v][i_b] = { 0.0,v,b,false };
				i_b++;
				continue;
			}
			visits_greedy.m_Vertices.push_back(b);
			cami = SalesmanTrackGreedy(graph, visits_greedy);
			M[i_v][i_b] = { cami.Length(),v,b, false };
			i_b++;
		}
		i_b = 0;
		i_v++;
	}


	//Apliquem backtracking sobre la matriu
	dist_actual = 0;
	LongitudCamiOptim = numeric_limits<double>::max();
	Caminito cami_actual;
	//cami_actual.cami.push_back(pOrigen);

	BackTrackingGreedyRec(M, &cami_actual, nvisits); 

	//Transformar la solucio a un cami de CTrack amb els greedy;
	visits_greedy.Add(CamiMinim.cami.front());
	CamiMinim.cami.pop_front();
	for (CVertex* v : CamiMinim.cami) {
		visits_greedy.Add(v);
		cami_minim.Append(SalesmanTrackGreedy(graph, visits_greedy));
		visits_greedy.Add(v);
	}

	return cami_minim;
}

void BackTrackingGreedyRec(Pas** M, Caminito* Juan, unsigned nvisits) {
	//Estem al inici
	if (Juan->cami.size() == 0) {
		pActual = pOrigen;
		Juan->cami.push_back(pActual);
		for (unsigned j = 0; j < nvisits - 1; j++) {
			M[j][0].usat = true;
		}

	}
	else pActual = Juan->cami.back();

	//Va be, o eso parece
	unsigned index = VertextoIndex(pActual, M, nvisits);

	//Estem al final
	if (Juan->cami.size() == nvisits - 1) {
		dist_actual = Juan->longitud + M[index][nvisits-1].longitud;
		if (dist_actual >= LongitudCamiOptim) return;
		CamiMinim = *Juan;
		CamiMinim.cami.push_back(M[index][nvisits - 1].Final);
		CamiMinim.longitud += M[index][nvisits - 1].longitud;
		LongitudCamiOptim = dist_actual;
		return;
	}

	for (unsigned i = 0; i < nvisits - 1; i++) {

		if (M[index][i].usat || index == i) continue;
		dist_actual = Juan->longitud + M[index][i].longitud;
		if (dist_actual >= LongitudCamiOptim) continue;
		jndex = VertextoIndex(Juan->cami.back(), M, nvisits);
		Juan->cami.push_back(M[index][i].Final); 
		Juan->longitud += M[index][i].longitud;
		
		for (unsigned j = 0; j < nvisits; j++) {
			if (j < nvisits - 1) M[j][index].usat = true;
			M[jndex][j].usat = true;
		}

		BackTrackingGreedyRec(M, Juan, nvisits);
		
		for (unsigned j = 0; j < nvisits; j++) {
			if (j < nvisits - 1) M[j][index].usat = false;
			M[jndex][j].usat = false;
		}
		Juan->longitud -= M[index][i].longitud;
		Juan->cami.pop_back();
	}
	return;
}

//Pensar 
unsigned VertextoIndex(CVertex* v, Pas** M, unsigned nvis) {
	CVertex* aux;
	unsigned i_ver;
	for (i_ver = 0; i_ver < nvis - 1; i_ver++) {
		if (i_ver == 0) aux = pOrigen;
		else aux = (&M[i_ver][0])->Inici;
		if (aux == v) return i_ver;
	}
	return nvis;
}