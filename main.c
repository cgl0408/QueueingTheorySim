#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>

/*
��J
S = server���ƶq
K = server + queue���ƶq 
lambda = �ƥ��F�v
mu = �ƥ�B�z�v
lambda �P mu�Үھ�Poission������F 
SIM_TIME = ����ɶ� 
*/

int S = 9;
int K = 25;
double lambda = 60.0;
double mu = 7.5;
int SIM_TIME = 1000000;

/*
��X
�i�P�H�U�������Ѫ���������
https://people.revoledu.com/kardi/tutorial/Queuing/MMsN-Queuing-System.html 
L = �����ƥ�b�t�θ̪��ƶq
LQ = �����ƥ�bqueue�̪��ƶq
W = ���������ƥ󪺮ɶ� 
WQ = �����ƥ�qqueue�i�J�t�Ϊ��ɶ� 
*/

double L = 0.0;
double W = 0.0;
double LQ = 0.0;
double WQ = 0.0;


typedef struct{
	double arrival_time;
	double exit_time;
	double process_time;
	struct event* next;	
}event;


double exponentialRandom(double lambda) {
	double tmp = -log(1.0 - ((double) rand() / RAND_MAX)) / lambda;
	while(isinf(tmp)) tmp = -log(1.0 - ((double) rand() / RAND_MAX)) / lambda;
    return tmp;
}
double max(double num1, double num2) {
    return (num1 > num2) ? num1 : num2;
}

event* init_events(int SIM_TIME, double lambda, double mu){
	srand(time(NULL));
	unsigned seed1 = (unsigned)time(NULL);
	srand(seed1);
	double last_arrival = 0;
	double inter_arrival, inter_process;
	int c = 0;
	event* head = (event*)malloc(sizeof(event));
	event* tmp = head;
	
	while(last_arrival < SIM_TIME){
		inter_arrival = exponentialRandom(lambda);
		inter_process = exponentialRandom(mu);
		//printf("�ƥ��%f�ɩ�F\n",last_arrival);
		event* e = (event*)malloc(sizeof(event));
		e->next = NULL;
		e->arrival_time = last_arrival;
		e->process_time = inter_process;
		tmp->next = e;
		tmp = tmp->next;
		last_arrival+=inter_arrival;
		c+=1;
	}
	printf("�ƥ�ƶq��%d��\n",c);
	return head;
}

void sim_run(event* e, int C){
	event* retrieve_e = NULL;
	double* servers_time = (double*)calloc(S, sizeof(double));
	int num_servers = 0, cur_c = 0, cur_s;//��e�i��server��queue���j�p
	double min_ele = 0, prev_arrival_time = 0;
	int num_processed = 0;
	while(e){
		//printf("�ƥ��F�ɶ� = %f\n",e->arrival_time);
		cur_c = cur_s = 0;
		
		int min_idx = 0;
		min_ele = servers_time[0];
	
		int i;
		for(i = 0; i < S; i++){
			if(min_ele > servers_time[i]){
				min_ele = servers_time[i];
				min_idx = i;
			}
		}
		//printf("%f\n",min_ele);
		for(i = 0; i < S; i++){
			if(min_ele < servers_time[i]){
				cur_s+=1;
			}
		}
		event* tmp = e;
		event* last_avl_e = e;// last_avl_e ��e�bqueue�̪��̷s���� 
		
		while(tmp && tmp->arrival_time < min_ele){//�̦�������server���ɶ����ߩ�ƥ��F�ɶ� => �iqueue 
			cur_c+=1;
			if(cur_c > C){// queue���F 
				retrieve_e = tmp;
				cur_c-=1;
				//printf("drop event\n");
			}
			else{		
				last_avl_e = tmp;
			} 
			tmp = tmp->next;
			//free(retrieve_e);
		}
		
		if(last_avl_e != tmp){
			last_avl_e->next = tmp;
		}
		
		//printf("queue size = %d ",cur_c);
		servers_time[min_idx] = max(e->arrival_time, min_ele) + e->process_time;
		e->exit_time = servers_time[min_idx];
		
		
		L+=(e->arrival_time - prev_arrival_time) * (cur_c + cur_s);
		LQ+=(e->arrival_time - prev_arrival_time) * cur_c;
		W+=e->exit_time - e->arrival_time; 
		WQ+=e->exit_time - e->arrival_time - e->process_time;
		num_processed+=1;
		prev_arrival_time = e->arrival_time;
		retrieve_e = e;
		e = e->next;
		free(retrieve_e);
		//break;
	}
	W/=num_processed;//�̫ᰣ�W�B�z���\���ƥ�ƶq
	WQ/=num_processed;
	L/=SIM_TIME;//���W����ɶ� 
	LQ/=SIM_TIME;
	printf("L, LQ, W, WQ = %f, %f, %f, %f \n", L,LQ, W, WQ);
}


int main(int argc, char *argv[])
{
	
	int C = K - S;//queue capacity
	event* head = init_events(SIM_TIME, lambda, mu);
	sim_run(head->next, C);
	
	printf("����\n");
	
  	return 0;
}
