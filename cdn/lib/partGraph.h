//
//  partGraph.h
//  
//
//  Created by Mordekaiser on 17/3/7.
//
//

#ifndef ____partGraph__
#define ____partGraph__

#include <stdio.h>
#include <vector>
#include "Graph.h"

class partGraph : public Graph {
public:
    partGraph(const Graph& graph, int& _search_consume_position,
              vector<int>& _service_position);
public:
    int search_consume_position;    // ��Ҫ���������ѽڵ��λ��
    vector<int> service_position;   // �Ѿ�ѡ���ķ������ڵ�λ��
    
    
};

#endif /* defined(____partGraph__) */
