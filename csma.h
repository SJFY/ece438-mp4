#ifndef CSMA_H
#define CSMA_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <vector>
#include <stdlib.h>

using namespace std;

typedef struct Node{
	//total collision number
	int collision_num;
	//successful transmissions number 
	int transmission_num; 
	//backoff value
	int backoff; 
	//current R value, randomly choose backoff value within this range 
	int currentR;
	//retransmit number, if this reaches M, need to drop packet
	int retrans_num; 
}Node;
int readhelper(char wheretostore, int val);
void readinput(string fname);
//void create_nodes();
//void decrement_all_backoffs();
//void getnewR(int nodekey);
//void getnewBackoff(int nodekey);
//void runsimulation();
//void getResults();
//double calc_variance(vector<int> &data);
//void printNodes();
//void printData(vector<void> &data, string fname);
//void test_variance();
//void printdData();

#endif
