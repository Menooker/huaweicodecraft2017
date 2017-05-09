//
//  Graph.cpp
//  
//
//  Created by Mordekaiser on 17/3/5.
//
//

#include "Graph.h"
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

Graph::Graph(vector<vector<int>>& graph_param) :  numVertexs( graph_param[0][0] ), edges(graph_param[0][1]),
numConsumes( graph_param[0][2]),    cost_per_service( graph_param[1][0] ),
contains(numVertexs, vector<int>(numVertexs, MYINFINITY)),
single_price(numVertexs, vector<int>(numVertexs, MYINFINITY)),
//PathMatrix(numVertexs, vector<int>(numVertexs, MYINFINITY)),    //�洢·��
//ShortPathTable(numVertexs, vector<int>(numVertexs, MYINFINITY)), //��̾�����󣨴洢������֮�����̾��룩
service_position(0)
{
    // ��ʼ���ڽӾ���
    // graph_param[0][1]��ʾ������·����
    for (int i = 2 ; i < graph_param[0][1] + 2; i++) {
                // graph_param[i][2]��ʾ����
                // graph_param[i][3]��ʾ����
                contains[graph_param[i][0]][graph_param[i][1]] = graph_param[i][2];
                contains[graph_param[i][1]][graph_param[i][0]] = graph_param[i][2];
                
                single_price[graph_param[i][0]][graph_param[i][1] ] = graph_param[i][3];
                single_price[graph_param[i][1]][graph_param[i][0] ] = graph_param[i][3];
        
                //ShortPathTable[graph_param[i][0]][graph_param[i][1] ] = graph_param[i][3];
               // ShortPathTable[graph_param[i][1]][graph_param[i][0] ] = graph_param[i][3];
    }
    
    
    
    
    
    
    
    int line_number = graph_param[0][1] + 2;
    for (int i = 0; i < numConsumes; i++) {
        consume_position.insert(std::make_pair( graph_param[line_number][0], std::make_pair(graph_param[line_number][1], graph_param[line_number][2] )));
        
        // std::cout << graph_param[line_number][1] << std::endl;
        line_number++;
    }
    
    for (auto i = consume_position.begin(); i != consume_position.end(); i++) {  // ����ÿһ�����ѵ�
        if (NetNodeNeed.find(i->second.first) != NetNodeNeed.begin())    // Ҳ����˵�����˶�����ѵ�����һ�������
            NetNodeNeed[i->second.first] += i->second.second;
        else
            NetNodeNeed[i->second.first] = i->second.second;
    }
        
}

void Graph::ShortestPath_Floyd() {
    // ��ʼ��PathMatrix
    for (int i = 0; i < numVertexs; i++)
        for (int j = 0; j < numVertexs; j++)
            PathMatrix[i][j] = j;

    for (int k = 0; k < numVertexs; k++)
        for (int v = 0; v < numVertexs; v++)
            for (int w = 0; w < numVertexs; ++w)
                if (ShortPathTable[v][w] >  (ShortPathTable[v][k] + ShortPathTable[k][w]) ) {
                    ShortPathTable[v][w] = ShortPathTable[v][k] + ShortPathTable[k][w];
                    PathMatrix[v][w] = PathMatrix[v][k];
                }
    
    int k;

    for (auto temp = NetNodeNeed.begin(); temp != NetNodeNeed.end(); temp++) {
        int mindistance = MYINFINITY;
        int w;
        for (auto each = NetNodeNeed.begin(); each != NetNodeNeed.end(); each++) {
            if (temp->first == each->first)
                continue;
            else {
                if (mindistance > ShortPathTable[temp->first][each->first]) {
                    w = each->first;
                    mindistance = ShortPathTable[temp->first][each->first];
                    //cout << w << endl;
                }
            }
        }
        
        //cout << temp->first << " To " << w << endl;
     // ��Ҫ���ѵ㣿����Ҫ����ʱ��Ҫ��
        ServicePositionByFolyd.push_back(temp->first);
        k = PathMatrix[temp->first][w];
        //cout << temp ->first << " "<< k;
        while (k != w) {
            ServicePositionByFolyd.push_back(k);
            k = PathMatrix[k][w];
            //cout << " " << k;
        }
        //cout << " " << w << endl;
        ServicePositionByFolyd.push_back(w);
        
    }
    
    std::sort(ServicePositionByFolyd.begin(), ServicePositionByFolyd.end());
    ServicePositionByFolyd.erase(std::unique(ServicePositionByFolyd.begin(), ServicePositionByFolyd.end()),
                                 ServicePositionByFolyd.end());
}
