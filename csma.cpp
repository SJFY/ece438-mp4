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
//Transmission variance among all nodes
double trans_var = 0;
//Collision variance among all nodes
double colli_var = 0;

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
	for (long long int i = 0; i < T; i ++) {
		for (int i = 0; i < N; i ++) {
			if (nodes[i]->backoff == 0) {
				collisionNode_index.push_back(i);
			}
		}
		// printf("the size is %d\n", collisionNode_index.size());
	//	printf("the first index is %d\n",collisionNode_index[0]);
		int collisionNode_num = collisionNode_index.size();
		// int collisionNode_num = 0;
		if (collisionNode_num == 0) {
			//No collision!
			//Just decrement every node's backoff value by 1
			//The channel is idle, increase the channel_idle by 1
			allNodes_decre_backoff();
			channel_idle ++;
			// printf("all nodes backoff\n");
			// // test();
		}
		else if (collisionNode_num == 1) {
			//Start transmit!
			//Only one node reach 0 backoff, no collision. 
			//Other nodes don't decrement their backoff value in the mean time
			//Increase i += L - 1, for the entire transmission time
			//Reset the currentR, backoff and retrans_num to 0
			//Increase the transmission_num by 1
			if (L > T - i) {
				channel_utilization += T - i;
				i += T - i;
				break;
			}
			else {
			i += L - 1;
			
			int index = collisionNode_index[0];
			nodes[index]->transmission_num ++;
			nodes[index]->currentR = R[0];
			nodes[index]->backoff = rand()%(R[0] + 1);
			nodes[index]->retrans_num = 0;
			channel_utilization += L;
			// printf("bingo! no collision, will start transmission\n");
			// printf("index is %d\n", index );
			// printf("new transmission_num is %d and backoff is %d\n", nodes[index]->transmission_num, nodes[index]->backoff );
			// printf("the new i is %d \n",i );
			}
			// test();
		}
		else {
			// printf("collision happens\n");
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
			//Because the unused clock ticks do not include collisions
			// channel_idle ++;
			// test();
		}
		collisionNode_index.clear();
	}
	printf("simulation finished\n");
	printf("total_collisions is %ld\n", total_collisions);
	printf("channel_idle time is %ld\n", channel_idle);
	printf("channel_utilization time is %ld\n", channel_utilization );
}

void printOutput() {
	double trans_mean, colli_mean;
	double trans_sum, colli_sum;
	std::vector<int> trans;
	std::vector<int> colli;
	for (int i = 0; i < N; i ++) {
		trans.push_back(nodes[i]->transmission_num);
		colli.push_back(nodes[i]->collision_num);
		trans_sum += trans[i];
		colli_sum += colli[i];
		printf("node %d collision_num is %d transmission_num is %d\n", i, nodes[i]->collision_num, nodes[i]->transmission_num);
	}
	trans_mean = trans_sum/ (double) N;
	colli_mean = colli_sum/ (double) N;
	double tmp1 = 0;
	double tmp2 = 0;
	for (int i = 0; i < N; i ++) {
		tmp1 += (trans_mean - (double) trans[i]) * (trans_mean - (double) trans[i]);
		tmp2 += (colli_mean - (double) colli[i]) * (colli_mean - (double) colli[i]);
	}
	trans_var = tmp1 / (double) N;
	colli_var = tmp2 / (double) N;
	printf("trans_mean is %f\n", trans_mean );
	printf("colli_mean is %f\n", colli_mean );
	printf("trans_var is %f\n", trans_var );
	printf("colli_var is %f\n", colli_var);

	ofstream outfile;
	outfile.open("output.txt");
	outfile << "Channel utilization: " << ((double) channel_utilization / (double) T) * 100.0 << "%\n";
	outfile << "Channel idle fraction: " << ((double) channel_idle / (double) T) * 100.0 << "%\n";
	outfile << "Total number of collisions: " << total_collisions << "\n";
	outfile << "Variance in number of successful transmissions: " << trans_var << "\n";
	outfile << "Variance in number of collisions: " << colli_var << "\n";
	outfile.close();
}

void printGraph() {
	ofstream f1("plota.txt");
	ofstream f2("plotb.txt");
	ofstream f3("plotc.txt");
	ofstream f41("plotd1.txt");
	ofstream f42("plotd2.txt");
	ofstream f43("plotd3.txt");
	ofstream f44("plotd4.txt");
	ofstream f45("plotd5.txt");
	ofstream f51("plote1.txt");
	ofstream f52("plote2.txt");
	ofstream f53("plote3.txt");
	ofstream f54("plote4.txt");
	ofstream f55("plote5.txt");

	//Print f1, f2, f3
	for (int i = 5; i <= 100; i++) {
		R.clear();
		nodes.clear();
		N = i;
		L = 20;
		R.push_back(8);
		R.push_back(16);
		R.push_back(32);
		R.push_back(64);
		R.push_back(128);
		// R.push_back(1);
		// R.push_back(2);
		// R.push_back(4);
		// R.push_back(8);
		// R.push_back(16);
		M = 6;
		T = 50000;
		channel_utilization = 0;
		channel_idle = 0;
		total_collisions = 0;
		createNodes();
		simulation();
		f1 << "Channel utilization for the i " <<i<<" "<< ((double) channel_utilization / (double) T) * 100.0 << "%\n";
		f2 << "Channel idle fraction: " <<i<<" "<< ((double) channel_idle / (double) T) * 100.0 << "%\n";
		f3 << "Total number of collisions: " <<i<<" "<< total_collisions << "\n";
	}
for (int i = 5; i <= 100; i++) {
		R.clear();
		nodes.clear();
		N = i;
		L = 20;
	
		R.push_back(1);
		R.push_back(2);
		R.push_back(4);
		R.push_back(8);
		R.push_back(16);
		M = 6;
		T = 50000;
		channel_utilization = 0;
		channel_idle = 0;
		total_collisions = 0;
		createNodes();
		simulation();
		f41 << "Channel utilization for the i " <<i<<" "<< ((double) channel_utilization / (double) T) * 100.0 << "%\n";
		}
	for (int i = 5; i <= 100; i++) {
		R.clear();
		nodes.clear();
		N = i;
		L = 20;
		R.push_back(2);
		R.push_back(4);
		R.push_back(8);
		R.push_back(16);
		R.push_back(32);
		M = 6;
		T = 50000;
		channel_utilization = 0;
		channel_idle = 0;
		total_collisions = 0;
		createNodes();
		simulation();
		f42 << "Channel utilization: " <<i<<" "<< ((double) channel_utilization / (double) T) * 100.0 << "%\n";
	// 	f2 << "Channel idle fraction: " << ((double) channel_idle / (double) T) * 100.0 << "%\n";
	// 	f3 << "Total number of collisions: " << total_collisions << "\n";
	}
	for (int i = 5; i <= 100; i++) {
		R.clear();
		nodes.clear();
		N = i;
		L = 20;
		R.push_back(4);
		R.push_back(8);
		R.push_back(16);
		R.push_back(32);
		R.push_back(64);
		M = 6;
		T = 50000;
		channel_utilization = 0;
		channel_idle = 0;
		total_collisions = 0;
		createNodes();
		simulation();
		f43 << "Channel utilization: " <<i<<" "<< ((double) channel_utilization / (double) T) * 100.0 << "%\n";
	// 	f2 << "Channel idle fraction: " << ((double) channel_idle / (double) T) * 100.0 << "%\n";
	// 	f3 << "Total number of collisions: " << total_collisions << "\n";

	}
	for (int i = 5; i <= 100; i++) {
		R.clear();
		nodes.clear();
		N = i;
		L = 20;
		R.push_back(8);
		R.push_back(16);
		R.push_back(32);
		R.push_back(64);
		R.push_back(128);
		M = 6;
		T = 50000;
		channel_utilization = 0;
		channel_idle = 0;
		total_collisions = 0;
		createNodes();
		simulation();
		f44 << "Channel utilization: "<<i<<" " << ((double) channel_utilization / (double) T) * 100.0 << "%\n";
	// 	f2 << "Channel idle fraction: " << ((double) channel_idle / (double) T) * 100.0 << "%\n";
	// 	f3 << "Total number of collisions: " << total_collisions << "\n";
	}
	for (int i = 5; i <= 100; i++) {
		R.clear();
		nodes.clear();
		N = i;
		L = 20;
		R.push_back(16);
		R.push_back(32);
		R.push_back(64);
		R.push_back(128);
		R.push_back(256);
		M = 6;
		T = 50000;
		channel_utilization = 0;
		channel_idle = 0;
		total_collisions = 0;
		createNodes();
		simulation();
		f45 << "Channel utilization: " <<i<<" "<< ((double) channel_utilization / (double) T) * 100.0 << "%\n";
	// 	f2 << "Channel idle fraction: " << ((double) channel_idle / (double) T) * 100.0 << "%\n";
	// 	f3 << "Total number of collisions: " << total_collisions << "\n";
	}
	for (int i = 5; i <= 100; i++) {
		R.clear();
		nodes.clear();
		N = i;
		L = 20;
		R.push_back(8);
		R.push_back(16);
		R.push_back(32);
		R.push_back(64);
		R.push_back(128);
		M = 6;
		T = 50000;
		channel_utilization = 0;
		channel_idle = 0;
		total_collisions = 0;
		createNodes();
		simulation();
		f51 << "Channel utilization: " <<i<<" "<< ((double) channel_utilization / (double) T) * 100.0 << "%\n";
	// 	f2 << "Channel idle fraction: " << ((double) channel_idle / (double) T) * 100.0 << "%\n";
	// 	f3 << "Total number of collisions: " << total_collisions << "\n";
	}
	for (int i = 5; i <= 100; i++) {
		R.clear();
		nodes.clear();
		N = i;
		L = 40;
		R.push_back(8);
		R.push_back(16);
		R.push_back(32);
		R.push_back(64);
		R.push_back(128);
		M = 6;
		T = 50000;
		channel_utilization = 0;
		channel_idle = 0;
		total_collisions = 0;
		createNodes();
		simulation();
		f52 << "Channel utilization: " <<i<<" "<< ((double) channel_utilization / (double) T) * 100.0 << "%\n";
	// 	f2 << "Channel idle fraction: " << ((double) channel_idle / (double) T) * 100.0 << "%\n";
	// 	f3 << "Total number of collisions: " << total_collisions << "\n";
	}for (int i = 5; i <= 100; i++) {
		R.clear();
		nodes.clear();
		N = i;
		L = 60;
		R.push_back(8);
		R.push_back(16);
		R.push_back(32);
		R.push_back(64);
		R.push_back(128);
		M = 6;
		T = 50000;
		channel_utilization = 0;
		channel_idle = 0;
		total_collisions = 0;
		createNodes();
		simulation();
		f53 << "Channel utilization: "<<i<<" " << ((double) channel_utilization / (double) T) * 100.0 << "%\n";
	// 	f2 << "Channel idle fraction: " << ((double) channel_idle / (double) T) * 100.0 << "%\n";
	// 	f3 << "Total number of collisions: " << total_collisions << "\n";
	}for (int i = 5; i <= 100; i++) {
		R.clear();
		nodes.clear();
		N = i;
		L = 80;
		R.push_back(8);
		R.push_back(16);
		R.push_back(32);
		R.push_back(64);
		R.push_back(128);
		M = 6;
		T = 50000;
		channel_utilization = 0;
		channel_idle = 0;
		total_collisions = 0;
		createNodes();
		simulation();
		f54 << "Channel utilization: " <<i<<" "<< ((double) channel_utilization / (double) T) * 100.0 << "%\n";
	// 	f2 << "Channel idle fraction: " << ((double) channel_idle / (double) T) * 100.0 << "%\n";
	// 	f3 << "Total number of collisions: " << total_collisions << "\n";
	}for (int i = 5; i <= 100; i++) {
		R.clear();
		nodes.clear();
		N = i;
		L = 100;
		R.push_back(8);
		R.push_back(16);
		R.push_back(32);
		R.push_back(64);
		R.push_back(128);
		M = 6;
		T = 50000;
		channel_utilization = 0;
		channel_idle = 0;
		total_collisions = 0;
		createNodes();
		simulation();
		f55 << "Channel utilization: "<<i<<" " << ((double) channel_utilization / (double) T) * 100.0 << "%\n";
	// 	f2 << "Channel idle fraction: " << ((double) channel_idle / (double) T) * 100.0 << "%\n";
	// 	f3 << "Total number of collisions: " << total_collisions << "\n";
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
	printOutput();
	printGraph();
}
