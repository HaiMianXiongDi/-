#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<windows.h>    //��ʱ��������

using namespace std;
# define maxnum 1000000  //���ݿ�����������100�����
# define m 5            //���B-��


int MAX = m - 1;          //���ؼ�������
int MIN = (m - 1) / 2;    //���ٹؼ�����


typedef struct node_ {
	int keynum;
	int key[m+1];          //[0]λ�ò�ʹ��
	struct node_* parent;
	struct node_* child[m+1];

}node,*btree;     //B���Ľڵ㶨��


typedef struct {                     //��B���еĲ��ҽ��
	node* pt;                 //ָ���ҵ��Ľ��ָ��
	int i;                          //�ڸý���еĹؼ���λ�ã�1��m) 
	int tag;                        //1�����ҳɹ���0������ʧ��
}Result;              //����B���Ĳ��ҽ��



//�����ڽڵ�P�У�Kֵ�������λ��i
int SearchBTNode(node* p, int k) {
	int i = 0;
	for (i = 0; i < p->keynum && p->key[i + 1] <= k; i++);
	return i;
}

//��B-���н�������
Result SearchBTree(btree t, int k) {
	node* p = t, * q = NULL;                            //pָ����ڵ���,qָ��p�ĸ���
	int found_tag = 0;                                //���ҳɹ�����־ 
	int i = 0;
	Result r;                                       

	while (p != NULL && found_tag == 0) {

		i = SearchBTNode(p, k);                        //�ڽ��p�в��ҹؼ���k,��ǰ�ڵ��key[i+1]Ϊ��һ������kֵ�Ĺؼ��� 
		
		if (i > 0 && p->key[i] == k)
		found_tag = 1;                            
		else {                                       
		q = p;
		p = p->child[i];
		}
	}

	if (found_tag == 1) {                               //���ҳɹ�
		r.pt = p;
		r.i = i;
		r.tag = 1;
	}
	else {                                           //����ʧ��
		r.pt = q;
		r.i = i;
		r.tag = 0;
	}
	return r;                                       //���ؽ��
}               //Result���͵�B�����Һ���������Result����


//�����µĸ����t,ԭ���p�ͽ��qΪ����ָ�루��������ʱp,q����NULL����)
void NewRoot(node *& t, int k, node* p, node* q) {
	t = (node*)malloc(sizeof(node));             //����ռ� 
	t->keynum = 1;
	t->child[0] = p;
	t->child[1] = q;
	t->key[1] = k;
	if (p != NULL)                                     //����˫�׽ڵ�ָ��t
		p->parent = t;
	if (q != NULL)
		q->parent = t;
	t->parent = NULL;
}



//���ؼ���k�ͽ��q�ֱ�ŵ����ڵ�P�ģ�key[i+1]��child[i+1]��
void InsertBTNode(node*& p, int i, int k, node* q) {
	int j;
	for (j = p->keynum; j > i; j--) {                       //��������ܣ�ʵ�ֲ���
		p->key[j + 1] = p->key[j];
		p->child[j + 1] = p->child[j];
	}
	p->key[i + 1] = k;
	p->child[i + 1] = q;
	if (q != NULL)
		q->parent = p;
	p->keynum++;
}



//�����p���ѳ��������,ǰһ�뱣��,��һ��������q,���ؼ������������ֵʱ����
void SplitBTNode(node*& p, node*& q) {
	int i;
	int s = (m + 1) / 2;          //���ӽڵ�һ���λ����
	q = (node*)malloc(sizeof(node));             //�����q����ռ�

	q->child[0] = p->child[s];                            //��һ��������q
	for (i = s + 1; i <= m; i++) {
		q->key[i - s] = p->key[i];
		q->child[i - s] = p->child[i];
	}
	q->keynum = p->keynum - s;
	q->parent = p->parent;
	for (i = 0; i <= p->keynum - s; i++)                         //�޸�˫��ָ�� 
		if (q->child[i] != NULL)
			q->child[i]->parent = q;
	p->keynum = s - 1;                              //���p����һ��,���p��keynum�޸�
}



//����t�Ͻ��q��key[i]��key[i+1]֮�����ؼ���k��������������ѣ�pΪ������λ�õĽڵ�ָ��
void InsertBTree(btree& t, int i, int k, node* p) {
	node* q;
	int finish_tag, newroot_tag, s;                   //�趨��Ҫ�½���־�Ͳ�����ɱ�־ 
	int x;
	if (p == NULL)                                     //t�ǿ���
		NewRoot(t, k, NULL, NULL);                     //���ɽ����ؼ���k�ĸ����t
	else {
		x = k;
		q = NULL;
		finish_tag = 0;
		newroot_tag = 0;

		while (finish_tag == 0 && newroot_tag == 0) {
			InsertBTNode(p, i, x, q);                  //���ؼ���x�ͽ��q�ֱ���뵽p->key[i+1]��p->child[i+1]
			if (p->keynum <= MAX)
				finish_tag = 1;                       //�������


			else {
				s = (m + 1) / 2;
				SplitBTNode(p, q);                 //���ѽ�� 
				x = p->key[s];

				if (p->parent) {                      //����x�Ĳ���λ��
					p = p->parent;
					i = SearchBTNode(p, x);
				}
				else                            //û�ҵ�x����Ҫ�µĸ���㣬�������Ӧ�ûᷢ���ڸ��ڵ���Ѻ�
					newroot_tag = 1;

			}


		}
		if (newroot_tag == 1)                      //������ѷ���Ϊ���p��q 
			NewRoot(t, x, p, q);                   //�����¸����t,p��qΪ����ָ��
	}
}



int database[maxnum];   //���ݿ�����
LARGE_INTEGER nFreq;        //64λ�з������������ڼ�ʱ����ȷ��΢�룩
LARGE_INTEGER t1;
LARGE_INTEGER t2;
double costtime;
/*
QueryPerformanceFrequency(&nFreq);          //��ȡ����ʱ������
QueryPerformanceCounter(&t1);

........

QueryPerformanceCounter(&t2);
costtime = (double)(t2.QuadPart - t1.QuadPart) / (double)nFreq.QuadPart;
cout << "��ʱ��" << costtime << "��";*/

//��ʱ�����ĸ�ʽ




//��ʼ�����ݿ⣬��100������鵥Ԫ�ֱ�ֵΪ1~1000000
void initedata(int* a) {
	for (int i = 0; i < maxnum; i++) 
		a[i] = i+1;
}  


void BTree_Searchfun(int target) {

	
	node* t = NULL;
	Result s;
	int j,n = maxnum;


	for (j = 0; j < n; j++) {
		s = SearchBTree(t, database[j]);
		if (s.tag == 0)
			InsertBTree(t, s.i, database[j], s.pt);
	}


//============================================================

	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&t1);

	s = SearchBTree(t, target);
	
	QueryPerformanceCounter(&t2);
	costtime = (double)(t2.QuadPart - t1.QuadPart) / (double)nFreq.QuadPart;
	cout << "���ݿ�����������ʱ��" << costtime << "��"<<endl;

	cout << "���ҵ�������ֵ��"<<s.pt->key[s.i];

}
//�������ݿ�����

void Sequence_Searchfun(int target) {
	initedata(database);
	int i;

//=========================================================
	

	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&t1);


	for (i = 0; i < maxnum; i++) {
		if (database[i] == target)
			break;
	}

	QueryPerformanceCounter(&t2);
	costtime = (double)(t2.QuadPart - t1.QuadPart) / (double)nFreq.QuadPart;
	cout << "˳��������Һ�ʱ��" << costtime << "��" << endl;

	cout << "���ҵ�������ֵ��" << database[i];

	}
//��˳���������

void Bin_Searchfun(int target) {
	int first = 0;
	int last = maxnum - 1;
	int mid;

	//=================================================================
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&t1);

	while (first <= last) {
		mid = (first + last) / 2;
		if (database[mid] > target)
			last = mid - 1;
		else if (database[mid] < target)
			first = mid + 1;
		else
			break;
	}

	QueryPerformanceCounter(&t2);
	costtime = (double)(t2.QuadPart - t1.QuadPart) / (double)nFreq.QuadPart;
	cout << "���ַ����Һ�ʱ��" << costtime << "��" << endl;

	cout << "���ҵ�������ֵ��" << database[mid];
}
//�����ַ���������





int main() {
	initedata(database);
	int target;
	
	cout << "����������ҵ����ݣ�1��100��֮�䣩" << endl;

	while (scanf_s("%d", &target) != EOF) {
		
		if (target < 1 || target>1000000) {
			cout << "�������ݴ���" << endl;
			cout << endl << "<------------------------------------------->" << endl;
			cout << "����������ҵ����ݣ�1��100��֮�䣩" << endl;
			continue;
		}
			


		cout << endl << endl;
		BTree_Searchfun(target);

		cout << endl << endl;
		Bin_Searchfun(target);

		cout << endl << endl;
		Sequence_Searchfun(target);

		cout <<endl<< "<------------------------------------------->" << endl;
		cout << "����������ҵ����ݣ�1��100��֮�䣩" << endl;
		
	}
	return 0;
}








