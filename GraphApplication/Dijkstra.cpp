#include "pch.h"
#include "Graph.h"
#include <queue>
#include <tuple>

#define DBL_MAX numeric_limits<double>::max()

bool operator< (const tuple<CVertex*, double> a, const tuple<CVertex*, double> b) {
    return get<1>(a) > get<1>(b);
}

bool operator> (const tuple<CVertex*, double> a, const tuple<CVertex*, double> b) {
    return get<1>(a) < get<1>(b);
}

// =============================================================================
// Dijkstra ====================================================================
// =============================================================================

void Dijkstra(CGraph& graph, CVertex* pStart)
{
    //Inicialitzacio
    unsigned n_vertex = graph.m_Vertices.size();

    for (CVertex& v : graph.m_Vertices) {
        v.m_DijkstraDistance = DBL_MAX;
        v.m_DijkstraVisit = 0;
    }
    pStart->m_DijkstraDistance = 0;

    CVertex* pActual = pStart;

    for (unsigned i = 0; i < n_vertex; i++) {

        for (CEdge* e : pActual->m_Edges) {
            CVertex* neighbor = e->m_pDestination;
            double newDist = pActual->m_DijkstraDistance + e->m_Length;

            if (neighbor->m_DijkstraDistance > newDist) {
                neighbor->m_DijkstraDistance = newDist;
            }
            
        }
        pActual->m_DijkstraVisit = 1;
        double min_dist = DBL_MAX;

        for (CVertex& v : graph.m_Vertices) {
            if (v.m_DijkstraVisit == 0 && v.m_DijkstraDistance < min_dist) {
                min_dist = v.m_DijkstraDistance;
                pActual = &v;
            }
        }
    }

}

// =============================================================================
// DijkstraQueue ===============================================================
// =============================================================================

void DijkstraQueue(CGraph& graph, CVertex* pStart)
{
  priority_queue <tuple<CVertex*,double>, vector<tuple<CVertex*,double>>, less<tuple<CVertex*,double>>> pq;

  for (CVertex& v : graph.m_Vertices) {
      v.m_DijkstraDistance = DBL_MAX;
      v.m_DijkstraVisit = 0;
  }
  pStart->m_DijkstraDistance = 0.;
  pStart->m_DijkstraAnterior = NULL;

  tuple <CVertex*, double> pActual(pStart, 0.0);
  pq.push(pActual);

  while (!pq.empty()) {
      pActual = pq.top();
      pq.pop();

      if (get<0>(pActual)->m_DijkstraVisit == 1) { continue; }

      for (CEdge* e : get<0>(pActual)->m_Edges) {
          CVertex* neighbor = e->m_pDestination;
          double newDist = get<0>(pActual)->m_DijkstraDistance + e->m_Length;

          if (neighbor->m_DijkstraDistance > newDist) {
              neighbor->m_DijkstraDistance = newDist;
              tuple <CVertex*, double> pVei(neighbor, newDist);
              neighbor->m_DijkstraAnterior = e;
              pq.push(pVei);
          }
          get<0>(pActual)->m_DijkstraVisit = 1;
      }
  }
}
