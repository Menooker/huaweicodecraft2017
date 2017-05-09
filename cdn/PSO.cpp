//
//  PSO.cpp
//  
//
//  Created by Mordekaiser on 17/4/5.
//
//


/*
 * ʹ��C����ʵ������Ⱥ�㷨(PSO)
 * �ο��ԡ�MATLAB�����㷨30������������
 * update: 16/12/3
 * ������Ѱ�ŷ����Ժ���Ϊ
 * f(x,y) = sin(sqrt(x^2+y^2))/(sqrt(x^2+y^2)) + exp((cos(2*PI*x)+cos(2*PI*y))/2) - 2.71289
 * �ú����кܶ�ֲ�����ֵ�㣬������λ��Ϊ(0,0),��(0,0)����ȡ�ü���ֵ
 */
#include "PSO.h"
#include "MiniCostFlow.h"
#include<iostream>


#define c1 2 //���ٶ�����һ���Ǹ��ݴ���ʵ������
#define c2 2
#define dim 10000
#define maxgen 300  // ��������
#define sizepop 100 // ��Ⱥ��ģ
int ParticleDim,num=0;  // ���ӵ�ά��
int popmax;  // �������ȡֵ
int popmin;  // ������Сȡֵ
float Vmax;  // �ٶ����ֵ
float Vmin;  //�ٶ���Сֵ
float Vmax1;  // �ٶ����ֵ
float Vmin1;  //�ٶ���Сֵ
int pop[sizepop][dim]; // ������Ⱥ����
float V[sizepop][dim]; // ������Ⱥ�ٶ�����
int fitness[sizepop]; // ������Ⱥ����Ӧ������
int result[maxgen];  //������ÿ�ε�����Ⱥ����ֵ������
int pbest[sizepop][dim];  // ���弫ֵ��λ��
int gbest[dim]; //Ⱥ�弫ֵ��λ��
int fitnesspbest[sizepop]; //���弫ֵ��Ӧ�ȵ�ֵ
int fitnessgbest; // Ⱥ�弫ֵ��Ӧ��ֵ
int genbest[maxgen][dim]; //ÿһ������ֵȡֵ����


using namespace std;



extern MiniCostFlow MCF;
extern vector<vector<int>> graph_param;
//��Ӧ�Ⱥ���
int getFittness(int x[],Graph& graph)
{
    //x�Ƿ�������λ��
    graph.service_position.clear();
    for (int i = 0; i < ParticleDim; i++)
        if (x[i] == 1)
            graph.service_position.push_back(i);
    
    return -MCF.solve(graph, graph_param);
    
    
}

// ��Ⱥ��ʼ��
void pop_init(Graph& graph)
{
    for(int i=0;i<sizepop;i++)
    {
        int service_size = (rand() % graph.NetNodeNeed.size()) + 1;
        int randService;
        while (service_size) {
            randService = rand() % ParticleDim;
            if (pop[i][randService] == 1)
                continue;
            pop[i][randService] = 1;
            V[i][randService] =(((double)rand())/RAND_MAX-0.5); //-0.5��0.5֮��
            service_size--;
        }
        
        fitness[i] = getFittness(pop[i],graph); //������Ӧ�Ⱥ���ֵ
    }
    
    int * best_fit_index; // ���ڴ��Ⱥ�弫ֵ����λ��(���)
    best_fit_index = max(fitness,sizepop); //��Ⱥ�弫ֵ
    int index = *best_fit_index;
    // Ⱥ�弫ֵλ��
    for(int i=0;i<ParticleDim;i++)
    {
        gbest[i] = pop[index][i];
    }
    // ���弫ֵλ��
    for(int i=0;i<sizepop;i++)
    {
        for(int j=0;j<ParticleDim;j++)
        {
            pbest[i][j] = pop[i][j];
        }
    }
    // ���弫ֵ��Ӧ��ֵ
    for(int i=0;i<sizepop;i++)
    {
        fitnesspbest[i] = fitness[i];
    }
    //Ⱥ�弫ֵ��Ӧ��ֵ
    int bestfitness = *(best_fit_index+1);
    fitnessgbest = bestfitness;
    
}

// max()��������
int* max(int * fit,int size)
{
    int index = 0; // ��ʼ�����
    int max = *fit; // ��ʼ�����ֵΪ�����һ��Ԫ��
    static int best_fit_index[2];
    for(int i=1;i<size;i++)
    {
        if(*(fit+i) > max) {
            max = *(fit+i);
            index = i;
        }
    }
    best_fit_index[0] = index;
    best_fit_index[1] = max;
    return best_fit_index;
    
}

// ����Ѱ��
void PSO_func(Graph& graph)
{
    pop_init(graph);

    //����Ѱ��
    double temp;
    for(int i=0;i<maxgen;i++)
    {
        for(int j=0;j<sizepop;j++)
        {
            //�ٶȸ��¼����Ӹ���
            for(int k=0;k<ParticleDim;k++)
            {
                // �ٶȸ���
                float rand1 = (float)rand()/RAND_MAX; //0��1֮��������
                float rand2 = (float)rand()/RAND_MAX;
                V[j][k] = 0.729 * V[j][k] +( c1*rand1*(pbest[j][k]-pop[j][k])) + (c2*rand2*(gbest[k]-pop[j][k]));
                
                // ÿ��ά�ȵĸ��¼���������0.268~0.731֮��
                if (V[j][k] > 1)
                    V[j][k] = 1;
                if (V[j][k] < -1)
                    V[j][k] = -1;
                
                temp =  1.0/(1 + exp(-V[j][k]));
                float randtemp = (float)rand()/RAND_MAX;
                if (randtemp < temp)
                    pop[j][k] = 1;
                else
                    pop[j][k] = 0;
   
                // ���Ӹ���
                /*
                pop[j][k] = pop[j][k] + V[j][k];
                if(pop[j][k] > popmax)
                    pop[j][k] = popmax;
                if(pop[j][k] < popmin)
                    pop[j][k] = popmin;
                 */
            }
            
            fitness[j] = getFittness(pop[j],graph); //�����ӵ���Ӧ��ֵ
            // std::cout<<std::endl<<"I:"<<num++<<std::endl;
        }
        for(int j=0;j<sizepop;j++)
        {
            // ���弫ֵ����
            if(fitness[j] > fitnesspbest[j])
            {
                for(int k=0;k<ParticleDim;k++)
                {
                    pbest[j][k] = pop[j][k];
                }
                fitnesspbest[j] = fitness[j];
            }
            // Ⱥ�弫ֵ����
            if(fitness[j] > fitnessgbest)
            {
                for(int k=0;k<ParticleDim;k++)
                    gbest[k] = pop[j][k];
                fitnessgbest = fitness[j];
            }
        }
        for(int k=0;k<ParticleDim;k++)
        {
            genbest[i][k] = gbest[k]; // ÿһ������ֵȡֵ����λ�ü�¼
        }
        result[i] = fitnessgbest; // ÿ��������ֵ��¼������
        std::cout << "iters " << i << " :  " << -fitnessgbest << std::endl;
    }
}

// ������
void optimize(Graph& graph)

{
    
    ParticleDim = graph.numVertexs;  // ���ӵ�ά��
    popmax=1;  // �������ȡֵ
    popmin=0;  // ������Сȡֵ
    Vmax=1;  // �ٶ����ֵ
    Vmin=-1;  //�ٶ���Сֵ
    //     Vmax1=2;  // �ٶ����ֵ
    //     Vmin1=-2;  //�ٶ���Сֵ
    // clock_t start,finish; //����ʼ�ͽ���ʱ��
    //start = clock(); //��ʼ��ʱ
    //srand((unsigned)time(nullptr)); // ��ʼ�����������
    PSO_func(graph);
    int * best_arr;
    best_arr = max(result,maxgen);
    int best_gen_number = *best_arr; // ����ֵ�����Ĵ���
    int best = *(best_arr+1); //����ֵ
    printf("%d    %d     :%d\n",maxgen,best_gen_number+1,-best);
    //    printf("ȡ������ֵ��λ��Ϊ(%lf,%lf).\n",genbest[best_gen_number][0],genbest[best_gen_number][1]);
    // finish = clock(); //����ʱ��
    // int duration = (int)(finish - start)/CLOCKS_PER_SEC; // ��������ʱ��
    //  printf("�������к�ʱ:%lf\n",duration);
    //return 0;
}
