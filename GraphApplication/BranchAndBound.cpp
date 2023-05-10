#include "pch.h"
#include "Graph.h"
#include <queue>
#include <iostream>
#include <iomanip> 
#include <tuple>


struct Pas {
	double longitud;
	double longitud_acumulada;
	CVertex* Inici;
	CVertex* Final;

};

class Juanca {
public:
	double Cost;
	vector<unsigned> path;
public:
	void clear() {
		Cost = 0;
		path.clear();
	}
	bool contains(int index) {
		for (int i : path) {
			if (index == i) return true;
		}
		return false;
	}
};



//Caminito Jose;
CVisits visits_greedy2 = NULL;
CVertex* pOrigen2 = NULL, * pActual2 = NULL, * pObjectiu2 = NULL, *pIniciActual = NULL, *pDestiActual = NULL;
double dist_actual2, inf = numeric_limits<double>::max();
unsigned j2, i_v2, i_b2, nvisits2;
Pas* pasActual;

struct comparatorBB {
	bool operator()(Juanca node_1, Juanca node_2) { return node_1.Cost > node_2.Cost; }
};



// SalesmanTrackBranchAndBound1 ===================================================

CTrack SalesmanTrackBranchAndBound1(CGraph& graph, CVisits& visits)
{

	CTrack cami(&graph);

	Pas** M;
	i_v2 = 0;
	i_b2 = 0;
	nvisits2 = visits.GetNVertices();

	//Creem i omplim matriu amb greedy i array de visites

	M = new Pas * [nvisits2];
	for (unsigned i = 0; i < nvisits2; i++) {
		M[i] = new Pas[nvisits2];
	}

	pOrigen2 = visits.m_Vertices.front();
	pObjectiu2 = visits.m_Vertices.back();

	for (CVertex* v : visits.m_Vertices) {
		for (CVertex* b : visits.m_Vertices) {
			visits_greedy2.m_Vertices.push_back(v);
			if (i_v2 == 1) pActual2 = v;
			if (v == b) {
				M[i_v2][i_b2] = { inf,0.0,v,b};
				i_b2++;
				continue;
			}
			visits_greedy2.m_Vertices.push_back(b);
			cami = SalesmanTrackGreedy(graph, visits_greedy2);
			M[i_v2][i_b2] = { cami.Length(),0.0,v,b};
			i_b2++;
		}
		i_b2 = 0;
		i_v2++;
	}

	priority_queue <Juanca,  vector<Juanca>, comparatorBB> pq;
	Juanca tram_actual, tram_aux;
	
	tram_actual.Cost = 0;
	tram_actual.path.push_back(0);
	pq.push(tram_actual);

	while (!pq.empty()) {
		vector<unsigned> indices;
		tram_actual = pq.top();
		pq.pop();

		if (tram_actual.path.size() == nvisits2) break;

		//indices.clear();
		for (int k = 1; k < nvisits2; k++) {
			if (!tram_actual.contains(k)) {
				if (tram_actual.path.size() == nvisits2 - 1 && k == nvisits2 - 1) indices.emplace_back(k);
				else if (k != nvisits2 - 1) indices.emplace_back(k);
			}
		}
		for (int index : indices) {
			tram_aux.clear();
			tram_aux.Cost = tram_actual.Cost + M[tram_actual.path.back()][index].longitud;
			tram_aux.path = tram_actual.path;
			tram_aux.path.emplace_back(index);
			pq.push(tram_aux);

		}
	}



	//Funciona correctament la conversio
	//pasem el cami a manueh per poder fer pop_front

	list<unsigned> manueh;
	for (unsigned f : tram_actual.path) {
		manueh.push_back(f);
	}

	CTrack cami_minim(&graph);
	visits_greedy2.Clear();
	//Transformar la solucio (tram_actual) a un cami de CTrack
	visits_greedy2.Add(M[manueh.front()][0].Inici);
	manueh.pop_front();
	for (unsigned i = 0; i < nvisits2 - 1; i++) {
		visits_greedy2.Add(M[manueh.front()][0].Inici);
		cami_minim.Append(SalesmanTrackGreedy(graph,visits_greedy2));
		visits_greedy2.Add(M[manueh.front()][0].Inici);
		manueh.pop_front();
	}
	return cami_minim;
}

// SalesmanTrackBranchAndBound2 ===================================================

CTrack SalesmanTrackBranchAndBound2(CGraph& graph, CVisits &visits)
{

	CTrack cami(&graph);
	
	Pas** M;
	i_v2 = 0;
	i_b2 = 0;
	nvisits2 = visits.GetNVertices();

	//Nova heuristica
	//vector<double> vector_distancies;
	double* vector_distancies;
	vector_distancies = new double[nvisits2];
	for (int k = 0; k < nvisits2; k++) vector_distancies[k] = inf;

	//graf massa gran, massa temps
	if (nvisits2 >= 16) return NULL;
	//Creem i omplim matriu amb greedy i array de visites

	M = new Pas * [nvisits2];
	for (unsigned i = 0; i < nvisits2; i++) {
		M[i] = new Pas[nvisits2];
	}

	pOrigen2 = visits.m_Vertices.front();
	pObjectiu2 = visits.m_Vertices.back();

	for (CVertex* v : visits.m_Vertices) {
		for (CVertex* b : visits.m_Vertices) {
			visits_greedy2.m_Vertices.push_back(v);
			if (i_v2 == 1) pActual2 = v;
			if (v == b) {
				M[i_v2][i_b2] = { inf,0.0,v,b };
				i_b2++;
				continue;
			}
			visits_greedy2.m_Vertices.push_back(b);
			cami = SalesmanTrackGreedy(graph, visits_greedy2);
			M[i_v2][i_b2] = { cami.Length(),0.0,v,b };
			if (cami.Length() < vector_distancies[i_b2]) vector_distancies[i_b2] = cami.Length();
			
			i_b2++;
		}
		i_b2 = 0;
		i_v2++;
	}

	priority_queue <Juanca, vector<Juanca>, comparatorBB> pq;
	Juanca tram_actual, tram_aux;

	tram_actual.Cost = 0;
	for (int k = 0; k < nvisits2; k++) {
		tram_actual.Cost += vector_distancies[k];
	}
	tram_actual.path.push_back(0);
	pq.push(tram_actual);

	while (!pq.empty()) {
		vector<unsigned> indices;
		tram_actual = pq.top();
		pq.pop();
		
		if (tram_actual.path.size() == nvisits2) break;

		//indices.clear();
		for (int k = 1; k < nvisits2; k++) {
			if (!tram_actual.contains(k)) {
				if(tram_actual.path.size()==nvisits2-1 && k==nvisits2-1) indices.emplace_back(k);
				else if(k!=nvisits2-1) indices.emplace_back(k);
			}
		}
		for (int index : indices) {
			tram_aux.clear();
			tram_aux.Cost = tram_actual.Cost + M[tram_actual.path.back()][index].longitud-vector_distancies[index];
			tram_aux.path = tram_actual.path;
			tram_aux.path.emplace_back(index);
			pq.push(tram_aux);

		}
	}


	//Funciona correctament la conversio
	//pasem el cami a manueh per poder fer pop_front

	list<unsigned> manueh;
	for (unsigned f : tram_actual.path) {
		manueh.push_back(f);
	}
	CTrack cami_minim(&graph);
	visits_greedy2.Clear();
	visits_greedy2.Add(M[manueh.front()][0].Inici);
	manueh.pop_front();
	for (unsigned i = 0; i < nvisits2 - 1; i++) {
		visits_greedy2.Add(M[manueh.front()][0].Inici);
		cami_minim.Append(SalesmanTrackGreedy(graph, visits_greedy2));
		visits_greedy2.Add(M[manueh.front()][0].Inici);
		manueh.pop_front();
	}
	return cami_minim;
}

// SalesmanTrackBranchAndBound3 ===================================================


CTrack SalesmanTrackBranchAndBound3(CGraph& graph, CVisits &visits)
{


	CTrack cami(&graph);

	Pas** M;
	i_v2 = 0;
	i_b2 = 0;
	nvisits2 = visits.GetNVertices();
	//graf massa gran, massa temps
	if (nvisits2 >= 16) return NULL;
	//Creem i omplim matriu amb greedy i array de visites

	M = new Pas * [nvisits2];
	for (unsigned i = 0; i < nvisits2; i++) {
		M[i] = new Pas[nvisits2];
	}

	pOrigen2 = visits.m_Vertices.front();
	pObjectiu2 = visits.m_Vertices.back();

	for (CVertex* v : visits.m_Vertices) {
		for (CVertex* b : visits.m_Vertices) {
			visits_greedy2.m_Vertices.push_back(v);
			if (i_v2 == 1) pActual2 = v;
			if (v == b) {
				M[i_v2][i_b2] = { inf,0.0,v,b };
				i_b2++;
				continue;
			}
			visits_greedy2.m_Vertices.push_back(b);
			cami = SalesmanTrackGreedy(graph, visits_greedy2);
			M[i_v2][i_b2] = { cami.Length(),0.0,v,b };
			i_b2++;
		}
		i_b2 = 0;
		i_v2++;
	}

	priority_queue <Juanca, vector<Juanca>, comparatorBB> pq;
	Juanca tram_actual, tram_aux;

	tram_actual.Cost = 0;
	tram_actual.path.push_back(0);
	pq.push(tram_actual);

	while (!pq.empty()) {
		vector<unsigned> indices;
		tram_actual = pq.top();
		pq.pop();

		if (tram_actual.path.size() == nvisits2) break;

		//indices.clear();
		for (int k = 1; k < nvisits2; k++) {
			if (!tram_actual.contains(k)) {
				if (tram_actual.path.size() == nvisits2 - 1 && k == nvisits2 - 1) indices.emplace_back(k);
				else if (k != nvisits2 - 1) indices.emplace_back(k);
			}
		}
		for (int index : indices) {
			tram_aux.clear();
			tram_aux.Cost = tram_actual.Cost + M[tram_actual.path.back()][index].longitud;
			tram_aux.path = tram_actual.path;
			tram_aux.path.emplace_back(index);
			pq.push(tram_aux);

		}
	}

	//Funciona correctament la conversio
	//pasem el cami a manueh per poder fer pop_front

	list<unsigned> manueh;
	for (unsigned f : tram_actual.path) {
		manueh.push_back(f);
	}

	CTrack cami_minim(&graph);
	visits_greedy2.Clear();
	//Transformar la solucio (tram_actual) a un cami de CTrack
	visits_greedy2.Add(M[manueh.front()][0].Inici);
	manueh.pop_front();
	for (unsigned i = 0; i < nvisits2 - 1; i++) {
		visits_greedy2.Add(M[manueh.front()][0].Inici);
		cami_minim.Append(SalesmanTrackGreedy(graph, visits_greedy2));
		visits_greedy2.Add(M[manueh.front()][0].Inici);
		manueh.pop_front();
	}
	return cami_minim;
}