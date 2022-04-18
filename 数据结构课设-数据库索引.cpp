#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<windows.h>    //计时函数所用

using namespace std;
# define maxnum 1000000  //数据库数据数量（100万个）
# define m 5            //五阶B-树


int MAX = m - 1;          //最大关键字数量
int MIN = (m - 1) / 2;    //最少关键字数


typedef struct node_ {
	int keynum;
	int key[m+1];          //[0]位置不使用
	struct node_* parent;
	struct node_* child[m+1];

}node,*btree;     //B树的节点定义


typedef struct {                     //在B树中的查找结果
	node* pt;                 //指向找到的结点指针
	int i;                          //在该结点中的关键字位置（1到m) 
	int tag;                        //1；查找成功，0；查找失败
}Result;              //代表B树的查找结果



//返回在节点P中，K值将插入的位置i
int SearchBTNode(node* p, int k) {
	int i = 0;
	for (i = 0; i < p->keynum && p->key[i + 1] <= k; i++);
	return i;
}

//在B-树中进行搜索
Result SearchBTree(btree t, int k) {
	node* p = t, * q = NULL;                            //p指向根节点结点,q指向p的父亲
	int found_tag = 0;                                //查找成功与否标志 
	int i = 0;
	Result r;                                       

	while (p != NULL && found_tag == 0) {

		i = SearchBTNode(p, k);                        //在结点p中查找关键字k,当前节点的key[i+1]为第一个大于k值的关键字 
		
		if (i > 0 && p->key[i] == k)
		found_tag = 1;                            
		else {                                       
		q = p;
		p = p->child[i];
		}
	}

	if (found_tag == 1) {                               //查找成功
		r.pt = p;
		r.i = i;
		r.tag = 1;
	}
	else {                                           //查找失败
		r.pt = q;
		r.i = i;
		r.tag = 0;
	}
	return r;                                       //返回结果
}               //Result类型的B树查找函数，返回Result类型


//生成新的根结点t,原结点p和结点q为子树指针（空树建立时p,q传入NULL即可)
void NewRoot(node *& t, int k, node* p, node* q) {
	t = (node*)malloc(sizeof(node));             //分配空间 
	t->keynum = 1;
	t->child[0] = p;
	t->child[1] = q;
	t->key[1] = k;
	if (p != NULL)                                     //子树双亲节点指向t
		p->parent = t;
	if (q != NULL)
		q->parent = t;
	t->parent = NULL;
}



//将关键字k和结点q分别放到到节点P的，key[i+1]和child[i+1]中
void InsertBTNode(node*& p, int i, int k, node* q) {
	int j;
	for (j = p->keynum; j > i; j--) {                       //整体往后窜，实现插入
		p->key[j + 1] = p->key[j];
		p->child[j + 1] = p->child[j];
	}
	p->key[i + 1] = k;
	p->child[i + 1] = q;
	if (q != NULL)
		q->parent = p;
	p->keynum++;
}



//将结点p分裂成两个结点,前一半保留,后一半移入结点q,当关键字数超出最大值时发生
void SplitBTNode(node*& p, node*& q) {
	int i;
	int s = (m + 1) / 2;          //孩子节点一半的位置嗯
	q = (node*)malloc(sizeof(node));             //给结点q分配空间

	q->child[0] = p->child[s];                            //后一半移入结点q
	for (i = s + 1; i <= m; i++) {
		q->key[i - s] = p->key[i];
		q->child[i - s] = p->child[i];
	}
	q->keynum = p->keynum - s;
	q->parent = p->parent;
	for (i = 0; i <= p->keynum - s; i++)                         //修改双亲指针 
		if (q->child[i] != NULL)
			q->child[i]->parent = q;
	p->keynum = s - 1;                              //结点p保留一半,结点p的keynum修改
}



//在树t上结点q的key[i]与key[i+1]之间插入关键字k。若结点过大则分裂，p为待插入位置的节点指针
void InsertBTree(btree& t, int i, int k, node* p) {
	node* q;
	int finish_tag, newroot_tag, s;                   //设定需要新结点标志和插入完成标志 
	int x;
	if (p == NULL)                                     //t是空树
		NewRoot(t, k, NULL, NULL);                     //生成仅含关键字k的根结点t
	else {
		x = k;
		q = NULL;
		finish_tag = 0;
		newroot_tag = 0;

		while (finish_tag == 0 && newroot_tag == 0) {
			InsertBTNode(p, i, x, q);                  //将关键字x和结点q分别插入到p->key[i+1]和p->child[i+1]
			if (p->keynum <= MAX)
				finish_tag = 1;                       //插入完成


			else {
				s = (m + 1) / 2;
				SplitBTNode(p, q);                 //分裂结点 
				x = p->key[s];

				if (p->parent) {                      //查找x的插入位置
					p = p->parent;
					i = SearchBTNode(p, x);
				}
				else                            //没找到x，需要新的根结点，这种情况应该会发生在根节点分裂后
					newroot_tag = 1;

			}


		}
		if (newroot_tag == 1)                      //根结点已分裂为结点p和q 
			NewRoot(t, x, p, q);                   //生成新根结点t,p和q为子树指针
	}
}



int database[maxnum];   //数据库数组
LARGE_INTEGER nFreq;        //64位有符号整数，用于计时（精确到微秒）
LARGE_INTEGER t1;
LARGE_INTEGER t2;
double costtime;
/*
QueryPerformanceFrequency(&nFreq);          //获取机器时钟周期
QueryPerformanceCounter(&t1);

........

QueryPerformanceCounter(&t2);
costtime = (double)(t2.QuadPart - t1.QuadPart) / (double)nFreq.QuadPart;
cout << "耗时：" << costtime << "秒";*/

//计时函数的格式




//初始化数据库，将100万个数组单元分别赋值为1~1000000
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
	cout << "数据库索引搜索耗时：" << costtime << "秒"<<endl;

	cout << "查找到的数据值："<<s.pt->key[s.i];

}
//索引数据库搜索

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
	cout << "顺序遍历查找耗时：" << costtime << "秒" << endl;

	cout << "查找到的数据值：" << database[i];

	}
//按顺序遍历搜索

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
	cout << "二分法查找耗时：" << costtime << "秒" << endl;

	cout << "查找到的数据值：" << database[mid];
}
//按二分法查找搜索





int main() {
	initedata(database);
	int target;
	
	cout << "请输入待查找的数据（1到100万之间）" << endl;

	while (scanf_s("%d", &target) != EOF) {
		
		if (target < 1 || target>1000000) {
			cout << "输入数据错误！" << endl;
			cout << endl << "<------------------------------------------->" << endl;
			cout << "请输入待查找的数据（1到100万之间）" << endl;
			continue;
		}
			


		cout << endl << endl;
		BTree_Searchfun(target);

		cout << endl << endl;
		Bin_Searchfun(target);

		cout << endl << endl;
		Sequence_Searchfun(target);

		cout <<endl<< "<------------------------------------------->" << endl;
		cout << "请输入待查找的数据（1到100万之间）" << endl;
		
	}
	return 0;
}








