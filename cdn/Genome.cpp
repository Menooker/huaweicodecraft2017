//
//  Genome.cpp
//  
//
//  Created by Mordekaiser on 17/3/14.
//
//

#include <algorithm>
#include "Genome.h"
#include "MiniCostFlow.h"
#include <random>
#include "solveAlgorithm.h"
#include <memory>


using namespace std;
extern MiniCostFlow MCF;
extern vector<vector<int>> graph_param;
// ���캯��������޸�graph����ӷ�����λ��----------
Genome::Genome() {
    
    _generations = 0;
    
}

void Genome::GA(Graph& graph) {
    // ע�⣬ִ���κ�һ�����������ܵ��¸���DNA�Ĵ�СΪ0��������
    time_t time_start, time_stop;
    time_start = time(nullptr);
    
    max_service_size = graph.NetNodeNeed.size();
    
    int iters = 0;
    int BestCost = MYINFINITY;
    int flag = 0;
    while (iters < MAX_ITERATERS) {
        //cout << "----------------------------" << endl;
        cout << "iters: " << iters << endl;
        //cout << "----------------------------" << endl;
        iters++;

        // ѡ����������ŵĸ��壬Ȼ����У����棬���죬�Ŵ�
        // ѡ��������������Ÿ���ֱ���޸�Genome�ĳ�Ա��������ֵ
        VARIATION_PROB += 0.002;
        select(graph);  // ѡ��ǰ50%����,�洢��_son_populations��Ա��
        
        time_stop = time(nullptr);
        long costtime = time_stop - time_start;
        cout << "cost time : " << costtime << endl;
        cout << "The Best Individual size is " << _BestIndividual._DNA.size() << " : "<< _BestIndividual._fittness << endl << endl;
        
        
        if (_BestIndividual._fittness < BestCost) {
            BestCost = _BestIndividual._fittness;
            flag = 0;
        }
        if (_BestIndividual._fittness == BestCost) {
            flag++;

            if (flag >= 200) {
                graph.service_position.clear();
                for (auto i : _BestIndividual._DNA)
                    graph.service_position.push_back(i);
                cout << "Time is : " << time_stop - time_start << endl;
                break;
            }
        }
        
        
        if (time_stop - time_start > MAX_ITER_TIME || iters >= MAX_ITERATERS) {
            graph.service_position.clear();
            for (auto i : _BestIndividual._DNA)
                graph.service_position.push_back(i);
            cout << "Time is : " << time_stop - time_start << endl;
            break;
        }
        intersect(_populations); // ǰ50%���彻��, ���������DNAƬ�ν���ı�

        variation(_populations);
        
        
    }
}

void Genome::select(Graph& graph) {
    //srand(0);
    if (_generations == 0) {
        _generations++;
        std::list<int> service_connect_consume;
        for (auto ii : graph.NetNodeNeed)
            service_connect_consume.push_back(ii.first);
        
        //random_shuffle(service_connect_consume.begin(), service_connect_consume.end());
        
        // ��ȡ������λ�ã�����������bool�����黻��int������������
        solveAlgorithm tempsol;
//        vector<bool> servicePositionTemp = tempsol.getServicePosition_GA(graph);
//        
//        for (int j = 0; j < servicePositionTemp.size(); j++)
//            if (servicePositionTemp[j] == true)
//                _servicePosition.push_back(j);
        
        
        // ��ʼ�������ѵ���Χ��
        
        vector<int> servicePositionInConsume = tempsol.BFS1(graph, 1);  //��һ���ʼ�������ѵ���Χ�����㣬��һ�������ʼ��
        sort(servicePositionInConsume.begin(), servicePositionInConsume.end());
        servicePositionInConsume.erase(unique(servicePositionInConsume.begin(), servicePositionInConsume.end()), servicePositionInConsume.end());
        
        
        vector<int> servicePositionInConsume2 = tempsol.BFS1(graph, 2); // ��ռ�Ϊ��������������ڵĵ�
        sort(servicePositionInConsume2.begin(), servicePositionInConsume2.end());
        servicePositionInConsume2.erase(unique(servicePositionInConsume2.begin(), servicePositionInConsume2.end()), servicePositionInConsume2.end());
        
        //graph.servicePositionInConsume = _servicePosition;
        //graph.servicePositionInConsume = servicePositionInConsume; // ��ʼ����ռ�
        if (graph.numVertexs < 200)
            _servicePosition = servicePositionInConsume2;   // ��ռ�
        else
            _servicePosition = servicePositionInConsume;
        
        
        _populations = MCF.getInitialPopulation_Improve(graph, graph_param, servicePositionInConsume);  // ��ȡ��ʼ��Ⱥ
        
        int temp = 10;
        _populations.sort(compare);
        auto tempiter1 = _populations.begin();
        while (temp) {
            tempiter1++;
            temp--;
        }
        _populations.erase( tempiter1, _populations.end());
        
        int numService;
        if (graph.numVertexs < 200) {
            numService = 120;
            VARIATION_PROB += 0.3;
        }
        else if (graph.numVertexs < 400) {
            numService = 200;
            VARIATION_PROB += 0.2;
        }
        else
            numService = 400;
        
        
        vector<int> randService = tempsol.rand_m(numService , graph.numVertexs);
        auto tempPopulation2 = MCF.getInitialPopulation_Improve(graph, graph_param, randService);
        temp = 10;
        tempPopulation2.sort(compare);
        auto tempiter2 = tempPopulation2.begin();
        while (temp) {
            tempiter2++;
            temp--;
        }
        tempPopulation2.erase(tempiter2, tempPopulation2.end());
//
//        vector<int> randService3 = tempsol.rand_m(numService , graph.numVertexs);
//        auto tempPopulation3 = MCF.getInitialPopulation_Improve(graph, graph_param, randService3);
//        temp = 5;
//        tempPopulation3.sort(compare);
//        auto tempiter3 = tempPopulation3.begin();
//        while (temp) {
//            tempiter3++;
//            temp--;
//        }
//        tempPopulation3.erase(tempiter3, tempPopulation3.end());
//        
//        _populations = tempPopulation1;
        _populations.merge(tempPopulation2, compare);
//        _populations.merge(tempPopulation3, compare);
        _BestIndividual = *(_populations.begin());
        _numIndividualsFromFather = 20;
        return;
// ��ǰ�ĳ�ʼ��������ֱ��һ���ʼ��
//        zkw_start(graph, graph_param);
//        _populations.sort(compare);
//        _generations++;
//        _numIndividualsFromFather = int(MAX_INDIVIDUALS_PER_POPULATION * GET_PERCENTAGE_INDIVIDUAL);
//        
//        int temp = _numIndividualsFromFather;
//        auto tempiter = _populations.begin();
//        while (temp) {
//            tempiter++;
//            temp--;
//        }
//        _populations.erase(tempiter, _populations.end());
//        _BestIndividual = *(_populations.begin());
//        return;
    }   // ����ǳ���������г�ʼ����Ⱥ
    
    
    // ����֮�������������
    // ������Ⱥ�и����������Ӧ��
    for (auto  temp = _populations.begin(); temp != _populations.end(); temp++) {
        //cout << "Individual " << tempcount++  << " : " << temp->_DNA.size() << endl;
        getFittness(*temp, graph);
        //cout << "costAllConsumes: " << temp->_fittness << endl << endl;
        //sumOfFittness += _populations.at(i)._fittness;
    }
    
    // Ѱ�Ҵ���ǰ20%�ĸ��壨����ԽС��Խǰ��
    _populations.sort(compare);
    
    _BestIndividual = *(_populations.begin());

}// ѡ�����



void Genome::intersect(list<Individual>& populations) {
    
    auto fatherBegin = populations.begin();
    auto fatherEnd = populations.end();
    fatherEnd--;

    int numOfIntersect = MAX_INDIVIDUALS_PER_POPULATION/2 - 1;
    while (numOfIntersect) {
        numOfIntersect--;
        int changePos1 = rand() % (_numIndividualsFromFather);
        int changePos2 = rand() % (_numIndividualsFromFather);
        auto firstIter = populations.begin();   // ָ��Ҫ�����ĵ�һ������

        while (changePos1) {
            firstIter++;
            changePos1--;
        }
        auto secondIter = populations.begin();   // ָ��Ҫ�����ĵڶ�������
        while (changePos2) {
            secondIter++;
            changePos2--;
        }
        
        Individual individual1 = *firstIter;
    
        Individual individual2 = *secondIter;


        if ( (rand() % 1000)/1000.0 > CROSS_PORB ) {   // �������Ϊ20%
            
            _populations.push_back(individual1);
            _populations.push_back(individual2);
            continue;
        }
        // ��󽻲��СΪDNA���ȵ�1/3, ��С�����СΪ0����������
        pair<int, int> intersec1 = getRandomSection(individual1, individual1._DNA.size() * 1 / 3 );
        pair<int, int> intersec2 = getRandomSection(individual2, individual2._DNA.size() * 1 / 3 );
        
        // ѡȡDNAƬ��,ע�⣬�����ɾ��divididual1�Ĳ���Ԫ�أ�����Ϊ�˷�����潻�����
        list<int> firstSec;
        copy_delete_DNASec(individual1._DNA, intersec1.first, intersec1.second, firstSec);
        list<int> secondSec;
        copy_delete_DNASec(individual2._DNA, intersec2.first, intersec2.second, secondSec);
        
        for (auto i = firstSec.begin(); i != firstSec.end(); ) {
            if (find(secondIter->_DNA.begin(), secondIter->_DNA.end(), *i) != secondIter->_DNA.end() ) {
                // ���firstSec����ĵ�i��Ԫ����*secondIter���棬��˵���ǳ�ͻƬ�Σ�Ӧ��ɾ��
                firstSec.erase(i++);
            }
            else
                i++;
        }
        
        for (auto i = secondSec.begin(); i != secondSec.end(); ) {
            if (find(firstIter->_DNA.begin(), firstIter->_DNA.end(), *i) != firstIter->_DNA.end() ) {
                // ���secondSec����ĵ�i��Ԫ����*firstIter���棬��˵���ǳ�ͻƬ�Σ�Ӧ��ɾ��
                secondSec.erase(i++);
            }
            else
                i++;
        }


        
        // DNA���棬��ɾ��ԭ��Ƭ��
        for (auto i : firstSec) {
            if (individual2._DNA.size() >= max_service_size)
                break;
            individual1._DNA.erase( find(individual1._DNA.begin(), individual1._DNA.end(), i) );
            individual2._DNA.push_back(i);
        }
        for (auto i : secondSec) {// ��individual1���Ҳ���secondSec��Ԫ�أ���push_back
            if (individual1._DNA.size() >= max_service_size)
                break;
            individual2._DNA.erase( find(individual2._DNA.begin(), individual2._DNA.end(), i) );
            individual1._DNA.push_back(i);
        }
        
        populations.push_back(individual1);
        populations.push_back(individual2);
    }
    
    
    populations.push_back(*fatherBegin);
    fatherBegin++;
    populations.push_back(*fatherBegin);
    // ɾ��ǰ��ĸ�����Ԫ��
    fatherBegin--;
    populations.erase(fatherBegin, ++fatherEnd);
    
    return;
    
}   // �����㷨

// �����㷨
void Genome::variation(list<Individual>& populations) {
    
    /*
    for(auto i = populations.begin(); i != populations.end(); i++)
    {
        double ran=rand()%1000/1000.0;
        if(ran>= VARIATION_PROB)
            continue;
         
        int newDNA;
        while(1)
        {
            newDNA=_servicePosition[ rand()%_servicePosition.size() ];
            if( find(i->_DNA.begin(), i->_DNA.end(), newDNA) == i->_DNA.end())
                break;  // �Ҳ�����˵�����ǳ�ͻ����
            // ֱ�Ӱѱ�������ĳ����������ʼ�����߻���ã�
        }
        
        // �ҳ�����Ԫ��λ��
        int varPos = rand() % i->_DNA.size();
        auto deletedPos = i->_DNA.begin();
        while (varPos) {
            deletedPos++;
            varPos--;
        }
        *deletedPos = newDNA;  // �滻����Ԫ��
    }
    */
    
    int numOfVaritions = MAX_INDIVIDUALS_PER_POPULATION - 2;//�������������Ĳ���
    auto individual = populations.begin();
    while (numOfVaritions) {
        
        numOfVaritions--;
        if ( (rand() % 1000)/1000.0 > VARIATION_PROB)  {   //�������Ϊ0.6
            continue;
        }
    
        int flag = rand() % 3;
        if (flag == 2) { // flagΪ1��ʾ�������Ϊ������ӷ�����
            // ���ӵķ����������������10 ��ֹ����̫��
            int increased_service = 1;
            int NumDNA = 0;
            while (NumDNA < increased_service) {
                int newDNASegment = _servicePosition[ rand() % _servicePosition.size() ];
                if (find(individual->_DNA.begin(), individual->_DNA.end(), newDNASegment) == individual->_DNA.end()) {
                    individual->_DNA.push_back(newDNASegment);
                    //cout << "increase: " << newDNASegment << endl;
                    NumDNA++;
                }
            }
        }
        
        else if (flag == 0) {  // flagΪ0����������Ϊ������ٷ�����
            int decreased_service;
        
                // ����
            decreased_service  = 1;
            
            
            int NumDNA = 0;
            
            while (NumDNA < decreased_service) {
                int decreasedPos = rand() % individual->_DNA.size();
                auto decreasedIter = individual->_DNA.begin();
                while(decreasedPos) {
                    decreasedIter++;
                    decreasedPos--;
                }
                individual->_DNA.erase(decreasedIter);
                NumDNA++;
            }
            
        }
        
        else {   // flagΪ2,�����ͻ�䣬�����ѡȡ���ַ�����λ�ý�������滻���������Ϊ�ʵ��ļ����Ŷ�
            int numOfChange = 1;// �ı�ķ��������������ڵ�1 / 4
            int NumDNA = 0;
            int changePosition;
            int changeSerive;
            while (NumDNA < numOfChange) {
                changePosition = rand() % individual->_DNA.size();
                
                changeSerive = _servicePosition [rand() % _servicePosition.size()];
                auto changeIter = individual->_DNA.begin();
                while(changePosition) {
                    changeIter++;
                    changePosition--;
                }
                *changeIter = changeSerive;
                NumDNA++;
            }
        }
        individual++;
    }
    
    return;
}

void Genome::climbMountain(Graph& graph) {
    time_t time_start, time_stop;
    time_start = time(nullptr);
    
    int BestCost;
    
    int iters = 0;
    graph.service_position.clear();
    for (auto temp : graph.NetNodeNeed)
        graph.service_position.push_back(temp.first);
    vector<int> BestDNA;
    BestDNA = graph.service_position;
    BestCost = graph.service_position.size() * graph.cost_per_service;
    
    while (iters < MAX_ITERATERS) {
        cout << "iters: " << iters << endl;
        iters++;
        
        vector<int> nextDNA;
        
        nextPath(BestDNA, nextDNA, graph);
        graph.service_position = nextDNA;
        
        int cost = MCF.solve(graph, graph_param);
        if (cost < BestCost) {
            BestCost = cost;
            BestDNA = nextDNA;
        }
        cout << "Best cost is " << BestCost << endl << endl;
    }
    
    time_stop = time(nullptr);
    int costtime = time_stop - time_start;
    cout << "cost time : " << costtime << endl;
    
    graph.service_position = BestDNA;
    
}

void Genome::nextPath(vector<int>& BestDNA ,vector<int>& NextDNA, Graph& graph){
    // ˼·������99.6%��Ԫ�أ�Ȼ���������Ԫ�أ������滻ʣ��Ԫ�ػ���ֱ�Ӷ���ʣ��Ԫ�أ����߶�����
    // �ں��������Ԫ��
    int k=0,i,j;
    for(i=0;i < BestDNA.size();++i)    // ����DNA�����ÿһ��Ԫ��
    {
        double ran = rand()%1000/1000.0;;
        if(ran<0.996)
        {
            NextDNA.push_back(BestDNA[i]) ;
            k++;
            continue;
        }
    }
    int numVarition=rand()%2;
    for(i=k;i<k+numVarition;++i)
    {
        bool hasSame = false;
        int randService = rand()% graph.numVertexs;
        for(j=0;j<i;++j)
        {
            if (randService == BestDNA[j]) {
                hasSame = true;
                break;
            }
        }
        
        if (hasSame)
            continue;
        NextDNA.push_back(randService);
    }
}
    


// graph����ֵ����,��Ϊÿ������һ���������Ӧ�Ⱥ�graph���뻹ԭ
void Genome::getFittness(Individual& individual,Graph& graph) {
    graph.service_position.clear();
    for (auto i : individual._DNA)
        graph.service_position.push_back(i);
    solveAlgorithm tempsol;
    
    std::ostringstream buf;
    sort(graph.service_position.begin(), graph.service_position.end());
    for (auto i : graph.service_position)
        buf << i << "-1";//ÿ��������λ����-1�����������ͱ������ظ�����
    
    if (ServiceToCost.find(buf.str()) != ServiceToCost.end()) {
        int Fittness = ServiceToCost[buf.str()];
        individual._fittness = Fittness;
        return;
    }
    
    individual._fittness =  tempsol.solver(graph);
    ServiceToCost[buf.str()] = individual._fittness;
    //cout << "costAllConsumes: " << individual._fittness << endl << endl;
   
    return;
}


// ����DNAƬ�εĳ�ͻ
vector<int> removeDepuInDNA(const vector<int>& firstDNA, const vector<int>& secDNA) {
    vector<int> result;
    for (size_t i = 0; i < firstDNA.size(); i++)
        if (find(secDNA.begin(), secDNA.end(), firstDNA.at(i)) == secDNA.end() )
            // ���firstDNA����ĵ�i��Ԫ�ز���secDNA���棬��˵�����ǳ�ͻƬ�Σ�Ӧ�ñ���
            result.push_back(firstDNA.at(i));

    return result;
}


void Genome::copy_delete_DNASec(list<int>& srcDNA, int minPos, int maxPos, list<int>& destDNASec) {
    int length = maxPos - minPos;
    if (length == 0)
        return;
    auto tempiter = srcDNA.begin();
    while (minPos) {
        tempiter++;
        minPos--;
    }
    
    while (length ) {
        destDNASec.push_back(*tempiter);
        tempiter++;
        length--;
    }
}


std::pair<int, int> Genome::getRandomSection(const Individual& individual ,int max_intersect_length){
    // �������䲻�ܳ���DNA�ĳ���
    //srand(0);
    int minPos = rand() % individual._DNA.size();
    int maxPos = rand() % individual._DNA.size();
    if (maxPos < minPos)
        swap(maxPos, minPos);
    
    // ��֤������˵�С���Ҷ˵�, �ҳ��Ȳ��ܳ�����󽻲泤��
    while ((maxPos - minPos) > max_intersect_length && (maxPos - minPos) < 0 ) {
        maxPos = rand() % individual._DNA.size();
        if (maxPos < minPos)
            swap(maxPos, minPos);
    }
    
    return make_pair(minPos, maxPos);
} // ��ȡ�������䣬����Ϊ�ɽ��ܵ���Ľ�������

bool Genome::isSameIndividual(Individual& individual1, Individual& individual2) {
    if (individual1._DNA.size() != individual2._DNA.size())
        return false;
    auto iter1 = individual1._DNA.begin();
    auto iter2 = individual2._DNA.begin();;
    for (; iter1 != individual1._DNA.end(); iter1++, iter2++)
        if (*iter1 != *iter2)
            return false;
    
    return true;
}
