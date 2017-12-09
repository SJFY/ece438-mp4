#include "csma.h"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <queue>
#include <vector>
#include <unordered_map>
#include <time.h>

using namespace std;
//N is number of nodes, L is packet length, M is maximum collision count
int N, L, M;
long long int T;
vector<int> R;
// unordered_map<int, Node*> nodes;
std::vector<Node*> nodes;


//Total collision number
long int total_collisions = 0;
//Total channel utilization time
long int channel_utilization = 0;
//Total channel idle time
long int channel_idle = 0;

void readFile(string fname) {
	ifstream infile(fname.c_str());
	string line;

	if (infile.is_open()) {
		while (getline(infile, line)) {
			// cout << line <<'\n';
			char inputchar;
			sscanf(line.c_str(), "%c", &inputchar);
			line = line.substr(line.find(" ") + 1);
			stringstream ss;
			ss.str("");
			ss.clear();

			ss << line;
			string temp;
			int test;

			while(getline(ss, temp, ' ')){
				if(stringstream(temp) >>  test)
					test = stoi(temp); //i think this is redundant
					if(!readhelper(inputchar, test))
						fprintf(stderr, "input file has invalid format\n");
			}
		}
		infile.close();
	}
}

int readhelper(char wheretostore, int val){
	switch(wheretostore){
		case 'N':
			N = val;
			break;
		case 'L':
			L = val;
			break;
		case 'M':
			M = val;
			break;
		case 'T':
			T = val;
			break;
		case 'R':
			R.push_back(val);
			break;
		default:
			return 0;
	}
	return 1;
}

void createNodes() {
	for (int i = 0; i < N; i ++) {
		Node *tmp = new Node;
		tmp->collision_num = 0;
		tmp->transmission_num = 0;
		tmp->currentR = R[0];
		tmp->backoff = rand()%(tmp->currentR + 1);
		tmp->retrans_num = 0;
		nodes.push_back(tmp);
		printf("no %d backoff is %d\n",i, tmp->backoff );
	}
	// printf("node backoff is %d\n", nodes[10]->backoff);
}

void allNodes_decre_backoff() {
	for (int i = 0; i < N; i ++) {
		nodes[i]->backoff --;
	}
}

void test() {
	for (int i = 0; i < N; i ++) {
		printf("no %d backoff is %d current R is %d\n",i, nodes[i]->backoff, nodes[i]->currentR );	
	}
}

int getNewR_backoff(int currentR) {
	bool found = false;
	int i = 0;
	for (i = 0; i < R.size(); i ++) {
		if (R[i] == currentR && R[i] != R.back()) {
			found = true;
			break;
		}
	}
	if (found) {
		return R[i+1];
	}
	else {
		return 2 * currentR;
	}
}

void simulation() {
	srand (time(NULL));
	std::vector<int> collisionNode_index;
	for (long long int i = 0; i < 100; i ++) {
		for (int i = 0; i < N; i ++) {
			if (nodes[i]->backoff == 0) {
				collisionNode_index.push_back(i);
			}
		}
		printf("the size is %d\n", collisionNode_index.size());
	//	printf("the first index is %d\n",collisionNode_index[0]);
		int collisionNode_num = collisionNode_index.size();
		// int collisionNode_num = 0;
		if (collisionNode_num == 0) {
			//No collision!
			//Just decrement every node's backoff value by 1
			//The channel is idle, increase the channel_idel by 1
			allNodes_decre_backoff();
			channel_idle ++;
			printf("all nodes backoff\n");
			test();
		}
		else if (collisionNode_num == 1) {
			//Start transmit!
			//Only one node reach 0 backoff, no collision. 
			//Other nodes don't decrement their backoff value in the mean time
			//Increase i += L - 1, for the entire transmission time
			//Reset the currentR, backoff and retrans_num to 0
			//Increase the transmission_num by 1
			i += L - 1;
			
			int index = collisionNode_index[0];
			nodes[index]->transmission_num ++;
			nodes[index]->currentR = R[0];
			nodes[index]->backoff = rand()%(R[0] + 1);
			nodes[index]->retrans_num = 0;
			channel_utilization += L;
			printf("bingo! no collision, will start transmission\n");
			printf("index is %d\n", index );
			printf("new transmission_num is %d and backoff is %d\n", nodes[index]->transmission_num, nodes[index]->backoff );
			printf("the new i is %d \n",i );
			test();
		}
		else {
			printf("collision happens\n");
			//Collision happens!
			//For all the collision nodes, double their currentR, increase retrans_num by 1, recalculate backoff value
			//For all the collision nodes, increase thier collision_num by 1
			//Increase the total_collisions by 1
			//For all the other nodes, decrement their backoff value by 1
			allNodes_decre_backoff();
			for (int i = 0; i < collisionNode_index.size(); i ++) {
				int index = collisionNode_index[i];
				nodes[index]->retrans_num ++;
				if (nodes[index]->retrans_num >= M) {
					//Reset to 0
					nodes[index]->currentR = R[0];
					nodes[index]->backoff = rand()%(R[0] + 1);
					nodes[index]->retrans_num = 0;
				}
				else {
					nodes[index]->currentR = getNewR_backoff(nodes[index]->currentR);
					nodes[index]->backoff = rand()%(nodes[index]->currentR + 1);
				}
				nodes[index]->collision_num ++;
			}
			total_collisions ++;
			test();
		}
		collisionNode_index.clear();
	}
}
int main(int argc, char** argv){

	if(argc != 2){
		fprintf(stderr, "usage: %s inputfile\n", argv[0]);
		exit(1);
	}
	string fileName;
	fileName = argv[1];
	srand (time(NULL));
	readFile(fileName);
	printf("Channel Used: %d\n", L);
	createNodes();
	simulation();
	// create_nodes();
	// runsimulation();
	// getResults();
	// printGraphData();
}
