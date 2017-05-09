//
//  MiniCostFlow.h
//  
//
//  Created by Mordekaiser on 17/4/3.
//
//

#ifndef ____MiniCostFlow__
#define ____MiniCostFlow__

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <string>
#include "deploy.h"
#include "Graph.h"
#include "Genome.h"
#include <queue>

class MiniCostFlow {
private:
    #define MAXNODE 1000
    #define MAXEDGE MAXNODE*MAXNODE
    #define MIN(a,b) ((a)<(b)?(a):(b))
    #define OPPOSITE(x) (((x)&1)?((x)+1):((x)-1))   // xΪ�����ͼ�1��xΪż����-1
    
    int N,S,T;
    // c��ʾ������cost��ʾ���ۣ�endt��ʾ������
    int begint[MAXNODE+1],endt[MAXEDGE+1],nextt[MAXEDGE+1],c[MAXEDGE+1],cost[MAXEDGE+1],d[MAXNODE+1],cur[MAXNODE+1];
    int cT[MAXEDGE+1],bT[MAXNODE+1];
    bool haslabel[MAXNODE+1];
    int Count = 0;  // ��¼����ͼ�бߵ�����
    vector<int> tmp1;
    vector<int> tmp2;
    vector<vector<int>> tmp3;
    int NumConsumeNode[500];
    int NodeNum ;
    int EdgeNum ;
    int GustNum ;
    int needFlow;
    int allFlow;
    
public:
    void initial(Graph& graph,vector<vector<int>> &graph_param);    // ��ʼ����С��������Ҫ����ر���
    int solve(Graph& graph,vector<vector<int>> &graph_param);   // �����С������

    int getCost(); // ��ȡ����
    bool Changelabel();//�������
    int getAugmentChain(int u,int f);// ��������
    void add_Node(int a,int b,int flow, int v); //�������ͼ
    void ClearAll(int a,int b,int flow, int v); //������У�ÿ�μ��������ִ��
    void ClearAll_butc(int a,int b,int flow, int v);

    // �����С��������������ÿһ���ߵ����������ڽ��������ֻ��Ҫ�����һ��
    vector<float> getResult(Graph& graph,vector<vector<int>> &graph_param);
    
    // ���������������ڻ�ȡ��ʼ��Ⱥ����������С������������ȡһ���ȽϺõĳ�ʼ�⣬Ȼ����Ŵ��㷨�����Ż�
    std::list<Genome::Individual> getInitialPopulation(Graph& graph,vector<vector<int>> &graph_param);
    std::list<Genome::Individual> getInitialPopulation_Improve(Graph& graph,vector<vector<int>> &graph_param, const vector<int>& inputservice);
};










#endif /* defined(____MiniCostFlow__) */
