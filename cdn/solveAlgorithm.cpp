//
//  solveAlgorith.cpp
//
//
//  Created by Mordekaiser on 17/3/7.
//
//  ����㷨��ĺ���ʵ��
#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <random>
#include <cstring>
#include "solveAlgorithm.h"
#include "MiniCostFlow.h"
using namespace std;

extern MiniCostFlow MCF;
extern vector<vector<int>> graph_param;

solveAlgorithm::solveAlgorithm() {} // ���캯��ʲô������

solveAlgorithm::~solveAlgorithm() {} // ��������ʲô������

vector<vector<int>> solveAlgorithm::parseFile(char* topo[MAX_EDGE_NUM], int line_num) {
    // @������

    vector<vector<int>> result;
    vector<int> line;
    
    
    // ��������������������·�������ѽڵ�����������result[0]��
    line.push_back(atoi(strtok(topo[0], " ")));
    line.push_back(atoi(strtok(nullptr, " ")));
    line.push_back(atoi(strtok(nullptr, " ")));
    result.push_back(line);
    
    // ��������Ƶ���ݷ������ɱ���������result[1]
    line.clear();
    line.push_back(atoi(strtok(topo[2], " ")));
    result.push_back(line);
    
    // ������������·֮������ӹ�ϵ���Լ������С���������÷ѣ�������result[2]...result[����Ŀ + 1]��
    // ���resultĳ��Ԫ��result[num][0] == 65535����ô���� == num - 2
    // ��Ҳ˵���ߵ����ӹ�ϵ�������˽���������ȥ���ļ��������������ѽڵ������ڵ�֮��Ĺ�ϵ
    // �ӵ����п�ʼ��,ÿһ�еĽ�������'\0'
    // �������εĽ�������0x0d
    int index = 0;
    int numEdges = result[0][1];
    while (index < numEdges ) {
        line.clear();
        line.push_back(atoi(strtok(topo[index + 4], " ")));
        line.push_back(atoi(strtok(nullptr, " ")));
        line.push_back(atoi(strtok(nullptr, " ")));
        line.push_back(atoi(strtok(nullptr, " ")));
                       
        result.push_back(line);
        index++;
    }

    index = 0;
    int numConsumes = result[0][2];
    while (index < numConsumes) {
        line.clear();
        line.push_back(atoi(strtok(topo[numEdges + 5 + index], " ")));
        line.push_back(atoi(strtok(nullptr, " ")));
        line.push_back(atoi(strtok(nullptr, " ")));
        result.push_back(line);
        index++;
    }
    
    
    
    return result;
}


vector<bool> solveAlgorithm::getServicePosition(Graph& graph) {
    // ��ȡ������λ�ú���
    // ע�⣺Ҫ���о�ֵ�����һ�������жȺʹ����Ȩ������
    // ע�⣬���ص�result�Ĵ�СӦ���ڶ������������������з����������Ҿ�Ϊtrue
    // @�����ܸ���
    vector<bool> result(graph.numVertexs, false);
    vector<vector<int>> uniformization(graph.numVertexs, vector<int>(graph.numVertexs, MYINFINITY));
    uniformization = graph.contains;
    //��һ������
    double sum = 0.0;
    double sum_single = 0.0;
    int  dun_single = 0;
    int  dun_sum = 0;
    vector<float> consum;
    vector<float> dun;
    for (int i = 0; i < graph.numVertexs; i++)
    {
        dun_single = 0;
        sum_single = 0;
        for (int j = 0; j < graph.numVertexs; j++)
        {
            //cout << graph.numVertexs << endl;
            if (uniformization[i][j] > 0 && uniformization[i][j] != MYINFINITY) {
                dun_single++;
                sum += uniformization[i][j];
                sum_single += uniformization[i][j];
                dun_sum++;
                
            }
        }
        dun.push_back(dun_single);
        consum.push_back(sum_single);
    }
    //cout << graph.numVertexs << endl;
    for (int i = 0; i < graph.numVertexs ; i++)
    {
        dun[i] = dun[i] / dun_sum;
        consum[i] = consum[i] / sum;
        consum[i] = 0.5*dun[i] + 0.5*consum[i];
    }
    
    vector<float> consum_copy = consum;
    
    std::sort(consum.begin(), consum.end());
    int i = 0;
    
    while (i < graph.numConsumes / 3) {
        result[ std::find(consum_copy.begin(), consum_copy.end(), consum[graph.numVertexs - i - 1]) - consum_copy.begin() ] = true;
        i++;
    }
    /*
     for (auto i : result)
     cout << i << endl;
     */
    for (int i = 0; i < result.size(); i++) {
        if (result[i] == true)
            graph.service_position.push_back(i);
    }
    
    
    return result;
}

// �����ȡ�������ڵ㺯����רΪ�Ŵ��㷨׼���ģ�
vector<bool> solveAlgorithm::getServicePosition_GA(Graph& graph) {
    // ��ȡ������λ�ú���
    // ע�⣺Ҫ���о�ֵ�����һ�������жȺʹ����Ȩ������
    // ע�⣬���ص�result�Ĵ�СӦ���ڶ������������������з����������Ҿ�Ϊtrue
    // @�����ܸ���
    //-------------------------------------
    
    vector<bool> result(graph.numVertexs, true);
    
    /*
     vector<vector<int>> uniformization(graph.numVertexs, vector<int>(graph.numVertexs, INFINITY));
     uniformization = graph.contains;
     //��һ������
     double sum = 0.0;
     double sum_single = 0.0;
     int  dun_single = 0;
     int  dun_sum = 0;
     vector<float> consum;
     vector<float> dun;
     for (int i = 0; i < graph.numVertexs; i++)
     {
     dun_single = 0;
     sum_single = 0;
     for (int j = 0; j < graph.numVertexs; j++)
     {
     //cout << graph.numVertexs << endl;
     if (uniformization[i][j] > 0 && uniformization[i][j] != INFINITY) {
     dun_single++;
     sum += uniformization[i][j];
     sum_single += uniformization[i][j];
     dun_sum++;
     
     }
     }
     dun.push_back(dun_single);
     consum.push_back(sum_single);
     }
     //cout << graph.numVertexs << endl;
     for (int i = 0; i < graph.numVertexs ; i++)
     {
     dun[i] = dun[i] / dun_sum;
     consum[i] = consum[i] / sum;
     consum[i] = 0.5*dun[i] + 0.5*consum[i];
     }
     
     vector<float> consum_copy = consum;
     
     std::sort(consum.begin(), consum.end());
     int i = 0;
     
     while (i < 3 * graph.numConsumes / 3) {
     result[ std::find(consum_copy.begin(), consum_copy.end(), consum[graph.numVertexs - i - 1]) - consum_copy.begin() ] = true;
     i++;
     }
     /*
     for (auto i : result)
     cout << i << endl;
     */
    /*
     for (int i = 0; i < result.size(); i++) {
     if (result[i] == true)
     graph.service_position.push_back(i);
     }
     */
    //---------------------------
    
    return result;
}

partGraph solveAlgorithm::BFS(const Graph& graph, vector<bool> servicePosition, int hiearcy, int comsume_position)   {
    
    // @�����θ���
    // �����������������Сͼ
    // partGraph��Graph����������
    // ����graph�ǰ�ֵ���ݣ����ڻ��кܴ��Ż��ռ䣡����
    
    // comsume_position = 2;
    
    int hiearcy_count=0;//�㼶����
    vector<int> service_position;//���ط�����λ��
    
    
    partGraph smallGraph(graph, comsume_position, service_position); // ���������������������λ����ʱ����ʼ��
    
    vector<bool> visit(smallGraph.numVertexs, false);//��¼���ʵĽڵ�
    std::queue<int> Que_hi;
    auto comsume_net = smallGraph.consume_position[comsume_position].first;//ͨ�����ѽڵ��������ڵ㣻
    int count = comsume_net;//��¼���ʵĽڵ�
    while ( hiearcy_count < hiearcy ) {//BFS
        //std::cout << hiearcy_count << " < " << hiearcy << std::endl;
        if( !visit[count] ) {
            visit[count] = 1;//���ʹ��ڵ㱣��������ɾ��
            if ( servicePosition[count] == 1 ) { //�õ��Ƿ�Ϊ�������ڵ�
                hiearcy_count++;
                // cout << "count = " << count << endl;
                smallGraph.service_position.push_back(count);//��¼�·�����λ��
            }
            if ( hiearcy_count < hiearcy)
                Que_hi.push(count);
            while ( !Que_hi.empty() ){
                //cout << "fff" << endl;
                auto i =Que_hi.front();
                Que_hi.pop();
                
                for(int m=0;m < smallGraph.numVertexs;m++)
                {
                    if( smallGraph.contains[i][m] == MYINFINITY )
                        continue;
                    
                    //std::cout << m << std::endl;
                    if( !visit[m] ){
                        visit[m] = 1; //���ʹ��ڵ㱣��������ɾ��
                        if( servicePosition[m] == 1 )   // �������Ƿ������ڵ㣬�ʹ�����
                        {
                            // cout << "m = " << m << endl;
                            if( hiearcy_count < hiearcy)
                                smallGraph.service_position.push_back(m);
                            hiearcy_count++;
                            
                        }
                        if(hiearcy_count < hiearcy)
                            Que_hi.push(m);
                    }
                }
            }
        }
        
        
    }
    
    // ��û�з��ʹ��ĵ���ΪINFINTY����ɾ��������
    int hasArcs = 0;   // ���ڼ�¼��ɾ���Ļ�������ɾ���ı������������һ��
    for(int n = 0;n < smallGraph.numVertexs;n++)
        for(int j = 0;j < smallGraph.numVertexs;j++)
        {
            if( n != j && ( visit[n] != true || visit[j] != true) ) {
                smallGraph.contains[n][j] = MYINFINITY;
                smallGraph.single_price[n][j] = MYINFINITY;
            }
            
            if (smallGraph.contains[n][j] != MYINFINITY)
                hasArcs++;
        }
    
    //  ���¶�����
    for (auto i : visit) {
        if (i != true)
            smallGraph.numVertexs--;
    }
    // ����Сͼ������������Сͼ�Ĳ����Ǵӻ�����������������Ҫ�����޸ġ�
    smallGraph.numConsumes = 1;     // �������ѵ���Ŀ
    smallGraph.edges = hasArcs / 2; // ���±ߵ���Ŀ
    auto temp = smallGraph.consume_position[comsume_position];      // �洢���ѽڵ���ڵ�value���Դ˸���Сͼ
    smallGraph.consume_position.erase( smallGraph.consume_position.begin(), smallGraph.consume_position.end() );
    smallGraph.consume_position.insert(std::make_pair(comsume_position, temp)); // �������ѽڵ�������ڵ�����ӹ�ϵ
    
    
    /*
     cout << "comsume_position: " << comsume_position << endl;
     // ���������λ��
     
     for (auto i : smallGraph.service_position)  // smallGraph��service_position��Ա���ø��£������Ա�����λ����Ա
     cout << "service position: " << i << endl;
     */
    
    //partGraph smallGraph(graph, comsume_position, service_position);
    
    return smallGraph;
}



//--------------------------- ���ܴ�ͼ�����������----------------------
vector<vector<float>> solveAlgorithm::getRelevanceMatrix(Graph& graph) {
    
    vector<vector<float>> CoefficientMatrix(graph.numVertexs+graph.edges*2+1,vector<float>(graph.edges*2+2,0));
    // map��key��ʾ�߶�Ӧ���������㣬value��ʾ�����ߵı�ţ�����
    std::map<std::pair<int, int>, int> edges;
    std::map<std::pair<int, int>, std::pair<int,int>> edges_for_contains;
    std::map<int, std::pair<int,int>> index2Edges;
    
    int count=0;
    for (int i = 0; i < graph.contains.size(); i++) {
        for (int j = 0; j < graph.contains.size(); j++)
            if ( MYINFINITY != graph.contains[i][j] ){ // ���i��j֮����ڱ�
                //cout << "edge : " << i << ", " << j << " : " << count << endl;
                edges.insert( make_pair( make_pair(i, j), count ) );
                edges_for_contains.insert( make_pair( make_pair(i, j), make_pair(count,graph.contains[i][j]) ) );
                index2Edges.insert(make_pair(count, make_pair(i, j)));
                count++;
            }
    }
    
    
    graph.edgesToIndex = edges;
    graph.indexToEdges = index2Edges;
    
    //��Լ������ֵ
    
    //Ŀ�꺯����ϵ��д�뵽����ĵ�һ��
    int single_price = 0;
    for(auto temp = edges.begin();temp != edges.end();temp++){
        single_price = graph.single_price[temp->first.first][temp->first.second];
        CoefficientMatrix[0][temp->second] = single_price;
        
    }
    
    //ÿ������Ӧ����������д����Ӧ��Լ��������ȥ
    for(auto temp = edges_for_contains.begin();temp != edges_for_contains.end();temp++)
        CoefficientMatrix[temp->second.first+1+graph.numVertexs][2*graph.edges+1] = temp->second.second;
    
    for(int j = 1;j <= graph.numVertexs; j++){
        for (int l = 0;l < graph.numVertexs; l++) {
            if (edges.find(make_pair(j-1 , l)) != edges.end()) {
                CoefficientMatrix[j][edges[make_pair(j-1, l)]] = 1;
                CoefficientMatrix[j][edges[make_pair(l, j-1)]] = -1;
            }//����Ϊ��, ����Ϊ��
        }
        CoefficientMatrix[j][graph.edges*2] = 1 ;//��ʽԼ��ϵ��
    }//�㻡���Ӿ�������ɡ�
    
    //д��������ѵ���Ҫ������
    int tempindex;
    for (auto i = graph.NetNodeNeed.begin(); i != graph.NetNodeNeed.end(); i++) {
        tempindex = i->first;
        
        //-------------------Ϊ�����Թ滮�����-----------
        // ���Թ滮���ܽ����ұ߲���С��0���������Ҫ���为�ŷ�ת
        for (int j = 0; j < CoefficientMatrix[tempindex+1].size() - 2; j++)
            CoefficientMatrix[tempindex+1][j] = - CoefficientMatrix[tempindex+1][j];
        
        CoefficientMatrix[tempindex+1][2*graph.edges+1] = graph.NetNodeNeed[tempindex];
    }
    
    for (int i = graph.numVertexs+1; i <= graph.numVertexs+graph.edges*2; i++) {
        for(int j = 0; j < graph.edges*2; j++){
            if (i == (j + graph.numVertexs+1)) {
                CoefficientMatrix[i][j] = 1;
                break;
            }
        }
        CoefficientMatrix[i][2*graph.edges] = 0;// ����Լ���ķ���λ��0��ʾС�ڵ���
    }
    
    
    
    
    
    // ������ڷ������㣬������Լ��Ӧ��ɾ�����������������һ�������ࡣ
    for (auto i : graph.service_position)
        for (int j = 0; j < CoefficientMatrix[i].size(); j++)
            CoefficientMatrix[i+1][j] = 0;
    
    
    return CoefficientMatrix;
}

/*
void solveAlgorithm::parseResultOfILP(vector<float>& flowOfEdges,
                                      Graph& graph){
    if (flowOfEdges[0] == 1) {
        vector<int > nodeLink;//��¼ÿһ����·
        vector<int > nodeFlow;//��¼��Ҫ�ı�ıߺŵ�����
        int size_seriv = graph.service_position.size();
        int size_edgs = flowOfEdges.size()-2;   // ������δ֪���ĸ���
        int i = 0;
        int flag = 1;
        int consum;
        //vector<bool > visit(size_edgs, false);



        
        int sum_per_seriv = 0;
        for(size_seriv; size_seriv > 0; size_seriv--, i++){
            //int end = graph.search_consume_position;
            int start = graph.service_position[i];
            for(int k = 1; k <= size_edgs; k++){
                if (graph.indexToEdges[k-1].first == start) {
                    sum_per_seriv += flowOfEdges[k];
                }
            }
            while(sum_per_seriv != 0){//ֻҪ�������ڵ㻹�������������ͼ�������·
                int min_flow =MYINFINITY;
                sum_per_seriv = 0;
                start = graph.service_position[i];
                nodeLink.push_back(start);
                int j = start;
                
                flag = 1;
                while (flag){//ֱ���ҵ����ѽڵ����ӵ�����ڵ�
                    flag = 1;
                    for(int k = 1; k <= size_edgs; k++){
                        if(flowOfEdges[k] != 0){
                            if (graph.indexToEdges[k-1].first == j) {
                                if(min_flow >= flowOfEdges[k] ) {
                                    min_flow = flowOfEdges[k];
                                }
                                j = graph.indexToEdges[k-1].second;
                                for(int ii = 0; ii < graph.numConsumes; ii++ ){
                                    if((j == graph.consume_position[ii].first) &&
                                       (find(graph.service_position.begin(), graph.service_position.end(), j) == graph.service_position.end()) )
                                    {
                                        flag = 0;
                                        consum = ii;
                                        //min_flow = flowOfEdges[k];
                                    }
                                }
                                nodeLink.push_back(j);
                                nodeFlow.push_back(k);
                                break;
                            }
                        }
                    }
                }
                nodeLink.push_back(consum);
                nodeLink.push_back(min_flow);
                graph.result.push_back(nodeLink);
                
                for(int s = 0; s < nodeFlow.size(); s++)
                    flowOfEdges[nodeFlow[s]]-= min_flow;//�ҵ�һ����·���޸���Ӧ�Ľ⣬ֱ���ⶼΪ0
                
                nodeLink.clear();
                nodeFlow.clear();
                for(int k = 1; k <= size_edgs; k++){
                    if (graph.indexToEdges[k-1].first == start) {
                        
                        sum_per_seriv += flowOfEdges[k];
                    }
                }
            }
        }

        for(int i = 0; i < graph.numConsumes; i++){
            for (auto j:graph.service_position){
                if (graph.consume_position[i].first == j)
                    graph.result.push_back(vector<int>{j, i, graph.consume_position[i].second});
            }
        }
    }
    else {
        for(int i = 0; i < graph.numConsumes; i++){
            for (auto j:graph.service_position){
                if (graph.consume_position[i].first == j)
                    graph.result.push_back(vector<int>{j, i, graph.consume_position[i].second});
            }
        }
    }
    
}
*/

void solveAlgorithm::result(const char *filename,  Graph& graph){
    string s;
    //char a = ;
    char *topo_file= new char[1000000];
    char c[10];
    for(int i = 0;i < graph.result.size();i++){
        for(int j = 0; j< graph.result[i].size()-1; j++){
            sprintf(c, "%d ", graph.result[i][j]);
            s += c;
        }
        sprintf(c, "%d", graph.result[i][graph.result[i].size()-1]);
        s += c;
        if(i <graph.result.size()-1){
        sprintf(c, "\n");
        s += c;
        }
    }
    sprintf(c, "%d\n\n", graph.result.size());
    s = c+s;

    
    memcpy(topo_file,s.c_str(),s.length());
    
    write_result(topo_file, filename);
    
    delete [] topo_file;
}

int solveAlgorithm::returnCost(const vector<float>& flowOfEdges) {
    // ������ɹ������ӡ������ѽڵ㣬�����Ӧ�ķ������ڵ㣬�Լ�������������Ҫ��ĸ�ʽ��ӡ
    // ע�⣺�����ɹ���Ҫ����ͼ������Ӧ��������������������
    // ������ɹ�������������������޽⣬�Ҽ���������ѽڵ㣬��������BFS����ȣ�����һ��
    if(flowOfEdges[0] == 1){
        int size = flowOfEdges.size();
        int cost;
        cost = flowOfEdges[size-1];
        
        return cost;
    }
    else
        return MYINFINITY;
}

void solveAlgorithm::BFS_cut(Graph& graph){
    vector<int> servicePositionInConsume2 = BFS1(graph, 2);
    sort(servicePositionInConsume2.begin(), servicePositionInConsume2.end());
    servicePositionInConsume2.erase(unique(servicePositionInConsume2.begin(), servicePositionInConsume2.end()), servicePositionInConsume2.end());
    graph.servicePositionInConsume = servicePositionInConsume2;
    vector<bool> servicePositionToBool(graph.numVertexs, false);
    for(int i=0; i < servicePositionInConsume2.size(); i++){
        servicePositionToBool[servicePositionInConsume2[i]] = 1;
    }
    int hasArcs = 0;
    for(int n = 0;n < graph.numVertexs;n++)
        for(int j = 0;j < graph.numVertexs;j++)
        {
            if( n != j && ( servicePositionToBool[n] != true || servicePositionToBool[j] != true) ) {
                graph.contains[n][j] = MYINFINITY;
                graph.single_price[n][j] = MYINFINITY;
            }
            
            if (graph.contains[n][j] != MYINFINITY)
                hasArcs++;
        }
    graph.numVertexs =  servicePositionInConsume2.size();
    //    for (auto i : visit_node) {
    //        if (i == true)
    //            graph.numVertexs++;
    //    }
    // ����Сͼ������������Сͼ�Ĳ����Ǵӻ�����������������Ҫ�����޸ġ�
    //        graph.numConsumes = 0;     // �������ѵ���Ŀ
    //        graph.consume_position.erase( graph.consume_position.begin(), graph.consume_position.end() );
    //        for (int i=0 ; i < visit_consume.size(); i++) {
    //            if (visit_consume[i] == true){
    //                smallGraph.numConsumes++;
    
    //                auto temp = graph.consume_position[i];      // �洢���ѽڵ���ڵ�value���Դ˸���Сͼ
    
    //                smallGraph.consume_position.insert(std::make_pair(i, temp)); // �������ѽڵ�������ڵ�����ӹ�ϵ
    //            }
    //    }
    
    graph.edges = hasArcs / 2; // ���±ߵ���Ŀ
}



int solveAlgorithm::solver(Graph& graph) {
    
    int costAllConsumes = 0;
    
    //vector<vector<float>> CoefficientMatrix = getRelevanceMatrix(graph);  // ��ȡ��������
    
    // flowOfEdges�ĵ�0λ�Ǳ�־λ����ʾ���ɹ����
    //vector<float> flowOfEdges = linear_programming(CoefficientMatrix);
    //parseResultOfILP(flowOfEdges, graph);
    // �������Թ滮���Ľ������ӡ��ͬʱ����ͼ�������������
    // ����޽⣬Ҳ�����Ӧ��ʾ
    // ����ֵΪ���ۣ�����޽⣬�򷵻�INFINTY
    //costAllConsumes = ZKW::zkw(graph, graph_param);
    costAllConsumes = MCF.solve( graph, graph_param);
    //costAllConsumes = returnCost(flowOfEdges);
    
//    if (costAllConsumes == MYINFINITY)
//        return costAllConsumes;
//    else
//        costAllConsumes = costAllConsumes + graph.service_position.size() * graph.cost_per_service;
    
    return costAllConsumes;
}

vector<int> solveAlgorithm::BFS1( Graph& graph,  int hiearcy)   {
    
    // @�����θ���
    // ��������������������ѵ�������������ڵ�����ڵ㣬�Դ���Ϊ�������ĳ�ʼ��λ��

    
    
    int hiearcy_count=0;//�㼶����
    vector<int> service_position;//���ط�����λ��
    
    
    //partGraph smallGraph(graph, comsume_position, service_position); // ���������������������λ����ʱ����ʼ��
    
    // vector<bool> visit(graph.numVertexs, false);//��¼���ʵĽڵ�
    std::queue<int> Que_hi;
    for(int f=0;f<graph.numConsumes;f++){
        vector<bool> visit(graph.numVertexs, false);
        hiearcy_count=0;
        auto comsume_net = graph.consume_position[f].first;//ͨ�����ѽڵ��������ڵ㣻
        int count = comsume_net;//��¼���ʵĽڵ�
        while ( hiearcy_count < hiearcy ) {//BFS
            //std::cout << hiearcy_count << " < " << hiearcy << std::endl;
            if( !visit[count] ) {
                visit[count] = 1;//���ʹ��ڵ㱣��������ɾ��
                service_position.push_back(count);
                // if ( servicePosition[count] == 1 ) { //�õ��Ƿ�Ϊ�������ڵ�
                hiearcy_count++;
                // cout << "count = " << count << endl;
                //smallGraph.service_position.push_back(count);//��¼�·�����λ��
                //}
                if ( hiearcy_count < hiearcy)
                    Que_hi.push(count);
                while ( !Que_hi.empty() ){
                    //cout << "fff" << endl;
                    auto i =Que_hi.front();
                    Que_hi.pop();
                    
                    for(int m=0;m < graph.numVertexs;m++)
                    {
                        if( graph.contains[i][m] == MYINFINITY )
                            continue;
                        
                        //std::cout << m << std::endl;
                        if( !visit[m] ){
                            service_position.push_back(m);
                            visit[m] = 1; //���ʹ��ڵ㱣��������ɾ��
                            //if( servicePosition[m] == 1 )   // �������Ƿ������ڵ㣬�ʹ�����
                            //{
                            // cout << "m = " << m << endl;
                            //if( hiearcy_count < hiearcy)
                            // smallGraph.service_position.push_back(m);
                            
                            //}
                            if(hiearcy_count < hiearcy)
                                Que_hi.push(m);
                            else
                                break;//tianjia!!!!
                        }
                    }
                    hiearcy_count++;
                    
                }
            }
            // break;
            
            
        }
    }
    
    
    return service_position;
}



//�޸ģ�returnco���Σ���ȥgraph��getRelevanceMatrix���ұ߿���Ϊ����ע���߹��ʼ��A�������Թ滮���������⣻

bool isService(int node,Graph& graph){
    for(int i:graph.service_position)
        if(node==i)
            return true;
    return false;
}
void return_i(int i,vector<int> stack_r,Graph& graph,vector<float> &flowOfEdges){
    stack_r.push_back(i);
    if(isService(i,graph)){
        vector<int> tmp=stack_r;
        for(int i1=tmp.size()-1;i1>2;i1--){
            int t=graph.edgesToIndex[make_pair(tmp[i1],tmp[i1-1])]+1;
            if (flowOfEdges[t] <= 0)
                return;
        }
        if(tmp[0]>graph.consume_position[tmp[1]].second)
            tmp[0]=graph.consume_position[tmp[1]].second;
        for(int i1=tmp.size()-1;i1>2;i1--){
            int t=graph.edgesToIndex[make_pair(tmp[i1],tmp[i1-1])]+1;
            if (tmp[0]>flowOfEdges[t])
                tmp[0]=flowOfEdges[t];
        }
        for(int i1=tmp.size()-1;i1>2;i1--){
            int t=graph.edgesToIndex[make_pair(tmp[i1],tmp[i1-1])]+1;
            //cout<<tmp[i1]<<" ";
            flowOfEdges[t]-=tmp[0];
        }
        graph.consume_position[tmp[1]].second-=tmp[0];
//        cout<<tmp[2]<<" ";
//        cout<<tmp[1]<<" ";
//        cout<<tmp[0]<<" ";
//        cout<<endl;
        vector<int> tmp1;
        for(int i1=tmp.size()-1;i1>=0;i1--){
            tmp1.push_back(tmp[i1]);
        }
        graph.result.push_back(tmp1);
        return;
    }
    
    int k,j=0;
    
    for(int m=0;m<graph.numVertexs;m++){
        //for(auto f=t.begin();f!=t.end();f++){
        if(graph.edgesToIndex.find(make_pair(m,i))!=graph.edgesToIndex.end()){
            if(flowOfEdges[graph.edgesToIndex[make_pair(m,i)]+1]>0){
                if(graph.consume_position[stack_r[1]].second>0){
                    vector<int> stack_r1=stack_r;
                    //                    if(stack_r1[0]>flowOfEdges[graph.edgesToIndex[make_pair(m,i)]+1])
                    //                        stack_r1[0]=flowOfEdges[graph.edgesToIndex[make_pair(m,i)]+1];
                    
                    return_i(m,stack_r1,graph,flowOfEdges);
                }
                
            }
        }
    }
    
    
    
}

void solveAlgorithm::return_text(Graph& graph,vector<float> &flowOfEdges){
    int size_seriv = graph.service_position.size();
    int size_edgs = flowOfEdges.size()-2;
    int size_con=graph.consume_position.size();
    vector<int> stack_re;
    int m,need;
    
    
    for(int i=0;i<size_con;i++){//�ҳ��������ѽڵ�
        stack_re.clear();
        stack_re.push_back(graph.consume_position[i].second);
        m=graph.consume_position[i].first;
        stack_re.push_back(i);
        // stack_re.push_back(m);
        return_i(m,stack_re,graph,flowOfEdges);
    }
    //cout<<endl;
    //  }
}


#include <numeric>
std::vector<int> solveAlgorithm::rand_m(int m,int n) {
    // assume m is 5 and n is 25.
    //int m
    //int n
    
    // initialize numbers.
    std::vector<int> v(n);
    std::iota(v.begin(), v.end(), 1);
    
    // do random shuffle.
    
    // std::random_device rd;
    srand(int(time(NULL)));
    
    std::mt19937 g(rand()%98456862);
    // std::mt19937 g(rd());
    std::shuffle(v.begin(), v.end(), g);
    
    // show first m numbers.
    std::vector<int> j(m);
    std::copy_n(v.begin(), m, j.begin());
    //std::cout << std::endl;
    
    return j;
}

void mergeService(Graph& graph) {
    for (int i = 0; i < graph.service_position.size(); i++) {
        int mindist = MYINFINITY;
        int mergedService1;
        int mergedService2;
        for (int j = 1; j < graph.service_position.size(); j++)
            if (mindist > graph.ShortPathTable[i][j]) {
                mindist = graph.ShortPathTable[i][j];
            }
    }
}

