#include "pch.h"
#include "Graph.h"
#include <queue>
#include <tuple>

// SalesmanTrackGreedy =========================================================

CTrack SalesmanTrackGreedy(CGraph& graph, CVisits &visits)
{
	CTrack Cami = CTrack(&graph);

	CVertex* pStart = visits.m_Vertices.front();
	CVertex* pEnd = visits.m_Vertices.back();
	visits.Delete(pStart);
	visits.Delete(pEnd);

	while(!visits.Empty()){
		//graph.ResetAnteriors();
		DijkstraQueue(graph, pStart);
		priority_queue <tuple<CVertex*, double>, vector<tuple<CVertex*, double>>, less<tuple<CVertex*, double>>> pq;
		//Tenim distancies
		for (CVertex* v : visits.m_Vertices){
			tuple <CVertex*, double> vert(v, v->m_DijkstraDistance);
			pq.push(vert);
		}
   		pStart = get<0>(pq.top());
		visits.Delete(pStart);

    //Tenim candidats a min-heap
		CVertex* V_aux = pStart;
		CTrack T_aux(&graph);
		T_aux.Clear();
		do{
			T_aux.AddFirst(V_aux->m_DijkstraAnterior);
			V_aux = (V_aux->m_DijkstraAnterior->m_pOrigin != V_aux)? V_aux->m_DijkstraAnterior->m_pOrigin : V_aux->m_DijkstraAnterior->m_pDestination;
		}while(V_aux->m_DijkstraAnterior != NULL);
		Cami.Append(T_aux);
	}

	  DijkstraQueue(graph, pStart);
	  CVertex* V_aux = pEnd;
	  CTrack T_aux(&graph);
	  T_aux.Clear();

	  do{
		T_aux.AddFirst(V_aux->m_DijkstraAnterior);
		V_aux = (V_aux->m_DijkstraAnterior->m_pOrigin != V_aux)? V_aux->m_DijkstraAnterior->m_pOrigin : V_aux->m_DijkstraAnterior->m_pDestination;
	  }while(V_aux->m_DijkstraAnterior != NULL);
	  Cami.Append(T_aux);

	return Cami;
}
