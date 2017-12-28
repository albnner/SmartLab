#ifndef TS_H_INCLUDED
#define TS_H_INCLUDED

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <time.h>

using namespace std;

class Neighborstep{
    public:
    int v;   //动作顶点
    int si;  //原颜色
    int di;  //目标颜色

    Neighborstep(int v1, int si1, int di1);
    Neighborstep();

};

void Inde_To_Arr( vector <int> * );
void Arr_To_Inde( vector <int> * );
void Color_Array_Generate();
void AdjacentColor_Table_Init();
void ConflictNum_Init();
void Tabu_Init();
void Move();
void Update(Neighborstep &, int );
int Conflict_Calculate(vector <int> *);
int Tabu_Search(vector <int> *);

#endif // TS_H_INCLUDED
