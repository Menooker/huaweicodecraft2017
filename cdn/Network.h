# ifndef __NETWORK__
# define __NETWORK__

# include <vector>
# include <queue>
# include <unordered_map>
# include <iostream>
# include <sstream>
# include <string>
# include "lib_io.h"
#include <array>

# define INFINITY 2100000000
#define MAX_NODES (1000*1001)
using namespace std;

class Network {
public:
	class Mat
	{
	public:
		int* buf;
		int sz;
		Mat()
		{
			buf = nullptr;
		}
		
		void alloc(int _sz, int val)
		{
			sz=_sz;
			buf = new int[sz*sz];
			for (int i = 0; i < sz*sz; i++)
				buf[i] = val;
		}
		~Mat()
		{
			delete[] buf;
		}
		int& operator ()(int p1, int p2)
		{
			return buf[p1*sz + p2];
		}
	};
private:
	int deployCost;
	int numNode;
	int numLink;
	int numConsumer;
	int networkSize;
	Mat networkCost;
	Mat networkVol;
	Mat networkFlow;
	Mat RnetworkCost;
	Mat RnetworkVol;
	Mat RnetworkFlow;
	vector<pair<int, int>> consumer; // pair: (connect node, need flow)
	int superSource, superSink;

	void MSFinit(vector<bool> & serverpos) {
		// init network/Rnetwork flow and super source first
		for (int i = 0; i < networkSize; i++) {
			for (int j = 0; j < networkSize; j++) {
				networkFlow(i,j) = 0;
				RnetworkFlow(j,i) = RnetworkVol(j,i);
			}
		}
		for (int i = 0; i < networkSize; i++) {
			networkCost(superSource,i) = INFINITY;
			networkVol(superSource,i) = 0;
			RnetworkCost(i,superSource) = INFINITY;
			RnetworkVol(i,superSource) = 0;
			RnetworkFlow(i,superSource) = RnetworkVol(i,superSource);
		}
		for (int i = 0; i < serverpos.size(); i++) {
			if (serverpos[i]) {
				networkCost(superSource,i) = 0;
				networkVol(superSource,i) = INFINITY;
				RnetworkCost(i,superSource) = 0;
				RnetworkVol(i,superSource) = INFINITY;
				RnetworkFlow(i,superSource) = RnetworkVol(i,superSource);
			}
		}
	}
public:

	Network(char* inLines[MAX_IN_NUM])
	{
		stringstream ss;
		ss << inLines[0];
		ss >> numNode >> numLink >> numConsumer;
		networkSize = numNode + 2;

		networkCost .alloc(networkSize, INFINITY);
		networkVol.alloc(networkSize, 0);
		networkFlow.alloc(networkSize, 0);
		RnetworkCost.alloc(networkSize, INFINITY);
		RnetworkVol.alloc(networkSize, 0);
		RnetworkFlow.alloc(networkSize, 0);
		/*networkCost = vector<vector<int>>(networkSize, vector<int>(networkSize, INFINITY));
		networkVol = vector<vector<int>>(networkSize, vector<int>(networkSize, 0));
		networkFlow = vector<vector<int>>(networkSize, vector<int>(networkSize, 0));
		RnetworkCost = vector<vector<int>>(networkSize, vector<int>(networkSize, INFINITY));
		RnetworkVol = vector<vector<int>>(networkSize, vector<int>(networkSize, 0));
		RnetworkFlow = vector<vector<int>>(networkSize, vector<int>(networkSize, 0));*/
		consumer = vector<pair<int, int>>(numConsumer, make_pair(0, 0));

		ss.clear();
		ss << inLines[2];
		ss >> deployCost;

		for (int i = 4; i < 4 + numLink; i++) {
			ss.clear();
			ss << inLines[i];
			int from, to, vol, cost;
			ss >> from >> to >> vol >> cost;
			networkCost(from,to) = cost;
			networkVol(from,to) = vol;
			RnetworkCost(to,from) = -cost;
			RnetworkVol(to,from) = vol;
			RnetworkFlow(to,from) = vol;
		}

		for (int i = 5 + numLink; i < 5 + numLink + numConsumer; i++) {
			ss.clear();
			ss << inLines[i];
			int cid, connect, need;
			ss >> cid >> connect >> need;
			consumer[cid] = make_pair(connect, need);
		}

		// add super source and super sink
		superSource = numNode;
		superSink = numNode + 1;
		for (auto e : consumer) {
			networkCost(e.first,superSink) = 0;
			networkVol(e.first,superSink) = e.second;
			RnetworkCost(superSink,e.first) = 0;
			RnetworkVol(superSink,e.first) = e.second;
			RnetworkFlow(superSink,e.first) = e.second;
		}
	}

	int getDeployCost() { return deployCost; }
	int getNumNode() { return numNode; }
	int getNumLink() { return numLink; }
	int getNumConsumer() { return numConsumer; }

	int calCost(vector<bool> & serverpos) {
		int cost = calMSF(serverpos);
		for (bool e : serverpos) {
			if (e)
				cost += deployCost;
		}
		// TODO legel check?
		for (auto e : consumer) {
			int connect = e.first;
			int need = e.second;
			if (networkFlow(connect,superSink) < need) {
				cost = INFINITY;
				break;
			}
		}
		// cout << "calCost: " << cost << endl;
		return cost;
	}
	int calMSF(vector<bool> & serverpos) {
		MSFinit(serverpos);

		int ret = 0;
		while (true) {
			vector<int> dist(networkSize, INFINITY);
			vector<pair<int, bool>> path(networkSize, make_pair(-1, false)); // pair: (last node, is R path)
			
			SPFA(dist, path);  // find augmented path
			if (dist[superSink] == INFINITY)
				return ret;

			int flow = INFINITY;
			int tmp = superSink;
			int last = path[tmp].first;
			bool Rpath = path[tmp].second;
			while (last > 0) {
				int currVol;
				if (!Rpath)
					currVol = networkVol(last,tmp) - networkFlow(last,tmp);
				else currVol = RnetworkVol(last,tmp) - RnetworkFlow(last,tmp);
				flow = min(flow, currVol);
				tmp = last;
				last = path[tmp].first;
				Rpath = path[tmp].second;
			}
			// apply augmented path
			tmp = superSink;
			last = path[tmp].first;
			Rpath = path[tmp].second;
			while (last > 0) {
				if (!Rpath) {
					ret += flow * networkCost(last,tmp);
					networkFlow(last,tmp) += flow;
					RnetworkFlow(tmp,last) -= flow;
				}
				else {
					ret += flow * RnetworkCost(last,tmp);
					RnetworkFlow(last,tmp) += flow;
					networkFlow(tmp,last) -= flow;
				}
				tmp = last;
				last = path[tmp].first;
				Rpath = path[tmp].second;
			}
		} 

		return ret;
	}
//#define DEBUG_QUEUE
	template<typename T>
	class myq
	{
	public:
		T* q;
		int head, tail;
		int sz;
		myq(int _sz) :head(0), tail(0), sz(_sz)
		{
			q = new T[_sz];
		}
		void clear()
		{
			head = 0; tail = 0;
		}
		void push(T data)
		{
			q[tail] = data;
			tail++;
#ifdef DEBUG_QUEUE
			if (tail == head)
				abort();
#endif
			tail = tail%sz;
		}
		T pop()
		{
			T ret=q[head];
			head = (head+1)%sz;
			return ret;
		}
		bool empty()
		{
			return head == tail;
		}
	};

	void SPFA(vector<int> & dist, vector<pair<int, bool>> & path) {
		//queue<int> q;
		//vector<bool> inq(networkSize, false);
		static array<bool, MAX_NODES> inq;
		std::fill(inq.begin(), inq.begin() + networkSize, false);

		dist[superSource] = 0;
		inq[superSource] = true;
		static myq<int> q(networkSize);
		q.clear();
		q.push(superSource); // start from super source
		while (!q.empty()) {
			//int node = q.front();q.pop();
			int node = q.pop();
			inq[node] = false;

			for (int i = 0; i < networkSize; i++) {
				if (networkCost(node,i) == INFINITY)
					continue;
				if (networkVol(node,i) - networkFlow(node,i) == 0) // fully used edge
					continue;
				if (dist[i] > dist[node] + networkCost(node,i)) {
					dist[i] = dist[node] + networkCost(node,i);
					path[i] = make_pair(node, false);
					if (!inq[i]) {
						q.push(i);
						inq[i] = true;
					}
				}
			}
			for (int i = 0; i < networkSize; i++) {
				if (RnetworkCost(node,i) == INFINITY)
					continue;
				if (RnetworkVol(node,i) - RnetworkFlow(node,i) == 0) // fully used edge
					continue;
				if (dist[i] > dist[node] + RnetworkCost(node,i)) {
					dist[i] = dist[node] + RnetworkCost(node,i);
					path[i] = make_pair(node, true);
					if (!inq[i]) {
						q.push(i);
						inq[i] = true;
					}
				}
			}
		}
	}

	vector<int> getResult(vector<bool> & globalMinDim) {
		return vector<int>();
	}

	void printFlow() {
		for (int i = 0; i < networkSize; i++) {
			for (int j = 0; j < networkSize; j++) {
				cout << networkFlow(i,j) << " ";
			}
			cout << endl;
		}
	}
};

#endif