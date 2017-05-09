#include "deploy.h"
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <queue>
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <ctime>
#include <map>
#include <string>

const int INF = 0x3f3f3f3f;
const int MAX_CON_NUM = 510;
const int MAX_NET_NUM = 1010;

using namespace std;

struct shortestPath
{
	vector <int> pat;
	int costToConsumer;
	int flowOfPath;
};
struct pathAndFlow
{
	vector<int> path1;
	int flow1;
	int cost1;
};
struct ReturnPath
{
	struct shortestPath;
	int Order1;
};

const int MAX = 99999;
int cost[MAX_NET_NUM][MAX_NET_NUM];//cost矩阵怎么定义。
int capacity[MAX_NET_NUM][MAX_NET_NUM];
int flow[MAX_NET_NUM][MAX_NET_NUM];
int connectToConsumer[MAX_CON_NUM];
bool is_con[MAX_NET_NUM];
map<int, int> connectToVertex;
int consumerRequest[MAX_CON_NUM];
int consumerAcheive[MAX_CON_NUM];
int consumerCost[MAX_CON_NUM];
vector<vector<pathAndFlow> > consumerPath;
vector<int > severPos;
vector<int > noServers;
int costTotal = 0, consumerNum = 0, edgeNum = 0;
bool minFlag[MAX_CON_NUM];
bool maxFlag[MAX_CON_NUM];
string str_res;
//临界表
vector<vector<int> > LinkedVec;

vector<vector<int> > res;//这里存路径，但是不区分是哪个消费节点的。
bool done[MAX_NET_NUM];
int d[MAX_NET_NUM];
int bw[MAX_NET_NUM];
bool vis[MAX_NET_NUM];
int pre[MAX_NET_NUM];
int netNum, serverCost, serverNum;
int net2con[MAX_NET_NUM], con2net[MAX_NET_NUM], consumers_need[MAX_NET_NUM];
int isCon[MAX_NET_NUM];
bool isSer[MAX_NET_NUM];
clock_t start, finish;
vector<int> G[MAX_NET_NUM];
int head[MAX_NET_NUM];
int edge_count;
int path[MAX_NET_NUM];
int first_cost, second_cost, best_cost;
vector<int> consumers, servers, oth_servers;

struct Edge
{
	int from;
	int to;
	int bw; //bandwidth
	int real_bw;
	int cost;
	__OPT
	Edge(int from, int to, int bw, int cost) :from(from), to(to), bw(bw), real_bw(bw), cost(cost) {}
};

struct Edge_MCMF
{
	int to, next, cap, flow, cost;
	//Edge_MCMF(int u, int v, int cap, int cost, int next):u(u), v(v), cap(cap), cost(cost), next(next) {}
};

struct Node
{
	int d, from;
	__OPT
	Node(int d, int from) :d(d), from(from) {}
	__OPT
	Node() {}
	__OPT
	bool operator < (const Node &rhs) const
	{
		return d > rhs.d;
	}
};

struct ConNode
{
	int connode;
	int netnode;
	int need;
	int real_need;
	bool is_server;
	double unit_bw;
	__OPT
	ConNode(int connode, int netnode, int need) :connode(connode), netnode(netnode), need(need), real_need(need)
	{
		is_server = 0;
		unit_bw = 1.0*serverCost / real_need;
	}
	__OPT
	bool operator < (const ConNode& cn) const
	{
		return real_need > cn.real_need;
	}

};

struct Distance
{
	int pos;
	int d;
	Distance(int pos, int d) :pos(pos), d(d) {}
	__OPT
	bool operator < (const Distance& dis) const
	{
		return d > dis.d;
	}
};

struct Min_Path
{
	vector<int> mp;
	int pos;
	int d;
	__OPT
	Min_Path(vector<int> mp, int d, int pos) :mp(mp), d(d), pos(pos) {}
};

vector<Edge> edges;
vector<ConNode> con_nodes;
Edge_MCMF edge_mcmf[MAX_EDGE_NUM << 1];
double _begin, _end, step;
//遗传
struct Chrom
{
	vector<int> bit;
	int fit;
	int servCount;
	double rfit;
	double cfit;
};
vector<Chrom> popCurrent(MAX_NET_NUM, Chrom());
vector<Chrom> popNext(MAX_NET_NUM, Chrom());
int popSize;
int MaxServerNum;
int BestIndivi;
//给网络节点分圈
//最外圈是connectToConsumer
vector<int> VertexFirst;
vector<int> VertexSecond;
vector<int> VertexThird;
__OPT
void add_edge_mcmf(int u, int v, int cap, int cost)
{
	edge_mcmf[edge_count].to = v;
	edge_mcmf[edge_count].cap = cap;
	edge_mcmf[edge_count].cost = cost;
	edge_mcmf[edge_count].flow = 0;
	edge_mcmf[edge_count].next = head[u];
	head[u] = edge_count++;
	edge_mcmf[edge_count].to = u;
	edge_mcmf[edge_count].cap = 0;
	edge_mcmf[edge_count].cost = -cost;
	edge_mcmf[edge_count].flow = 0;
	edge_mcmf[edge_count].next = head[v];
	head[v] = edge_count++;
}
__OPT
bool spfa(int s, int t, int n)
{
	deque<int> q;
	for (int i = 0; i < n; i++)
	{
		d[i] = INF;
		vis[i] = false;
		pre[i] = -1;
	}
	d[s] = 0;
	vis[s] = true;
	q.push_back(s);
	while (!q.empty())
	{
		int u = q.front();
		q.pop_front();
		vis[u] = false;
		for (int i = head[u]; i != -1; i = edge_mcmf[i].next)
		{
			int v = edge_mcmf[i].to;
			if (edge_mcmf[i].cap > edge_mcmf[i].flow &&
				d[v] > d[u] + edge_mcmf[i].cost)
			{
				d[v] = d[u] + edge_mcmf[i].cost;
				pre[v] = i;
				if (!vis[v])
				{
					vis[v] = true;
					if (!q.empty())
					{
						if (d[v] > d[q.front()])
							q.push_back(v);
						else
							q.push_front(v);
					}
					else
						q.push_back(v);
				}
			}
		}
	}
	if (pre[t] == -1) return false;
	else return true;
}

__OPT
int mcmf(int s, int t, int n)
{
	int flow = 0;
	int cost = 0;
	while (spfa(s, t, n))
	{
		int Min = INF;
		for (int i = pre[t]; i != -1; i = pre[edge_mcmf[i ^ 1].to])
		{
			if (Min > edge_mcmf[i].cap - edge_mcmf[i].flow)
				Min = edge_mcmf[i].cap - edge_mcmf[i].flow;
		}
		for (int i = pre[t]; i != -1; i = pre[edge_mcmf[i ^ 1].to])
		{
			edge_mcmf[i].flow += Min;
			edge_mcmf[i ^ 1].flow -= Min;
			cost += edge_mcmf[i].cost * Min;
		}
		flow += Min;
	}
	int part_need = 0;
	for (int i = 0; i < consumers.size(); i++)
		part_need += consumers_need[consumers[i]];
	if (part_need != flow)
		cost = -1;
	return cost;
}
__OPT
int mcmf2(int s, int t, int n)
{
	int flow = 0;
	int cost = 0;
	while (spfa(s, t, n))
	{
		int Min = MAX;
		for (int i = pre[t]; i != -1; i = pre[edge_mcmf[i ^ 1].to])
		{
			if (Min > edge_mcmf[i].cap - edge_mcmf[i].flow)
				Min = edge_mcmf[i].cap - edge_mcmf[i].flow;
		}
		for (int i = pre[t]; i != -1; i = pre[edge_mcmf[i ^ 1].to])
		{
			edge_mcmf[i].flow += Min;
			edge_mcmf[i ^ 1].flow -= Min;
			cost += edge_mcmf[i].cost * Min;
		}
		flow += Min;
	}
	int request = 0;
	for (int m = 0; m<noServers.size(); m++)
	{
		request += consumerRequest[noServers[m]];
	}
	if (request != flow)
		return MAX;
	return cost;
}
__OPT
int get_path_mcmf(int s, int now_path_pos, int min_bw)
{
	if (s == netNum + 1)
	{
		vector<int> p;
		for (int i = 1; i < now_path_pos - 1; i++)
			p.push_back(path[i] - 1);
		p.push_back(net2con[path[now_path_pos - 2] - 1]);
		p.push_back(min_bw);
		res.push_back(p);
		return min_bw;
	}
	int res_bw = 0;
	for (int i = head[s]; i != -1; i = edge_mcmf[i].next)
	{
		if (vis[edge_mcmf[i].to] || edge_mcmf[i].flow <= 0) continue;

		path[now_path_pos] = edge_mcmf[i].to;
		vis[edge_mcmf[i].to] = 1;
		int tmp_bw = get_path_mcmf(edge_mcmf[i].to, now_path_pos + 1, min(min_bw - res_bw, edge_mcmf[i].flow));
		vis[edge_mcmf[i].to] = 0;
		edge_mcmf[i].flow -= tmp_bw;
		res_bw += tmp_bw;
		if (min_bw <= res_bw) break;
	}
	return res_bw;
}
__OPT
void boundary()
{
	puts("----------------------------------------");
}
__OPT
void InsertConsumEdge(int v1, int v2, int req)
{
	consumerRequest[v1] = req;
	connectToConsumer[v1] = v2;
	connectToVertex[v2] = v1;
}
__OPT
void InsertEdge(int v1, int v2, int ca, int co)
{
	cost[v1][v2] = co;
	cost[v2][v1] = co;
	capacity[v1][v2] = ca;
	capacity[v2][v1] = ca;
	LinkedVec[v1].push_back(v2);
	LinkedVec[v2].push_back(v1);

}
__OPT
//You need to complete the function
void OutPutResult(char * filename)
{
	/*	str_res.clear();
	char a[20];
	int count = 0;
	for(int i = 0;i<consumerNum;i++)
	{
	count += consumerPath[i].size();
	if(consumerPath[i].size() == 0)
	count++;

	}
	cout<<endl;
	sprintf(a, "%d\n\n",count);
	str_res = a;
	for(int i = 0; i < consumerNum; i++)
	{

	if(consumerPath[i].size() == 0)
	{
	if(maxFlag[i] == true)
	{
	count--;
	sprintf(a, "%d", connectToConsumer[i]);
	str_res += a;
	sprintf(a, " %d", i);
	str_res += a;
	if(count == 0)
	{
	sprintf(a, " %d", consumerRequest[i]);
	}
	else
	{
	sprintf(a, " %d\n", consumerRequest[i]);
	}
	str_res += a;

	}
	}
	else
	{

	for(int j = 0; j < consumerPath[i].size(); j++)
	{
	count--;
	sprintf(a, "%d", consumerPath[i][j].path1[0]);
	str_res += a;
	int mm = 0;
	int k = 1;
	for(;k<consumerPath[i][j].path1.size();k++)
	{
	sprintf(a, " %d", consumerPath[i][j].path1[k]);
	str_res += a;
	mm = consumerPath[i][j].flow1;

	}
	sprintf(a, " %d", i);
	str_res += a;
	if(k != 1)
	{

	if(count == 0)
	{
	sprintf(a," %d",mm);
	str_res += a;
	}
	else
	{
	sprintf(a," %d\n",mm);
	str_res += a;
	}

	}



	}
	}

	}*/
	str_res.clear();
	char a[20];
	sprintf(a, "%d\n\n", res.size());
	str_res = a;

	for (int i = 0; i < res.size(); i++)
	{
		sprintf(a, "%d", res[i][0]);
		str_res += a;
		for (int j = 1; j < res[i].size(); j++)
		{
			sprintf(a, " %d", res[i][j]);
			str_res += a;
		}
		if (i != res.size() - 1)
		{
			str_res += "\n";
		}
	}
	char * topo_file = (char *)str_res.c_str();
	cout << str_res;
	write_result(topo_file, filename);

}
__OPT
void BuildGraph(char *topo[MAX_EDGE_NUM], int line_num)
{
	char *ch;
	int spaceNum = 0;
	int verNode, vSrc, vDes, cost, bandwidth, req, vConsu;

	for (int i = 1; i < consumerNum + 1; i++)
	{
		ch = topo[line_num - i];
		//  verNode = need = spaceCount = 0;
		verNode = req = spaceNum = 0;
		while (*ch != '\0' && *ch != '\n' && *ch != '\r')
		{
			if (*ch == ' ')
			{
				ch++;
				spaceNum++;
				continue;
			}
			if (spaceNum == 1)
			{
				verNode = *ch - '0' + verNode * 10;
			}
			else if (spaceNum == 2)
			{
				req = *ch - '0' + req * 10;
			}
			ch++;
		}
		InsertConsumEdge(consumerNum - i, verNode, req);
	}

	for (int i = line_num - consumerNum - 2;; i--)
	{
		ch = topo[i];
		if (*ch == '\n' || *ch == '\r')
			break;
		vSrc = vDes = bandwidth = spaceNum = cost = 0;
		while (*ch != '\0' && *ch != '\n' && *ch != '\r')
		{
			if (*ch == ' ')
			{
				ch++;
				spaceNum++;
				continue;
			}
			if (spaceNum == 1)
			{
				vDes = *ch - '0' + vDes * 10;
			}
			else if (spaceNum == 2)
			{
				bandwidth = *ch - '0' + bandwidth * 10;
			}
			else if (spaceNum == 3)
			{
				cost = *ch - '0' + cost * 10;
			}
			else if (spaceNum == 0)
			{
				vSrc = *ch - '0' + vSrc * 10;
			}
			ch++;
		}
		InsertEdge(vSrc, vDes, bandwidth, cost);
	}
}
__OPT
int PathToFlow(vector<int> v)
{
	int flowTemp = MAX;
	for (int j = 0; j<v.size() - 1; j++)
	{
		if (capacity[v[j]][v[j + 1]]>flow[v[j]][v[j + 1]] &&
			capacity[v[j]][v[j + 1]] - flow[v[j]][v[j + 1]]<flowTemp)
			flowTemp = capacity[v[j]][v[j + 1]] - flow[v[j]][v[j + 1]];
	}
	if (flowTemp == MAX)
		return 0;
	else
		return flowTemp;
}
__OPT
shortestPath Dijkstra(int v0, int v1)
{
	shortestPath result;
	int bandWidth[MAX_NET_NUM];
	int parent[MAX_NET_NUM];
	bool visited11[MAX_NET_NUM];
	int dist[MAX_NET_NUM];
	for (int i = 0; i<netNum; i++)
	{
		bandWidth[i] = MAX;
		visited11[i] = false;
		dist[i] = MAX;
		parent[i] = -1;
	}
	priority_queue<Node>q;
	dist[v0] = 0;
	Node temp;
	temp.from = v0;
	temp.d = 0;
	q.push(temp);
	while (!q.empty())
	{
		Node cd = q.top();
		int u = cd.from;
		//cout<<"top"<<u<<endl;
		q.pop();
		if (visited11[u])
			continue;
		visited11[u] = true;
		for (int m = 0; m<LinkedVec[u].size(); m++)
		{
			int v;
			v = LinkedVec[u][m];//邻接点
			//cout<<v<<endl;
			if (visited11[v] || v == u || capacity[u][v] <= flow[u][v])
				continue;
			if (dist[v]>dist[u] + cost[u][v])
			{
				bandWidth[v] = min(bandWidth[u], capacity[u][v] - flow[u][v]);
				dist[v] = dist[u] + cost[u][v];
				parent[v] = u;
				Node tmp;
				tmp.from = v;
				tmp.d = dist[v];
				q.push(tmp);
			}
			else if (dist[v] == dist[u] + cost[u][v])
			{
				if (min(bandWidth[u], (capacity[u][v] - flow[u][v]))>bandWidth[v])
				{
					bandWidth[v] = min(bandWidth[u], capacity[u][v] - flow[u][v]);
					dist[v] = dist[u] + cost[u][v];
					parent[v] = u;
					Node tmp;
					tmp.from = v;
					tmp.d = dist[v];
					q.push(tmp);
				}
			}

		}
	}
	vector<int> pathForward;
	vector<int> pathBackward;
	if (parent[v1] == -1)
	{
		result.pat = vector<int>();
		result.costToConsumer = MAX;
		result.flowOfPath = 0;
		return result;
	}
	pathBackward.push_back(v1);
	int tmp = parent[v1];
	while (tmp != v0)
	{
		pathBackward.push_back(tmp);
		tmp = parent[tmp];
	}
	pathBackward.push_back(tmp);
	for (int j = pathBackward.size() - 1; j >= 0; j--)
	{
		pathForward.push_back(pathBackward[j]);
	}
	result.pat = pathForward;
	result.flowOfPath = bandWidth[v1];
	result.costToConsumer = dist[v1];
	return result;

}
__OPT
pair<shortestPath, int> Dijkstra(int v0)
{
	pair<shortestPath, int> ss;
	shortestPath result;
	bool S[MAX_NET_NUM];
	int dist[MAX_NET_NUM];
	int prev1[MAX_NET_NUM];
	int bandWidth[MAX_NET_NUM];
	for (int i = 0; i<netNum; i++)
	{
		S[i] = false;
		dist[i] = MAX;
		prev1[i] = -1;
		bandWidth[i] = MAX;
	}
	priority_queue<Node>q;
	dist[v0] = 0;
	Node temp;
	temp.from = v0;
	temp.d = 0;
	q.push(temp);
	while (!q.empty())
	{
		Node cd = q.top();
		int u = cd.from;
		//cout<<"top"<<u<<endl;
		q.pop();
		if (S[u])
			continue;
		S[u] = true;
		for (int m = 0; m<LinkedVec[u].size(); m++)
		{
			int v;
			v = LinkedVec[u][m];//邻接点
			//cout<<v<<endl;
			if (S[v] || v == u || capacity[u][v] <= flow[u][v])
				continue;
			if (dist[v]>dist[u] + cost[u][v])
			{
				bandWidth[v] = min(bandWidth[u], capacity[u][v] - flow[u][v]);
				dist[v] = dist[u] + cost[u][v];
				prev1[v] = u;
				Node tmp;
				tmp.from = v;
				tmp.d = dist[v];
				q.push(tmp);
			}
			else if (dist[v] == dist[u] + cost[u][v])
			{
				if (min(bandWidth[u], capacity[u][v] - flow[u][v])>bandWidth[v])
				{
					bandWidth[v] = min(bandWidth[u], capacity[u][v] - flow[u][v]);
					dist[v] = dist[u] + cost[u][v];
					prev1[v] = u;
					Node tmp;
					tmp.from = v;
					tmp.d = dist[v];
					q.push(tmp);
				}
			}

		}
	}

	//想把这改成直接返回最小的路径
	int minCostConsu = -1;
	int minCostPath = MAX;
	for (int i = 0; i<consumerNum; i++)
	{
		if (i == v0 || maxFlag[i] || consumerRequest[i] == consumerAcheive[i])
		{
			continue;
		}
		vector<int> pathForward;
		vector<int> pathBackward;
		//沿着prev找到v0到该节点的路径
		if (prev1[connectToConsumer[i]] == -1)
		{
			continue;
		}
		pathBackward.push_back(connectToConsumer[i]);
		int tmp = prev1[connectToConsumer[i]];
		while (tmp != v0)
		{
			pathBackward.push_back(tmp);
			tmp = prev1[tmp];
		}
		pathBackward.push_back(tmp);
		for (int j = pathBackward.size() - 1; j >= 0; j--)
		{
			pathForward.push_back(pathBackward[j]);
		}
		int flowPath = bandWidth[connectToConsumer[i]];
		int costPath = dist[connectToConsumer[i]];
		if (costPath == MAX || (((costPath)*flowPath) >=/*1.225**/((serverCost - consumerCost[i])*1.0*flowPath / (consumerRequest[i] - consumerAcheive[i]))))
		{
			continue;
		}
		if (flowPath>0 && costPath<minCostPath)
		{
			minCostConsu = i;
			minCostPath = costPath;
			result.pat = pathForward;
			result.costToConsumer = costPath;
			result.flowOfPath = flowPath;

		}
	}
	if (minCostConsu == -1)
	{
		result.pat = vector<int>();
		result.costToConsumer = MAX;
		result.flowOfPath = 0;
		ss.first = result;
		ss.second = -1;
	}
	else
	{
		ss.first = result;
		ss.second = minCostConsu;
	}
	return ss;

}
__OPT
pair<shortestPath, int> DijkstraAdj(int v0)
{
	pair<shortestPath, int> ss;
	shortestPath result;
	bool S[MAX_NET_NUM];
	int dist[MAX_NET_NUM];
	int prev1[MAX_NET_NUM];
	int bandWidth[MAX_NET_NUM];
	for (int i = 0; i<netNum; i++)
	{
		S[i] = false;
		dist[i] = MAX;
		prev1[i] = -1;
		bandWidth[i] = MAX;
	}
	priority_queue<Node>q;
	dist[v0] = 0;
	Node temp;
	temp.from = v0;
	temp.d = 0;
	q.push(temp);
	while (!q.empty())
	{
		Node cd = q.top();
		int u = cd.from;
		//cout<<"top"<<u<<endl;
		q.pop();
		if (S[u])
			continue;
		S[u] = true;
		for (int m = 0; m<LinkedVec[u].size(); m++)
		{
			int v;
			v = LinkedVec[u][m];//邻接点
			//cout<<v<<endl;
			if (S[v] || v == u || capacity[u][v] <= flow[u][v])
				continue;
			if (dist[v]>dist[u] + cost[u][v])
			{
				bandWidth[v] = min(bandWidth[u], capacity[u][v] - flow[u][v]);
				dist[v] = dist[u] + cost[u][v];
				prev1[v] = u;
				Node tmp;
				tmp.from = v;
				tmp.d = dist[v];
				q.push(tmp);
			}
			else if (dist[v] == dist[u] + cost[u][v])
			{
				if (min(bandWidth[u], capacity[u][v] - flow[u][v])>bandWidth[v])
				{
					bandWidth[v] = min(bandWidth[u], capacity[u][v] - flow[u][v]);
					dist[v] = dist[u] + cost[u][v];
					prev1[v] = u;
					Node tmp;
					tmp.from = v;
					tmp.d = dist[v];
					q.push(tmp);
				}
			}

		}
	}

	//想把这改成直接返回最小的路径
	int minCostConsu = -1;
	int minCostPath = MAX;
	for (int i = 0; i<noServers.size(); i++)
	{
		int con = noServers[i];
		if (con == v0 || maxFlag[con] || consumerRequest[con] == consumerAcheive[con])
		{
			continue;
		}
		vector<int> pathForward;
		vector<int> pathBackward;
		//沿着prev找到v0到该节点的路径
		if (prev1[connectToConsumer[con]] == -1)
		{
			continue;
		}
		pathBackward.push_back(connectToConsumer[con]);
		int tmp = prev1[connectToConsumer[con]];
		while (tmp != v0)
		{
			pathBackward.push_back(tmp);
			tmp = prev1[tmp];
		}
		pathBackward.push_back(tmp);
		for (int j = pathBackward.size() - 1; j >= 0; j--)
		{
			pathForward.push_back(pathBackward[j]);
		}
		int flowPath = bandWidth[connectToConsumer[con]];
		int costPath = dist[connectToConsumer[con]];
		if (costPath == MAX)
		{
			continue;
		}
		if (flowPath>0 && costPath<minCostPath)
		{
			minCostConsu = con;
			minCostPath = costPath;
			result.pat = pathForward;
			result.costToConsumer = costPath;
			result.flowOfPath = flowPath;

		}
	}
	if (minCostConsu == -1)
	{
		result.pat = vector<int>();
		result.costToConsumer = MAX;
		result.flowOfPath = 0;
		ss.first = result;
		ss.second = -1;
	}
	else
	{
		ss.first = result;
		ss.second = minCostConsu;
	}
	return ss;

}
void BackFlow(int i)
{
	if (!consumerPath[i].empty())
	{
		consumerCost[i] = 0;
		for (int m = 0; m<consumerPath[i].size(); m++)
		{
			costTotal -= consumerPath[i][m].cost1;
			vector<int> temp;
			int ff = consumerPath[i][m].flow1;
			temp = consumerPath[i][m].path1;
			//更新边的流量
			for (int n = 0; n<temp.size() - 1; n++)
			{
				flow[temp[n]][temp[n + 1]] -= ff;
			}
			//更新需求,i是消费节点！！consumerPath[i][m]是结构体
			consumerAcheive[i] -= consumerPath[i][m].flow1;
		}
		consumerPath[i].clear();

		if (consumerPath[i].empty())
		{
		}

	}

}
__OPT
void AdjustPath2()
{
	cout << "服务器所在网络节点" << endl;
	for (int i = 0; i<severPos.size(); i++)
	{
		cout << severPos[i] << " ";

	}
	for (int i = 0; i<consumerNum; i++)
	{
		BackFlow(i);
	}
	//cout<<"去掉所有路径，这时成本为："<<costTotal<<endl;
	//cout<<"本来应该是："<<serverCost<<"*"<<serverNum<<serverCost*serverNum<<endl;

	//1、找到需要设置路径的消费节点
	for (int m = 0; m<consumerNum; m++)
	{
		if (!maxFlag[m])
			noServers.push_back(m);
	}
	//cout<<"放了服务器的节点有："<<severPos.size()<<endl;
	//cout<<"没有放置服务器的节点有："<<noServers.size()<<endl;
	while (1)
	{
		//找到一条最短的
		int minCostConsu = -1;
		int minCost = MAX;
		shortestPath minCostPath;
		for (int m = 0; m<severPos.size(); m++)
		{
			int n = severPos[m];
			pair<shortestPath, int> minCostPathTemp = DijkstraAdj(connectToConsumer[n]);
			if (minCostPathTemp.first.costToConsumer<minCost)
			{
				minCostPath = minCostPathTemp.first;
				minCost = minCostPathTemp.first.costToConsumer;
				minCostConsu = minCostPathTemp.second;
			}

		}
		//找不到????
		if (minCostConsu == -1)
		{
			cout << "一条可行路径都没有，如果还是没有满足，就要加服务器了" << endl;
			break;
		}
		//找到了,更新和这条边有关的各种信息
		if (minCostPath.flowOfPath >= (consumerRequest[minCostConsu] - consumerAcheive[minCostConsu]))
			minCostPath.flowOfPath = consumerRequest[minCostConsu] - consumerAcheive[minCostConsu];
		//调试
		/*cout<<"最后给"<<connectToConsumer[minCostConsu]<<"("<<minCostConsu<<")"<<"更新的流量是："
		<<result[minCostConsu].flowOfPath<<endl;*/
		//2后续处理：更新总成本
		costTotal += minCostPath.costToConsumer*minCostPath.flowOfPath;
		//2后续处理：将此路径加入到改消费点的路径中去
		pathAndFlow pp;
		pp.path1 = minCostPath.pat;
		pp.flow1 = minCostPath.flowOfPath;
		pp.cost1 = minCostPath.costToConsumer*minCostPath.flowOfPath;
		consumerPath[minCostConsu].push_back(pp);
		//2后续处理：更新本路径流量
		vector<int> temp1 = minCostPath.pat;
		for (int i = 0; i<temp1.size() - 1; i++)
		{
			flow[temp1[i]][temp1[i + 1]] += minCostPath.flowOfPath;
		}
		//2后续处理：更新本消费节点的需求
		consumerAcheive[minCostConsu] += minCostPath.flowOfPath;
		//cout<<"更新为："<<consumerAcheive[minCostConsu]<<endl;
		//没有放服务器的消费节点也满足了，退出
		int test = 0;
		for (; test<noServers.size(); test++)
		{
			if (consumerRequest[noServers[test]] <= consumerAcheive[noServers[test]])
				continue;
			else
				break;
		}
		if (test == noServers.size())
		{

			break;
		}
	}
	//	cout<<"服务器个数为："<<serverNum<<endl;
	cout << "总花费2是：" << costTotal << endl;

	finish = clock();
	printf("time: %.3fs\n", 1.0*(finish - start) / CLOCKS_PER_SEC);
}
void AdjustPath3()//最大流
{
	for (int i = 0; i<consumerNum; i++)
	{
		BackFlow(i);
	}

	//1、找到需要设置路径的消费节点
	for (int m = 0; m<consumerNum; m++)
	{
		if (!maxFlag[m])
			noServers.push_back(m);
	}
	memset(head, -1, sizeof(head));
	edge_count = 0;
	res.clear();
	for (int i = 0; i<netNum; i++)
	{
		for (int j = 0; j<netNum; j++)
		{
			if (cost[i][j] != MAX)
			{
				add_edge_mcmf(i + 1, j + 1, capacity[i][j], cost[i][j]);
			}
		}
	}
	for (int i = 0; i<severPos.size(); i++)
	{
		add_edge_mcmf(0, connectToConsumer[severPos[i]] + 1, MAX, 0);
	}
	for (int i = 0; i<noServers.size(); i++)
	{
		add_edge_mcmf(connectToConsumer[noServers[i]] + 1, netNum + 1, consumerRequest[noServers[i]], 0);
	}
	vector<int> p;
	for (int i = 0; i < severPos.size(); i++)
	{
		p.clear();
		int m = connectToConsumer[severPos[i]];
		p.push_back(m);
		int t = connectToVertex[m];
		p.push_back(t);
		p.push_back(consumerRequest[t]);
		res.push_back(p);
	}
	costTotal = mcmf2(0, netNum + 1, netNum + 2) + costTotal;
	// costTotal = mcmf(0, netNum+1, VeconsumerRequest[t]rtexNum+2) + serverCost*serverNum;
	memset(vis, 0, sizeof(vis));
	path[0] = 0;
	vis[0] = 1;
	get_path_mcmf(0, 1, MAX);
	cout << "路径为：" << endl;
	for (int i = 0; i<res.size(); i++)
	{
		for (int j = 0; j<res[i].size(); j++)
		{
			cout << res[i][j] << " ";
		}
		char b[5];
		cout << endl;
	}
	cout << "总花费2是：" << costTotal << endl;

	finish = clock();
	printf("time: %.3fs\n", 1.0*(finish - start) / CLOCKS_PER_SEC);

}
void IniMCMF()
{
	noServers.clear();
	for (int m = 0; m<consumerNum; m++)
	{
		if (!maxFlag[m])
			noServers.push_back(m);
	}
	memset(head, -1, sizeof(head));
	edge_count = 0;
	//res.clear();
	for (int i = 0; i<netNum; i++)
	{
		for (int j = 0; j<netNum; j++)
		{
			if (cost[i][j] != MAX)
			{
				add_edge_mcmf(i + 1, j + 1, capacity[i][j], cost[i][j]);
			}
		}
	}
	//cout<<"与0相连的节点：";
	for (int i = 0; i<severPos.size(); i++)
	{
		//cout<<severPos[i]<<" ";
		add_edge_mcmf(0, severPos[i] + 1, MAX, 0);
	}
	//cout<<endl;
	//cout<<"与51相连的节点是：";
	for (int i = 0; i<noServers.size(); i++)
	{
		//cout<<connectToConsumer[noServers[i]]<<" ";
		add_edge_mcmf(connectToConsumer[noServers[i]] + 1, netNum + 1, consumerRequest[noServers[i]], 0);
	}
	//cout<<endl;

}
void updateServer(Chrom ch)
{
	severPos.clear();
	memset(maxFlag, false, consumerNum);
	vector<int> temp = ch.bit;
	for (int i = 0; i<netNum; i++)
	{
		if (temp[i] == 1)
		{
			severPos.push_back(i);
		}
		for (int j = 0; j<consumerNum; j++)
		{
			if (temp[i] == 1 && i == connectToConsumer[j])
			{
				maxFlag[j] = 1;
			}
		}
	}
	serverNum = severPos.size();
}
bool CanProvide(Chrom ch)
{
	int provide = 0;
	int request = 0;
	vector<int> temp = ch.bit;
	for (int i = 0; i<netNum; i++)
	{
		if (temp[i] == 1)
		{
			for (int j = 0; j<netNum; j++)
			{
				provide += capacity[i][j];
			}
		}
	}
	severPos.clear();
	memset(maxFlag, false, consumerNum);
	for (int i = 0; i<netNum; i++)
	{
		if (temp[i] == 1)
		{
			severPos.push_back(i);
		}
		for (int j = 0; j<consumerNum; j++)
		{
			if (temp[i] == 1 && i == connectToConsumer[j])
			{
				maxFlag[j] = 1;
			}
		}
	}
	noServers.clear();
	for (int m = 0; m<consumerNum; m++)
	{
		if (!maxFlag[m])
			noServers.push_back(m);
	}
	for (int m = 0; m<noServers.size(); m++)
	{
		request += consumerRequest[noServers[m]];
	}
	if (provide<request)
		return false;
	else
		return true;

}
int GetServNum(Chrom ch)
{
	int result = 0;
	vector<int> temp = ch.bit;
	for (int i = 0; i<netNum; i++)
	{
		if (temp[i] == 1)
		{
			result++;
		}
	}
	return result;
}
void evpop(int num)
{
	int random;
	double sum = 0;
	double part = rand() % 20;
	part = (part + 80) / 100.0;
	int firstServer = num*part;
	part = rand() % 20;
	part = part / 100.0;
	int seconServer = num*part;
	int thirdServer = num - firstServer - seconServer;

	for (int i = 0; i<popSize; i++)
	{
		vector<int> temp(netNum, 0);
		if (i == 0)
		{
			for (int i = 0; i<severPos.size(); i++)
			{
				temp[connectToConsumer[severPos[i]]] = 1;
			}
		}
		if (i != 0 && i < popSize / 2)
		{
			//前一半在外圈移动
			temp = popCurrent[0].bit;
			int kk = 0, ss = 0;
			do
			{
				kk = rand() % VertexFirst.size();

			} while (popCurrent[0].bit[VertexFirst[kk]] != 1);
			temp[VertexFirst[kk]] = 0;
			do
			{
				ss = rand() % VertexFirst.size();

			} while (popCurrent[0].bit[VertexFirst[ss]] != 0);
			temp[VertexFirst[ss]] = 1;


		}
		else if (i != 0 && i >= popSize / 2)
		{
			//后一半在邻接点处移动
			int kk = rand() % severPos.size();
			int ss = rand() % LinkedVec[severPos[kk]].size();
			temp = popCurrent[0].bit;
			temp[severPos[kk]] = 0;
			temp[LinkedVec[severPos[kk]][ss]] = 1;

		}
		popCurrent[i].bit = temp;
		popCurrent[i].servCount = GetServNum(popCurrent[i]);
		updateServer(popCurrent[i]);
		IniMCMF();
		int costTemp = mcmf2(0, netNum + 1, netNum + 2);
		popCurrent[i].fit = (costTemp == MAX ? MAX : (costTemp + popCurrent[i].servCount*serverCost));
		//cout<<"个体fit："<< popCurrent[i].fit<<endl;
		if (popCurrent[i].fit != MAX)
		{
			sum += 1.0 / (popCurrent[i].fit);
		}


	}
	for (int i = 0; i<popSize; i++)
	{
		if (popCurrent[i].fit == MAX)
		{
			popCurrent[i].rfit = 0;
		}
		else
		{
			popCurrent[i].rfit = (1.0 / popCurrent[i].fit) / sum;
		}
		popCurrent[i].cfit = 0;
	}
	for (int i = 0; i<popSize; i++)
	{
		cout << "本个体的fit：" << popCurrent[i].fit << " ";
	}
	cout << endl;
}
int GetBestIndividual()
{
	int min = MAX;
	int minOrder = -1;
	for (int i = 0; i<popSize; i++)
	{
		if (popNext[i].fit<min)
		{
			min = popNext[i].fit;
			minOrder = i;
		}
	}
	if (minOrder == -1)
	{
		cout << "error" << endl;
	}
	else
	{
		return minOrder;
	}

}
void pickChromes_new(/*vector<Chrom> popNext*/)
{

	double p;
	double sum = 0.0;
	for (int i = 0; i< popSize; i++)
	{
		if (popCurrent[i].fit != MAX)
		{
			sum += 1.0 / popNext[i].fit;
		}

	}
	for (int i = 0; i<popSize; i++)
	{
		if (popCurrent[i].fit == MAX)
		{
			popNext[i].rfit = 0;
		}
		else
		{
			popNext[i].rfit = (1.0 / popNext[i].fit) / sum;
		}
	}
	int min = MAX;
	int minOrder = -1;
	for (int i = 0; i<popSize; i++)
	{
		if (popCurrent[i].fit<min)
		{
			min = popCurrent[i].fit;
			minOrder = i;
		}
	}
	if (minOrder == -1)
	{
		cout << "error" << endl;
	}
	else
	{
		BestIndivi = minOrder;
	}
	popNext[0].cfit = popNext[0].rfit;
	for (int i = 1; i<popSize; i++)
	{
		popNext[i].cfit = popNext[i - 1].cfit + popNext[i].rfit;
	}
	for (int i = 0; i<popSize; i++)
	{
		if (i == minOrder)
			continue;
		p = rand() % 10;
		p = p / 10;
		if (p<popNext[0].cfit)
		{
			popCurrent[i] = popNext[0];
		}
		else
		{
			for (int j = 0; j<popSize; j++)
			{
				if (popNext[j].cfit <= p&&p<popNext[j + 1].cfit)
				{
					popCurrent[i] = popNext[j + 1];
					break;
				}
			}
		}
	}
	for (int i = 0; i<popSize; i++)
	{
		popNext[i] = popCurrent[i];
	}
}
void crossOver(/*vector<Chrom> popNext*/)
{
	//对于每一条染色体进行内部交换，本次最佳不交换
	int exch1, exch2, temp;
	for (int i = 0; i<popSize; i++)
	{
		//if(i == minOrder) continue;
		double random = rand() % 100;
		random = random / 100.0;
		{
			if (random>0)
			{
				int minPos = rand() % netNum;
				int maxPos = rand() % netNum;
				while (maxPos<minPos || (maxPos - minPos)>netNum)
				{
					maxPos = rand() % netNum;
				}
				int individual = rand() % popSize;
				Chrom parent1 = popNext[i];
				Chrom parent2 = popNext[individual];
				for (int j = 0; j <= maxPos - minPos; j++)
				{
					int temp = popNext[i].bit[j];
					popNext[i].bit[j] = popNext[individual].bit[j];
					popNext[individual].bit[j] = temp;
				}
				//子代中不好的被父代代替，写的琐碎
				popNext[i].servCount = GetServNum(popNext[i]);
				updateServer(popNext[i]);
				IniMCMF();
				int costTemp = mcmf2(0, netNum + 1, netNum + 2);
				int childCost1 = (costTemp == MAX ? MAX : (costTemp + popNext[i].servCount*serverCost));
				popNext[individual].servCount = GetServNum(popNext[individual]);
				updateServer(popNext[individual]);
				IniMCMF();
				costTemp = mcmf2(0, netNum + 1, netNum + 2);
				int childCost2 = (costTemp == MAX ? MAX : (costTemp + popNext[individual].servCount*serverCost));
				//从4个里边选两个好的
				int max1 = childCost1>childCost2 ? i : individual;
				Chrom min2 = parent1.fit<parent2.fit ? parent1 : parent2;
				popNext[max1] = min2;
				//int maxChild = childCost1>childCost2?individual:i;
				//popNext[maxChild] = popNext[BestIndivi];/*parent1.fit<parent2.fit?parent1:parent2;*/
				if (GetServNum(popNext[i]) >= MaxServerNum)
				{
					popNext[i] = popNext[BestIndivi];
				}
				if (GetServNum(popNext[individual]) >= MaxServerNum)
				{
					popNext[individual] = popNext[BestIndivi];
				}

			}
		}
	}
	for (int i = 0; i<popSize; i++)
	{
		popNext[i].servCount = GetServNum(popNext[i]);
		updateServer(popNext[i]);
		IniMCMF();
		int costTemp = mcmf2(0, netNum + 1, netNum + 2);
		popNext[i].fit = (costTemp == MAX) ? MAX : (costTemp + popNext[i].servCount*serverCost);
		//cout<<"个体fit："<< popNext[i].fit<<endl;
	}
	BestIndivi = GetBestIndividual();
}
void mutation(/*vector<Chrom> popNext*/)
{
	int random;
	int row, col;
	random = rand() % netNum;
	//cout<<random<<endl;
	if (random <= netNum)
	{
		for (int i = 0; i<popSize; i++)
		{
			row = rand() % popSize;
			col = rand() % netNum;
			Chrom IndividTemp = popNext[row];
			int temp = popNext[row].bit[col];
			if (popNext[row].bit[col] == 0)
			{
				popNext[row].bit[col] = 1;
				//cout<<"网络节点："<<col<<"y由0变1"<<endl;
			}
			else if (popNext[row].bit[col] == 1)
			{
				popNext[row].bit[col] = 0;
				//cout<<"网络节点："<<col<<"y由1变0"<<endl;
			}
			if (GetServNum(popNext[row]) >= MaxServerNum)
			{
				popNext[row].bit[col] = temp;
			}

			popNext[row].servCount = GetServNum(popNext[row]);
			/*if(!CanProvide(popNext[row]))
			{
			popNext[row] = IndividTemp;
			continue;
			}*/

			updateServer(popNext[row]);
			IniMCMF();
			int costTemp = mcmf2(0, netNum + 1, netNum + 2);
			popNext[row].fit = (costTemp == MAX ? MAX : (costTemp + popNext[row].servCount*serverCost));
			/*if(popNext[row].fit>IndividTemp.fit)
			{
			popNext[row] = IndividTemp;
			}*/
			//cout<<"个体fit："<< popNext[i].fit<<endl
		}

	}
	BestIndivi = GetBestIndividual();
	for (int i = 0; i<popSize; i++)
	{
		popCurrent[i] = popNext[i];
	}
}
void mutationEvery(/*vector<Chrom> popNext*/)
{
	int random;
	int row, col;
	random = rand() % netNum;
	//cout<<random<<endl;
	if (random <= netNum)
	{
		for (int i = 0; i<popSize; i++)
		{
			col = rand() % netNum;
			Chrom IndividTemp = popNext[i];
			int temp = popNext[i].bit[col];
			if (popNext[i].bit[col] == 0)
			{
				popNext[i].bit[col] = 1;
				//cout<<"网络节点："<<col<<"y由0变1"<<endl;
			}
			else if (popNext[i].bit[col] == 1)
			{
				popNext[i].bit[col] = 0;
				//cout<<"网络节点："<<col<<"y由1变0"<<endl;
			}
			if (GetServNum(popNext[i]) >= MaxServerNum)
			{
				popNext[i].bit[col] = temp;
				continue;
			}

			popNext[i].servCount = GetServNum(popNext[i]);
			/*if(!CanProvide(popNext[row]))
			{
			popNext[row] = IndividTemp;
			continue;
			}*/

			updateServer(popNext[i]);
			IniMCMF();
			int costTemp = mcmf2(0, netNum + 1, netNum + 2);
			popNext[i].fit = (costTemp == MAX ? MAX : (costTemp + popNext[i].servCount*serverCost));
			if (popNext[i].fit>IndividTemp.fit)
			{
				popNext[i] = IndividTemp;
			}
			//cout<<"个体fit："<< popNext[i].fit<<endl
		}

	}
	BestIndivi = GetBestIndividual();
	for (int i = 0; i<popSize; i++)
	{
		popCurrent[i] = popNext[i];
	}
}
void mutation1(/*vector<Chrom> popNext*/)
{
	int random;
	int row, col;
	random = rand() % netNum;
	//cout<<random<<endl;
	if (random <= netNum)
	{
		for (int i = 0; i<popSize; i++)
		{
			row = rand() % popSize;
			col = rand() % netNum;
			vector<int> resultTemp;
			resultTemp = popNext[i].bit;
			vector<int> ServerTotal;
			for (int j = 0; j<resultTemp.size(); j++)
			{
				if (resultTemp[j] == 1)
				{
					ServerTotal.push_back(j);
				}
			}
			int kk = rand() % ServerTotal.size();
			int ss = rand() % LinkedVec[ServerTotal[kk]].size();
			Chrom temp = popNext[i];
			popNext[i].bit[ServerTotal[kk]] = 0;
			popNext[i].bit[LinkedVec[ServerTotal[kk]][ss]] = 1;
			if (GetServNum(popNext[row]) >= MaxServerNum)
			{
				popNext[i] = temp;
				continue;
			}
			popNext[i].servCount = GetServNum(popNext[i]);
			updateServer(popNext[i]);
			IniMCMF();
			int costTemp = mcmf2(0, netNum + 1, netNum + 2);
			popNext[i].fit = (costTemp == MAX ? MAX : (costTemp + popNext[i].servCount*serverCost));
			if (popNext[i].fit>temp.fit)
			{
				popNext[i] = temp;
			}
			//cout<<"个体fit："<< popNext[i].fit<<endl
		}

	}
	BestIndivi = GetBestIndividual();
	for (int i = 0; i<popSize; i++)
	{
		popCurrent[i] = popNext[i];
	}
}
void ga()
{

	for (int i = 0; i<consumerNum; i++)
	{
		BackFlow(i);
	}

	// IniMCMF();
	MaxServerNum = consumerNum;
	int CurrentServNum = MaxServerNum*0.7/*serverNum*/;
	popSize = 40;
	int ReNum = 50/*100*/;//30,300能搜出最优解，50个节点 //最新4-4，种群个数为40，迭代次数为100到200之间
	if (netNum <= 200)
		popSize = 30;
	int MinCostResult = MAX;
	Chrom ChromResult;
	int MinCostLast = MinCostResult;
	evpop(CurrentServNum);
	int countEqual = 0;
	for (int i = 0; i<ReNum; i++)
	{
		finish = clock();
		if ((finish - start) / CLOCKS_PER_SEC > 85) break;

		for (int j = 0; j<popSize; j++)
		{
			popNext[j] = popCurrent[j];
		}
		pickChromes_new(/*popNext*/);
		cout << "选择以后" << endl;
		for (int i = 0; i<popSize; i++)
		{
			cout << "个体：" << popCurrent[i].fit << " ";
		}
		cout << endl;
		crossOver(/*popNext*/);
		// mutation(/*popNext*/);
		mutationEvery();
		//mutation1();
		for (int j = 0; j<popSize; j++)
		{
			popCurrent[j] = popNext[j];
		}
		int CostTemp = MAX;
		Chrom ChromTemp;
		cout << "交叉变异以后" << endl;
		for (int i = 0; i<popSize; i++)
		{
			cout << "个体：" << popCurrent[i].fit << " ";
		}
		cout << endl;
		cout << endl;

		for (int l = 0; l<popSize; l++)
		{
			if (popCurrent[l].fit < CostTemp)
			{
				CostTemp = popCurrent[l].fit;
				ChromTemp = popCurrent[l];
				serverNum = popCurrent[l].servCount;
			}
		}
		//本次结果比最优结果还要好，更新最优结果
		if (CostTemp<MinCostResult)
		{
			MinCostResult = CostTemp;
			ChromResult = ChromTemp;//服务器分析来源
			//1.更新服务器的内容和个数
			updateServer(ChromResult);
			IniMCMF();
			res.clear();
			vector<int> p;
			for (int i = 0; i < severPos.size(); i++)
			{
				if (!is_con[severPos[i]]) continue;
				p.clear();
				int m = severPos[i];
				p.push_back(m);
				int t = connectToVertex[m];
				p.push_back(t);
				p.push_back(consumerRequest[t]);
				res.push_back(p);
			}
			costTotal = mcmf2(0, netNum + 1, netNum + 2) + serverCost*serverNum;
			memset(vis, 0, sizeof(vis));
			path[0] = 0;
			vis[0] = 1;
			get_path_mcmf(0, 1, MAX);
		}
		cout << "此次迭代的结果是：" << CostTemp << "，服务器个数是：" << serverNum << endl;
		if (MinCostResult == MinCostLast)
		{
			countEqual++;
		}
		else
		{
			countEqual = 0;
			MinCostLast = MinCostResult;
		}
		if (countEqual >= 2)
		{
			mutation1();
			cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
			// mutationEvery();
			countEqual = 0;
			MinCostLast = MinCostResult;
		}
	}
	cout << "最后的结果是：" << MinCostResult << "，服务器个数是：" << serverNum << endl;
	cout << "路径为：" << endl;
	cout << "总花费2是：" << costTotal << endl;
	finish = clock();
	printf("time: %.3fs\n", 1.0*(finish - start) / CLOCKS_PER_SEC);
}
void VertexDivided()
{
	bool VertexDiv[MAX_NET_NUM];
	memset(VertexDiv, false, MAX_NET_NUM);
	for (int k1 = 0; k1<consumerNum; k1++)
	{
		VertexFirst.push_back(connectToConsumer[k1]);
		VertexDiv[connectToConsumer[k1]] = true;
	}
	for (int k1 = 0; k1<VertexFirst.size(); k1++)
	{
		for (int k2 = 0; k2<LinkedVec[k1].size(); k2++)
		{
			if (!VertexDiv[LinkedVec[k1][k2]])
			{
				VertexSecond.push_back(LinkedVec[k1][k2]);
				VertexDiv[LinkedVec[k1][k2]] = true;
			}
		}
	}
	for (int k1 = 0; k1<netNum; k1++)
	{
		if (!VertexDiv[k1])
		{
			VertexThird.push_back(k1);
			VertexDiv[k1] = true;
		}
	}
	cout << "第一圈：" << VertexFirst.size() << endl;
	for (int i = 0; i<VertexFirst.size(); i++)
	{
		cout << VertexFirst[i] << " ";
	}
	cout << endl;
	cout << "第2圈：" << VertexSecond.size() << endl;
	for (int i = 0; i<VertexSecond.size(); i++)
	{
		cout << VertexSecond[i] << " ";
	}
	cout << endl;
	cout << "第3圈：" << VertexThird.size() << endl;
	for (int i = 0; i<VertexThird.size(); i++)
	{
		cout << VertexThird[i] << " ";
	}
	cout << endl;
}
void deploy_server_for_mini(char * topo[MAX_EDGE_NUM], int line_num, char * filename)
{

	// Output demo
	//BuildGraph(topo, line_num);
	//初始化
	srand(time(0));
	for (int i = 0; i<MAX_CON_NUM; i++)
	{

		vector<pathAndFlow> temp;
		consumerPath.push_back(temp);
		minFlag[i] = false;
		maxFlag[i] = false;
		connectToConsumer[i] = 0;
		consumerRequest[i] = 0;
		consumerAcheive[i] = 0;
		consumerCost[i] = 0;
	}
	for (int i = 0; i<MAX_NET_NUM; i++)
	{
		for (int j = 0; j<MAX_NET_NUM; j++)
		{
			flow[i][j] = 0;
			capacity[i][j] = 0;
			cost[i][j] = MAX;//这里不太确定，到底是0还是很大
		}
		LinkedVec.push_back(vector<int>());
	}
	BuildGraph(topo, line_num);
	//给网络节点分层
	VertexDivided();
	for (int i = 0; i<consumerNum; i++)
	{
		//1、找需求最大
		int pos = -1;
		int maxRequest = 0;
		for (int j = 0; j<consumerNum; j++)
		{
			if (!maxFlag[j] && (consumerRequest[j] - consumerAcheive[j])>maxRequest)
			{
				pos = j;
				maxRequest = consumerRequest[j] - consumerAcheive[j];
			}
		}
		if (pos == -1)
		{
			continue;
		}
		//1选定服务器位置后续处理
		maxFlag[pos] = true;
		severPos.push_back(pos);
		BackFlow(pos);
		consumerAcheive[pos] = consumerRequest[pos];
		costTotal += serverCost;
		serverNum++;
		while (1)
		{
			pair<shortestPath, int> resultTemp = Dijkstra(connectToConsumer[pos]);//pos到自己的？
			shortestPath result = resultTemp.first;
			int minCostConsu = resultTemp.second;
			if (minCostConsu == -1)
			{
				break;
			}
			//2如果最小路径存在，后续处理：更新流量、存储路径。
			if (result.flowOfPath >= (consumerRequest[minCostConsu] - consumerAcheive[minCostConsu]))
			{
				result.flowOfPath = consumerRequest[minCostConsu] - consumerAcheive[minCostConsu];
			}
			//2后续处理：更新总成本
			/*cout<<"现有成本："<<costTotal<<"+"<< result.costToConsumer<<"*"<<result.flowOfPath<<"="<<result.costToConsumer*result.flowOfPath<<
			"="<<costTotal+result.costToConsumer*result.flowOfPath<<endl;*/
			costTotal += result.costToConsumer*result.flowOfPath;
			//2后续处理：将此路径加入到改消费点的路径中去
			pathAndFlow pp;
			pp.path1 = result.pat;
			pp.flow1 = result.flowOfPath;
			pp.cost1 = result.costToConsumer*result.flowOfPath;
			consumerPath[minCostConsu].push_back(pp);
			//2后续处理：更新本路径流量
			vector<int> temp1 = result.pat;
			for (int i = 0; i<temp1.size() - 1; i++)
			{
				flow[temp1[i]][temp1[i + 1]] += result.flowOfPath;
			}
			//2后续处理：更新本消费节点的需求
			consumerAcheive[minCostConsu] += result.flowOfPath;
			consumerCost[minCostConsu] += result.flowOfPath*result.costToConsumer;

		}
		int test = 0;
		for (; test<consumerNum; test++)
		{
			if (consumerRequest[test] <= consumerAcheive[test])
				continue;
			else
				break;
		}
		if (test == consumerNum)
		{
			cout << "服务器个数为：" << serverNum << endl;
			cout << "总花费1是：" << costTotal << endl;
			//AdjustPath1();

			break;
		}
	}
	//AdjustPath3();
	ga();
	OutPutResult(filename);
}

void write2file(char * filename)
{
	freopen(filename, "w", stdout);
}

void print_res(char * filename, int now_server_num)
{
	str_res.clear();
	char a[20];
	sprintf(a, "%d\n\n", res.size());
	str_res = a;

	for (int i = 0; i < res.size(); i++)
	{
		sprintf(a, "%d", res[i][0]);
		str_res += a;
		for (int j = 1; j < res[i].size(); j++)
		{
			sprintf(a, " %d", res[i][j]);
			str_res += a;
		}
		if (i != res.size() - 1)
		{
			str_res += "\n";
		}
	}

	char * topo_file = (char *)str_res.c_str();
	write_result(topo_file, filename);

	cout << "server_num: " << now_server_num << endl;
	cout << "first_cost: " << first_cost << endl;
	cout << "second_cost: " << second_cost << endl;
	boundary();
}

void add_edge(int from, int to, int bw, int cost)
{
	edges.push_back(Edge(from, to, bw, cost));
	G[from].push_back(edges.size() - 1);
}

vector<int> get_path(int t, bool is_reverse)
{
	vector<int> path;
	for (; t != -1; t = pre[t])
		path.push_back(t);
	if (is_reverse)
		reverse(path.begin(), path.end());
	return path;
}


void dijkstra(int s, int e)
{
	priority_queue<Node> Q;
	vector<int> p;
	for (int i = 0; i < netNum; i++)
	{
		bw[i] = INF;
		d[i] = INF;
	}
	d[s] = 0;
	memset(done, 0, sizeof(done));
	memset(pre, -1, sizeof(pre));
	Q.push(Node(d[s], s));
	while (!Q.empty())
	{
		Node x = Q.top();
		Q.pop();
		int from = x.from;
		if (done[from]) continue;
		done[from] = true;
		if (from == e) break;
		for (int i = 0; i < G[from].size(); i++)
		{
			Edge e = edges[G[from][i]];
			if (e.bw == 0) continue;
			if ((d[e.to] > d[from] + e.cost))
			{
				bw[e.to] = min(bw[from], e.bw);
				d[e.to] = d[from] + e.cost;
				pre[e.to] = from;
				Q.push(Node(d[e.to], e.to));
			}
			/*else if(d[e.to] == d[from] + e.cost)
			{
			if(min(bw[from], e.bw) > bw[e.to])
			{
			bw[e.to] = min(bw[from], e.bw);
			d[e.to] = d[from] + e.cost;
			pre[e.to] = from;
			Q.push(Node(d[e.to], e.to));
			}
			}*/
		}
	}
}

void init(char *topo[MAX_EDGE_NUM], int line_num)
{
	start = clock();
	serverNum = 0;
	first_cost = second_cost = 0;
	best_cost = INF;
	char *c;
	int spaceCount = 0;
	netNum = consumerNum = edgeNum = serverCost = 0;
	c = topo[0];
	memset(isCon, -1, sizeof(isCon));
	memset(is_con, 0, sizeof(is_con));
	while (*c != '\0' && *c != '\n' && *c != '\r')
	{
		if (*c == ' ')
		{
			c++;
			spaceCount++;
			continue;
		}
		if (spaceCount == 2)
		{
			consumerNum = *c - '0' + consumerNum * 10;
		}
		else if (spaceCount == 0)
		{
			netNum = *c - '0' + netNum * 10;
		}
		else if (spaceCount == 1)
		{
			edgeNum = *c - '0' + edgeNum * 10;
		}
		c++;
	}

	c = topo[2];
	while (*c != '\0' && *c != '\n' && *c != '\r')
	{
		serverCost = *c - '0' + serverCost * 10;
		c++;
	}

	int netnode, need, from, to, bw, cost;

	for (int i = 1; i < consumerNum + 1; i++)
	{
		c = topo[line_num - i];
		netnode = need = spaceCount = 0;
		while (*c != '\0' && *c != '\n' && *c != '\r')
		{
			if (*c == ' ')
			{
				c++;
				spaceCount++;
				continue;
			}
			if (spaceCount == 1)
			{
				netnode = *c - '0' + netnode * 10;
			}
			else if (spaceCount == 2)
			{
				need = *c - '0' + need * 10;
			}
			c++;
		}
		con_nodes.push_back(ConNode(consumerNum - i, netnode, need));
		isCon[netnode] = i - 1;
		net2con[netnode] = consumerNum - i;
		consumers_need[netnode] = need;
		con2net[consumerNum - i] = netnode;
		is_con[netnode] = true;
	}

	for (int i = line_num - consumerNum - 2;; i--)
	{
		c = topo[i];
		if (*c == '\n' || *c == '\r')
			break;
		from = to = bw = spaceCount = cost = 0;
		while (*c != '\0' && *c != '\n' && *c != '\r')
		{
			if (*c == ' ')
			{
				c++;
				spaceCount++;
				continue;
			}
			if (spaceCount == 1)
			{
				to = *c - '0' + to * 10;
			}
			else if (spaceCount == 2)
			{
				bw = *c - '0' + bw * 10;
			}
			else if (spaceCount == 3)
			{
				cost = *c - '0' + cost * 10;
			}
			else if (spaceCount == 0)
			{
				from = *c - '0' + from * 10;
			}
			c++;
		}
		add_edge(from, to, bw, cost);
		add_edge(to, from, bw, cost);
	}
}

bool is_ok()
{
	for (int i = 0; i < consumerNum; i++)
	{
		if (con_nodes[i].need != 0)
		{
			//cout << i << endl;
			return false;
		}

	}
	return true;
}

void restore_con_need()
{
	for (int i = 0; i < consumerNum; i++)
		con_nodes[i].need = con_nodes[i].real_need;

}

void restore_unit_bw()
{
	for (int i = 0; i < consumerNum; i++)
		con_nodes[i].unit_bw = 1.0*serverCost / con_nodes[i].real_need;
}

void restore_net_bw()
{
	for (int from = 0; from < netNum; from++)
		for (int i = 0; i < G[from].size(); i++)
			edges[G[from][i]].bw = edges[G[from][i]].real_bw;

}

void restore_is_server()
{
	for (int i = 0; i < consumerNum; i++)
		con_nodes[i].is_server = 0;

}

int find_next_way(char * filename, int now_server_num)
{
	memset(head, -1, sizeof(head));
	edge_count = 0;
	res.clear();
	//restore_con_need();
	restore_net_bw();

	for (int from = 0; from < netNum; from++)
		for (int i = 0; i < G[from].size(); i++)
			add_edge_mcmf(edges[G[from][i]].from + 1, edges[G[from][i]].to + 1, edges[G[from][i]].bw, edges[G[from][i]].cost);

	for (int i = 0; i < consumers.size(); i++)
		add_edge_mcmf(consumers[i] + 1, netNum + 1, consumers_need[consumers[i]], 0);

	for (int i = 0; i < servers.size(); i++)
		add_edge_mcmf(0, servers[i] + 1, INF, 0);

	int cost = mcmf(0, netNum + 1, netNum + 2);

	if (cost == -1) return -1;

	second_cost = cost + serverCost * now_server_num;

	if (second_cost < best_cost)
	{
		vector<int> p;
		for (int i = 0; i < consumerNum; i++)
		{
			if (!con_nodes[i].is_server) continue;

			p.clear();
			p.push_back(con_nodes[i].netnode);
			p.push_back(con_nodes[i].connode);
			p.push_back(con_nodes[i].real_need);
			res.push_back(p);
		}
		memset(vis, 0, sizeof(vis));
		path[0] = 0;
		vis[0] = 1;
		get_path_mcmf(0, 1, INF);
		print_res(filename, now_server_num);
		best_cost = second_cost;
		return best_cost;
	}
	return -1;
}

void go_inside(char * filename)
{
	oth_servers.clear();
	int cnt = 0, real_server_num = serverNum;
	if (netNum <= 500)
		real_server_num = INF;
	restore_con_need();

	while (true)
	{
		vector<int> p;
		int min_pos, min_need = INF;
		for (int i = 0; i < consumerNum; i++)
		{
			if (!con_nodes[i].is_server) continue;
			if (min_need > con_nodes[i].need)
			{
				min_need = con_nodes[i].need;
				min_pos = i;
			}
		}
		if (min_need == INF || cnt++ > real_server_num / 2) break;

		int min_d = INF, min_d_pos;
		dijkstra(con_nodes[min_pos].netnode, -1);

		for (int i = 0; i < consumerNum; i++)
		{
			if (i == min_pos || !con_nodes[i].is_server)
				continue;
			if (min_d > d[con_nodes[i].netnode])
			{
				min_d = d[con_nodes[i].netnode];
				min_d_pos = i;
			}
		}

		if (min_d == INF)
		{
			con_nodes[min_pos].need = INF;
			continue;
		}
		servers.clear();
		consumers.clear();

		for (int i = 0; i < consumerNum; i++)
		{
			if (i == min_d_pos || i == min_pos) continue;
			if (!con_nodes[i].is_server)
			{
				consumers.push_back(con_nodes[i].netnode);
				continue;
			}
			servers.push_back(con_nodes[i].netnode);
		}

		for (int i = 0; i < oth_servers.size(); i++)
			servers.push_back(oth_servers[i]);
		//cout << "oth_servers.size: " << oth_servers.size() << endl;

		p = get_path(con_nodes[min_d_pos].netnode, true);
		bool is_reback = false;
		int cost1 = -1, cost2 = -1, cost = -1;
		con_nodes[min_pos].is_server = 1;
		con_nodes[min_d_pos].is_server = 0;
		servers.push_back(con_nodes[min_pos].netnode);
		consumers.push_back(con_nodes[min_d_pos].netnode);

		cost1 = find_next_way(filename, serverNum - 1);
		if (cost1 > 0) is_reback = true;

		con_nodes[min_pos].is_server = 0;
		con_nodes[min_d_pos].is_server = 1;
		servers[servers.size() - 1] = con_nodes[min_d_pos].netnode;
		consumers[consumers.size() - 1] = con_nodes[min_pos].netnode;
		cost2 = find_next_way(filename, serverNum - 1);

		if (cost2 > 0) is_reback = true;

		bool is_first = false;
		consumers.push_back(con_nodes[min_d_pos].netnode);
		con_nodes[min_pos].is_server = 0;
		con_nodes[min_d_pos].is_server = 0;

		for (int i = 1; i < p.size() - 1; i++)
		{
			if (isCon[p[i]] != -1)
				con_nodes[isCon[p[i]]].is_server = 1;
			servers[servers.size() - 1] = p[i];
			cost = find_next_way(filename, serverNum - 1);
			if (cost > 0)
			{
				if (isCon[p[i]] == -1)
				{
					if (!is_first)
						oth_servers.push_back(p[i]);
					else
						oth_servers[oth_servers.size() - 1] = p[i];
				}
				is_reback = true;
				is_first = true;
				puts("one solution");
			}
			else
			{
				if (isCon[p[i]] != -1)
					con_nodes[isCon[p[i]]].is_server = 0;
			}
		}
		if (is_reback)
		{
			serverNum--;
			if (is_first) continue;
			if (cost2 > 0)
			{
				con_nodes[min_pos].is_server = 0;
				con_nodes[min_d_pos].is_server = 1;
				con_nodes[min_pos].need = INF;
			}
			else
			{
				con_nodes[min_pos].is_server = 1;
				con_nodes[min_d_pos].is_server = 0;
				con_nodes[min_d_pos].need = INF;
			}
		}

		else
		{
			con_nodes[min_pos].is_server = 1;
			con_nodes[min_d_pos].is_server = 1;
			con_nodes[min_pos].need = INF;
			con_nodes[min_d_pos].need = INF;
		}
		//con_nodes[min_pos].need = INF;
	}
}

void deploy(char * topo[MAX_EDGE_NUM], int line_num, char * filename, double now_weight)
{
	res.clear();
	vector <int> p;
	int pos;
	int max_need = -1;

	while (!is_ok())
	{
		max_need = -1;
		for (int i = 0; i < consumerNum; i++)
		{
			if (con_nodes[i].need > max_need)
			{
				max_need = con_nodes[i].need;
				pos = i;
			}
		}

		p.clear();
		p.push_back(con_nodes[pos].netnode);
		p.push_back(con_nodes[pos].connode);
		p.push_back(con_nodes[pos].need);
		res.push_back(p);
		con_nodes[pos].need = 0;
		con_nodes[pos].is_server = 1;
		first_cost += serverCost;
		serverNum += 1;
		isCon[con_nodes[pos].connode] = -1;

		while (true)
		{
			dijkstra(con_nodes[pos].netnode, -1);
			int min_d = INF;
			int min_pos = -1;
			for (int j = 0; j < consumerNum; j++)
			{
				double tmp_weight = now_weight;
				if (bw[con_nodes[j].netnode] >= con_nodes[j].need)
					tmp_weight = 1.0;

				if (j == pos || 1.0*d[con_nodes[j].netnode] > tmp_weight*con_nodes[j].unit_bw ||
					con_nodes[j].need == 0) continue;

				if (tmp_weight > 1.0 && con_nodes[j].unit_bw*con_nodes[j].need -
					d[con_nodes[j].netnode] * bw[con_nodes[j].netnode] <= 0)
					continue;
				if (min_d > d[con_nodes[j].netnode])
				{
					min_d = d[con_nodes[j].netnode];
					min_pos = j;
				}
			}
			if (min_d == INF) break;

			p = get_path(con_nodes[min_pos].netnode, true);
			int min_bw = bw[con_nodes[min_pos].netnode];

			if (min_bw == 0) continue;
			min_bw = min(min_bw, con_nodes[min_pos].need);
			for (int k = 0; k < p.size() - 1; k++)
			{
				int u = p[k];
				for (int m = 0; m < G[u].size(); m++)
				{
					if (edges[G[u][m]].to == p[k + 1])
					{
						edges[G[u][m]].bw -= min_bw;
						first_cost += edges[G[u][m]].cost * min_bw;
						break;
					}
				}
			}
			con_nodes[min_pos].unit_bw = (con_nodes[min_pos].unit_bw*con_nodes[min_pos].need - d[con_nodes[min_pos].netnode] * min_bw) / (con_nodes[min_pos].need - min_bw);
			con_nodes[min_pos].need -= min_bw;
			p.push_back(net2con[p[p.size() - 1]]);
			p.push_back(min_bw);
			res.push_back(p);
		}
	}
	if (first_cost < best_cost)
	{
		print_res(filename, serverNum);
		best_cost = first_cost;
	}
	servers.clear();
	consumers.clear();
	for (int i = 0; i < consumerNum; i++)
	{
		if (con_nodes[i].is_server)
			servers.push_back(con_nodes[i].netnode);
		else
			consumers.push_back(con_nodes[i].netnode);
	}
	find_next_way(filename, serverNum);
	go_inside(filename);
}

void deploy_server(char* topo[MAX_IN_NUM], int line_num, const char * const filename)
{
	//write2file(filename);
	init(topo, line_num);
	if (netNum <= 500)
		deploy_server_for_mini(topo, line_num, (char*)filename);
	else
	{
		_begin = 0.63;
		_end = 0.65;
		//_end = 0.600001;
		step = 0.01;
		//serverCost = 1000;
		int cnt = 0;
		for (double now_weight = _begin; now_weight < _end; now_weight += step)
		{
			//boundary();
			//cout << "cnt: " << ++cnt << endl;
			//cout << "best_cost: " << best_cost << endl;
			cout << "now_weight: " << now_weight << endl;

			deploy(topo, line_num, (char*)filename, now_weight);

			first_cost = second_cost = serverNum = 0;
			restore_con_need();
			restore_net_bw();
			restore_is_server();
			restore_unit_bw();
			cout << "best_cost: " << best_cost << endl;
		}
	}
	finish = clock();
	printf("time: %.3fs\n", 1.0*(finish - start) / CLOCKS_PER_SEC);
}
