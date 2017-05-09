//
//  solveAlgorith.h
//  
//
//  Created by Mordekaiser on 17/3/7.
//
//  ����㷨���������������⺯��������������

#ifndef ____solveAlgorithm__
#define ____solveAlgorithm__

#include <stdio.h>
#include "Graph.h"
#include "partGraph.h"
#include <sstream>


class solveAlgorithm {
public:
    //friend class Genome; // ʹ���ΪGenome����Ԫ�࣬����Genome����solveAlgorithm��getServicePositon�����Լ�solver����
    solveAlgorithm();   // �Ŵ��㷨����ĳ�Ա�����ڴ˳�ʼ��
    ~solveAlgorithm();
    
    inline bool isANumber(char ch) {
        return ( ch >= '0' && ch <= '9' );
    }
    
    vector<vector<int>> parseFile(char* topo[MAX_EDGE_NUM], int line_num); // �����ļ������洢�ɶ�ά����
    vector<bool> getServicePosition(Graph& graph); // ��ȡ������λ�ú���
    vector<bool> getServicePosition_GA(Graph& graph); // �Ŵ��㷨���õĻ�ȡ������λ�ú���
    
    partGraph BFS(const Graph& graph, vector<bool> servicePosition, int hiearcy, int comsume_position);  // �����������������Сͼ
    
    vector<vector<float>> getRelevanceMatrix(partGraph& graph); // ����������ͼ����������󣬻�ȡ��������֮��Ϳ��Եõ���Ӧ��Լ����
    vector<vector<float>> getRelevanceMatrix(Graph& graph);
    
    // bool searchPath(Graph& graph, const partGraph& smallGraph);
    
    
    // �������
    int returnCost(const vector<float>& flowOfEdges);
    void BFS_cut(Graph& graph);
    
    void parseResultOfILP(vector<float>& flowOfEdges, Graph& graph);
    
    void result(const char *filename,  Graph& graph);
    vector<vector<float>> getRelevanceMatrixSuper(Graph& graph);
    vector<int> BFS1( Graph& graph,  int hiearcy);
    
    void return_text(Graph& graph,vector<float> &flowOfEdges);
    std::vector<int> rand_m(int m,int n);
    
    
    int solver(Graph& graph);
    
};


#endif /* defined(____solveAlgorith__) */
