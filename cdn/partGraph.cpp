//
//  partGraph.cpp
//  
//
//  Created by Mordekaiser on 17/3/7.
//
//
#include "Graph.h"
#include <vector>

#include "partGraph.h"


partGraph::partGraph(const Graph& graph, int& _search_consume_position,
                     vector<int>& _service_position) : Graph(graph){
    // �������ʵ�֣����û��๹�캯��, ����ʼ����Ӧ��Ա
    search_consume_position = _search_consume_position;// ����������ѽڵ�Ϳ��Եõ�һ��Сͼ
    service_position = _service_position;   // �õ���Сͼ������������λ��
    
    
}

