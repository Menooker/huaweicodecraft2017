//
//  Genome.h
//  
//
//  Created by Mordekaiser on 17/3/14.
//
//
// ע�⣺������ʿ��Դ�һЩ������ÿ�����ŵı���һ��Ҫ��

#ifndef ____Genome__
#define ____Genome__

#include <iostream>
#include <vector>
#include <list>
#include <unordered_map>
#include <string>
#include "Graph.h"

// ������Щ������Ҫ�����޸ģ��ѵ������Ϻ��ʵ�ֵ
const int MAX_INDIVIDUALS_PER_POPULATION = 80;   // һ����Ⱥ�����������ĸ�����Ŀ
const int MAX_ITERATERS = 2500;   // ����������

const int MAX_ITER_TIME = 80;

using std::vector;


class Genome {

public:
    struct Individual {
        
        int _fittness;
        std::list<int> _DNA;
    };

    // �������壬_DNA��Ա����������λ��,������ _fittness��Ա������Ӧ��    
    std::list<Individual> _populations;    // ������Ⱥ
    //vector<Individual> _son_populations; // �Ӵ���Ⱥ, ע�⣬�õ���list���������ɾ��
    
    
    int _generations;
    
    vector<int> _servicePosition;   // ��ѡ������λ�ã���ʼ��ʱ�õ���solveAlgorithm�����getServicePosition_GA()����
    
    int max_service_size;
    
    std::unordered_map<std::string, int> ServiceToCost; // �洢ÿ�������DNA��Ӧ��cost��ÿ�μ��㶼���������´���Ҫֱ��ȡ
    int _numIndividualsFromFather;
    
public:
    float CROSS_PORB = 0.8;     // �������
    float VARIATION_PROB = 0.4;     // �������
    float GET_PERCENTAGE_INDIVIDUAL = 0.5; // ÿ�λ�ȡǰ15%���������

    Genome();
    Individual _BestIndividual; // �������Ÿ���

    
    void GA(Graph& graph); // �Ŵ��㷨����������
    
    void select(Graph& graph); // ѡ�����, ѡ������ĸ���ֱ�Ӹ������Ա�������أ�
    void variation(std::list<Individual>& populations ); // �������
    void intersect(std::list<Individual>& populations); // �������
    void nextPath(vector<int>& BestDNA ,vector<int>& NextDNA, Graph& graph);
    void climbMountain(Graph& graph);
    
    
    // ������Ӧ�Ⱥ�����Ӧ�õ���solveAlgorithm�������solver����
    // ע�⣬����ֵ���ݣ������޸�graph
    void getFittness(Individual& individual, Graph& graph);
    bool isSameIndividual(Individual& individual1, Individual& individual2); // �Ƚ����������Ƿ���ͬ
    
    std::pair<int, int> getRandomSection(const Individual& individual, int max_intersect_length); // ��ȡ�������䣬����Ϊ�ɽ��ܵ���Ľ�������
    
    // ����srcDNAƬ���еĲ���DNA��destDNASec
    void copy_delete_DNASec(std::list<int>& srcDNA, int minPos, int maxPos, std::list<int>& destDNASec);
    
    // ����DNAƬ�εĳ�ͻ
    vector<int> removeDepuInDNA(const vector<int>& firstDNA, const vector<int>& secDNA);
    static bool compare(Individual& individual1, Individual& individual2) {
        return individual1._fittness < individual2._fittness;
        
    }
    
};


#endif /* defined(____Genome__) */
