# ifndef __PSO__
# define __PSO__

# include "Network.h"
# include <math.h>
# include <time.h>

# define C1 2
# define C2 2
# define INERTIA 0.729
# define ITER 300
# define PARTICLESIZE 100

class PSO {
private:
	int dimension;
	int maxServerNum;
	vector<int> particleCurr;
	Network::Mat<bool> particle;
	Network::Mat<float> velocity;

	vector<int> localMin;
	Network::Mat<bool> localMinDim;
	int globalMin;
	bool * globalMinDim;

	// vector<int> maxEachIter;
	// vector<vector<bool>> maxEachIterPos;
	template<typename T1,typename T2>
	static void copy_arr(T1 dest,T2 src, int sz)
	{
		for (int i = 0; i < sz; i++)
		{
			dest[i] = src[i];
		}
	}
	static int mrand()
	{
		static unsigned x = time(NULL)>>16;
		static unsigned y = time(NULL) & 0x0000ffff;
		static unsigned z = 123456789;
		x ^= x << 16;
		x ^= x >> 5;
		x ^= x << 1;

		unsigned t = x;
		x = y;
		y = z;
		z = t ^ x ^ y;
		return x & RAND_MAX;

	}
	void PSOinit(Network & network) {
		cout << "===== PSOinit =====" << endl;
		int gmaxpos = -1;
		// randomly init particles
		for (int i = 0; i < PARTICLESIZE; i++) {
			int serverNum = (mrand() % maxServerNum) + 1;
			for (int j = 0; j < serverNum; j++) {
				int randNode = mrand() % dimension;
				if (!particle[i][randNode]) {
					particle[i][randNode] = true;
					velocity[i][randNode] = (((double)mrand()) / RAND_MAX - 0.5);
				}
			}
			/*
			cout << "particle " << i << " solution:" << endl;
			for (int j = 0; j < dimension; j++) {
				cout << particle[i][j] << " ";
			}
			cout << endl; */
			// check
			/*
			for (int j = 0; j < dimension; j++)
				velocity[i][j] = (((double)mrand()) / RAND_MAX - 0.5);
				*/
			particleCurr[i] = network.calCost(particle[i]);
			// update local max and global max
			localMin[i] = particleCurr[i];
			copy_arr(localMinDim[i], particle[i], dimension);
			if (localMin[i] < globalMin) {
				globalMin = localMin[i];
				gmaxpos = i;
			}
		}
		if (gmaxpos > 0)
			copy_arr(globalMinDim, localMinDim[gmaxpos], dimension); 
		cout << "initial global min: " << globalMin << endl;
	}
public:
	PSO(Network & network) {
		dimension = network.getNumNode();
		maxServerNum = network.getNumConsumer();
		particleCurr = vector<int>(PARTICLESIZE, 0);
		particle .alloc(PARTICLESIZE, dimension, false);
		velocity .alloc(PARTICLESIZE, dimension, 0.0);

		localMin = vector<int>(PARTICLESIZE, INFINITY);
		localMinDim .alloc(PARTICLESIZE, dimension, false);
		globalMin = INFINITY;
		globalMinDim = new bool[dimension];
		memset(globalMinDim, sizeof(bool)*dimension, 0);

		// maxEachIter = vector<int>(ITER, -1);
		// maxEachIterPos = vector<vector<bool>>(ITER, vector<bool>(dimension, false));
	}
	int getGlobalMin() {
		return globalMin;
	}
	bool * getGlobalMinDim() {
		return globalMinDim;
	}

	void doPSO(Network & network) {
		PSOinit(network);

		double threshold;
		for (int i = 0; i < ITER; i++) {
			cout << "===== PSO ITERATION " << i+1 << " =====" << endl;
			// update each particle
			int gmaxpos = -1;
			for (int j = 0; j < PARTICLESIZE; j++) {
				for (int k = 0; k < dimension; k++) {
					// update speed
					float rand1 = (float)mrand() / RAND_MAX;
					float rand2 = (float)mrand() / RAND_MAX;
					float selfLearn = C1*rand1-1, globalLearn = C2*rand2-1;
					if (localMin[j] != INFINITY)
						selfLearn = C1*rand1*(localMinDim[j][k] - particle[j][k]);
					if (globalMin != INFINITY)
						globalLearn = C2*rand2*(globalMinDim[k] - particle[j][k]);
					velocity[j][k] = INERTIA*velocity[j][k] + selfLearn + globalLearn;
					// cout << "velocity update: " << rand1 << " " << rand2 << " " << velocity[j][k] << endl;

					if (velocity[j][k] > 1)
						velocity[j][k] = 1;
					else if (velocity[j][k] < -1)
						velocity[j][k] = -1;

					// update particle
					threshold = 1.0 / (1 + exp(-velocity[j][k]));
					if ((float)mrand() / RAND_MAX < threshold)
						particle[j][k] = 1;
					else particle[j][k] = 0;
				}
				/*
				cout << "particle " << j << " solution:" << endl;
				for (int m = 0; m < dimension; m++) {
					cout << particle[j][m] << " ";
				}
				cout << endl; */
				particleCurr[j] = network.calCost(particle[j]);
				if (particleCurr[j] < localMin[j]) {
					localMin[j] = particleCurr[j];
					copy_arr(localMinDim[j], particle[j],dimension);
				}
				if (particleCurr[j] < globalMin) {
					globalMin = particleCurr[j];
					gmaxpos = j;
				}
			}
			if (gmaxpos >= 0) {
				cout << "update global Min: " << globalMin << endl;
				copy_arr(globalMinDim , particle[gmaxpos],dimension);
			}
		}
	}
};

# endif