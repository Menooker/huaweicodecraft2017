//
//  Graph.h
//  ͼ���ڽӾ���ʵ��
//
//  Created by Mordekaiser on 17/3/5.
//
//

#ifndef ____Graph__
#define ____Graph__

#include <stdio.h>
#include <vector>
#include <unordered_map>
#include <list>
#include <map>
#include "lib_io.h"

#define MYINFINITY 147483647 // ��ʾ�޽⣬����û��������

using std::vector;


class Graph {
public:
    int numVertexs;
    int numConsumes;
    int cost_per_service;
    int edges;
    
    
    // INFINITY��ʾû�б�
    vector<vector<int>> contains;   // numVertexs x numVertexs�Ķ�ά���󣬴洢����
    vector<vector<int>> single_price;   // numVertexs x numVertexs�Ķ�ά���󣬴洢��������
    
    // �洢���ѽڵ��λ�ã�key�����ѽڵ㣬��Ӧ��value��һ��Ԫ��, Ԫ���һ��Ԫ�������ѽڵ����ӵ�����ڵ㣬�ڶ���ֵ�Ǵ���
    std::map<int, std::pair<int, int>> consume_position;
    
    std::vector<int> service_position;   // ���Ŵ��㷨ѡȡ�ķ�����λ�ô�����
    
    //��ÿһ�������б�ţ������������Թ滮���֮��ԭ�����
    std::map<std::pair<int, int>, int> edgesToIndex;
    
    // ��һ����ʾ�ߵı�ţ��ߵ���������
    std::map<int, std::pair<int, int> > indexToEdges;
    vector<vector<int>> result; // ������
    
    // �洢���ѵ��Ӧ�Ĵ�������consume_position�����Ա�������ô��������ѵ��Ӧһ������ڵ�ĳ���
    //  key�����(���ѵ��Ӧ�������)��value�Ǵ�������
    std::map<int, int> NetNodeNeed;
    vector<int> servicePositionInConsume;
    
    
    // Floyd�㷨�����·��
    vector<vector<int>> PathMatrix;
    vector<vector<int>> ShortPathTable;
    vector<int> ServicePositionByFolyd;
    void ShortestPath_Floyd();
    
public:
    Graph(vector<vector<int>>& graph_param); // ���ý�����Ľ������ͼ
    //explicit Graph(Graph const &graph);   ��constӦ����Ϊ�������캯���ᵼ��BFS����������֪����ô�����
    
};
#endif /* defined(____Graph__) */
