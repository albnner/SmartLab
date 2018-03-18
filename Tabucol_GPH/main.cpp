#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <ctype.h>

using namespace std;

char *file_name;
int N,E,K,CF,CF_best;
int **Graph;
int *Color;
int **Adjacent_Color_Table;
int **Tabu_List;
int iter=0, max_iter=3000000;
vector <int> Conflict_Vertice;           //记录冲突节点
int *Conflict_Vertice_Position;         //记录冲突节点在数组中的位置
int length;                              //冲突节点表的长度
//int iter_temp = 0;
//double ** f_repeat;
//double max_f = 0;

double runtime;


class Neighborstep
{
public:
    int v;   //动作顶点
    int si;  //原颜色
    int di;  //目标颜色

    Neighborstep(int v1, int si1, int di1)
    {
        v = v1;
        si = si1;
        di = di1;
    }

    Neighborstep()
    {
        v=0;
        si=0;
        di=0;
    }

};

Neighborstep step_best;

void Graph_Generate()
{
    int i,j,v1,v2;
    ifstream F;
    F.open(file_name);
    if( F.fail() )
    {
        cout << file_name << " " << "FILE OPEN FAILED!" << endl;
        exit(0);
    }
    char blank1[100],blank2[50];
    F >> blank1;
    while( !F.eof() )
    {
        if( strcmp( blank1, "p") == 0)
        {
            F >> blank2 >> N >> E;
            cout << "Number of vertexes:" << N << endl;
            cout << "Number of edges:" << E << endl;
            //生成图邻接矩阵
            Graph = new int *[N];
            for(i=0; i<N; i++)
            {
                Graph[i] = new int[N];
            }
            for(i=0; i<N; i++)
            {
                for(j=0; j<N; j++)
                    Graph[i][j] = 0;
            }
            //建立图
            for(i=0; i<E; i++)
            {
                F>>blank2;
                F>>v1>>v2;
                Graph[v1-1][v2-1] = 1;
                Graph[v2-1][v1-1] = 1;
            }
        }
        F >> blank1;
    }
    //cout<<"Graph[0][1]:"<<Graph[0][1]<<endl;
    //cout<<"Graph[6][3]:"<<Graph[6][3]<<endl;
}



void ColorArray_Generate()
{
    int i;
    //生成顶点颜色对应向量
    Color = new int[N];
    //随机初始化向量
    for(i=0; i<N; i++)
    {
        //srand (time(NULL));
        Color[i] = rand() % K;
    }
}

void AdjacentColor_Table_Init()
{
    //生成颜色冲突表
    Adjacent_Color_Table = new int *[N];
    for(int i=0; i<N; i++)
    {
        Adjacent_Color_Table[i] = new int[K];
    }
    for(int i=0; i<N; i++)
        for(int j=0; j<K; j++)
            Adjacent_Color_Table[i][j] = 0;
    //初始化冲突表
    for(int i=0; i<N; i++)
    {

        for(int m=0; m<K; m++)
        {
            for(int j=0; j<N; j++)
            {
                if( Graph[i][j]==1 && Color[j]==m )
                {
                    Adjacent_Color_Table[i][m]++;
                }
            }
        }
    }
    //初始化重复频率表

}

void Confilct_Vertice_Vector_Init()
{
    Conflict_Vertice_Position = new int [N];
    int l = 0;  //冲突节点表长度

    for( int i=0; i<N; i++ )
    {
        if( Adjacent_Color_Table[i][Color[i]] != 0 )
        {
            Conflict_Vertice.push_back( i );
            Conflict_Vertice_Position[i] = l;
            l++;
        }
    }
    length = Conflict_Vertice.size();
}


void ConflictNum_Init()
{
    CF=0;
    int i;
    for(i=0; i<N; i++)
    {
        CF = CF + Adjacent_Color_Table[i][Color[i]];
    }
    CF = CF / 2;
    CF_best = CF;
}

void Tabu_Init()
{
    int i,j;
    Tabu_List = new int *[N];
    for(i=0; i<N; i++)
    {
        Tabu_List[i] = new int[K];
    }
    for(i=0; i<N; i++)
    {
        for(j=0; j<K; j++)
        {
            Tabu_List[i][j] = 0;
        }
    }
}


void Move()
{
    vector <Neighborstep> step_best_nt, step_best_t;
    void Update(Neighborstep &step_move, int delta_ture);
    int delta_Tabu=0,delta_NoTabu=0,delta_Tabu_min=K,delta_NoTabu_min=K;
    //Neighborstep step_best;
    for( int i=0; i<length; i++ )
    {
//        if( Adjacent_Color_Table[i][Color[i]] != 0 )   //冲突数不为0则需动作
//        {
        int c_v = Conflict_Vertice[ i ];     //冲突节点
        int j=0;
        while(j<K)
        {
            if( j==Color[c_v] )
            {
                j++;
                continue;
            }
            if( Tabu_List[c_v][j] > iter)              //是否为禁忌的动作
            {
                delta_Tabu = Adjacent_Color_Table[c_v][j] - Adjacent_Color_Table[c_v][Color[c_v]];
                if( delta_Tabu < delta_Tabu_min )           //存储禁忌动作的最优值
                {
                    delta_Tabu_min = delta_Tabu;
                    step_best_t.clear();
                    step_best_t.push_back(Neighborstep(c_v, Color[c_v], j));
                }
                else if( delta_Tabu == delta_Tabu_min )
                {
                    step_best_t.push_back(Neighborstep(c_v, Color[c_v], j));
                }
            }
            else
            {
                delta_NoTabu = Adjacent_Color_Table[c_v][j] - Adjacent_Color_Table[c_v][Color[c_v]];
                if( delta_NoTabu < delta_NoTabu_min )        //存储非禁忌动作的最优值
                {
                    delta_NoTabu_min = delta_NoTabu;
                    step_best_nt.clear();
                    step_best_nt.push_back(Neighborstep(c_v, Color[c_v], j));
                }
                else if( delta_NoTabu == delta_NoTabu_min )
                {
                    step_best_nt.push_back(Neighborstep(c_v, Color[c_v], j));
                }
            }
            j++;
        }
//        }
    }
    iter++;
    if( delta_Tabu_min + CF < CF_best && delta_Tabu_min < delta_NoTabu_min )       //释放准则：禁忌动作是历史最优解
    {
        int k = (rand() % step_best_t.size());
        step_best = step_best_t[k];
        Update(step_best, delta_Tabu_min);
    }
    else                                              //按非禁忌最优动作进行动作
    {
        int k = (rand() % step_best_nt.size());
        step_best = step_best_nt[k];
        Update(step_best, delta_NoTabu_min);
    }
}

void Update(Neighborstep &step_move, int delta)
{
    //更新Adjacent_Color_Table
    for( int i=0; i<N; i++)
    {
        if( Graph[step_move.v][i] )
        {
            Adjacent_Color_Table[i][step_move.si]--;
            Adjacent_Color_Table[i][step_move.di]++;
            //更新冲突结点表
            if( step_move.si == Color[i] && Adjacent_Color_Table[i][Color[i]] == 0 )
            {
                int p1 = Conflict_Vertice_Position[ i ];
                int p_last = Conflict_Vertice.back();
                Conflict_Vertice[ Conflict_Vertice_Position[i] ] = Conflict_Vertice.back();
                Conflict_Vertice.pop_back();
                Conflict_Vertice_Position[ p_last ] = p1;
                length--;
            }
            if( step_move.di == Color[i] && Adjacent_Color_Table[i][Color[i]] == 1 )
            {
                Conflict_Vertice.push_back( i );
                Conflict_Vertice_Position[ i ] = length;
                length++;
            }
        }
    }
    //如果自己也不冲突了，从冲突表中剔除
    if( Adjacent_Color_Table[step_move.v][step_move.di] == 0 )
    {
        int p = step_move.v;
        int p1 = Conflict_Vertice_Position[ p ];
        int p_last = Conflict_Vertice.back();
        Conflict_Vertice[ Conflict_Vertice_Position[p] ] = Conflict_Vertice.back();
        Conflict_Vertice.pop_back();
        Conflict_Vertice_Position[ p_last ] = p1;
        length--;
    }
    //更新冲突数
    CF =  CF + delta;
    //更新禁忌表
    Tabu_List[step_move.v][step_move.si] = CF + iter + (rand() % 10);
    //更新颜色对应表
    Color[step_move.v] = step_move.di;
}


void Log_Record()
{
    ofstream Log;
    char *log_file_name;
    if( file_name == "DSJC125.1.col" )
        log_file_name = "DSJC125.1.csv";
    if( file_name == "DSJC125.5.col" )
        log_file_name = "DSJC125.5.csv";
    if( file_name == "DSJC125.9.col" )
        log_file_name = "DSJC125.9.csv";
    if( file_name == "DSJC250.1.col" )
        log_file_name = "DSJC250.1.csv";
    if( file_name == "DSJC250.5.col" )
        log_file_name = "DSJC250.5.csv";
    if( file_name == "DSJC250.9.col" )
        log_file_name = "DSJC250.9.csv";
    if( file_name == "DSJC500.1.col" )
        log_file_name = "DSJC500.1.csv";
    if( file_name == "DSJC500.5.col" )
        log_file_name = "DSJC500.5.csv";
    if( file_name == "DSJC500.9.col" )
        log_file_name = "DSJC500.9.csv";
    if( file_name == "DSJC1000.1.col" )
        log_file_name = "DSJC1000.1.csv";
    if( file_name == "DSJC1000.5.col" )
        log_file_name = "DSJC1000.5.csv";
    if( file_name == "DSJC1000.9.col" )
        log_file_name = "DSJC1000.9.csv";

    Log.open( log_file_name, ios::app );
    if( Log.fail() )
        cout << "File:" << log_file_name << "open failed!" <<endl;

    Log << K << ",";
    Log << iter << ",";
    Log << runtime << ",";
    Log << CF_best;
    Log << "\n";
}

int main()
{
    for( int i=0; i<5; i++ )
    {
        iter =0;
        int breakiter = 0;
        //图生成
        file_name = "DSJC500.5.col";
        Graph_Generate();
        K = 49;
        //随机种子
        srand (time(0));

//        max_f = 0;
        //各种表的初始化
        ColorArray_Generate();
        Color = new int[N];
//        Color[0] = 0;
//        Color[1] = 1;
//        Color[2] = 0;
//        Color[3] = 0;
//        Color[4] = 2;
//        Color[5] = 2;
//        Color[6] = 1;
//        Color[7] = 1;
//        Color[8] = 2;

        AdjacentColor_Table_Init();

//        for( int i=0; i<N; i++ )
//            cout << Adjacent_Color_Table[i][0] << "\t" << Adjacent_Color_Table[i][1] << "\t" << Adjacent_Color_Table[i][2] << endl;

        Confilct_Vertice_Vector_Init();
        ConflictNum_Init();
        Tabu_Init();
        cout << CF_best << "\t";
        cout << endl;
        //时间记录
        clock_t start,finish;
        start = clock();

//        for( int i=0; i<length; i++ )
//            cout << Conflict_Vertice[i] << "\t";
//        cout << endl;
//        for( int i=0; i<N; i++ )
//            cout << Conflict_Vertice_Position[i] << "\t";
//        cout << endl;
//
//        Move();
//        for( int i=0; i<length; i++ )
//            cout << Conflict_Vertice[i] << "\t";
//        cout << endl;
//        for( int i=0; i<N; i++ )
//            cout << Conflict_Vertice_Position[i] << "\t";
//        cout << endl;

        //开始搜索
        while( CF_best )
        {
//            for( int i=0; i<length; i++ )
//                cout << Conflict_Vertice[i] << "\t";
//            cout << endl;
//            for( int i=0; i<N; i++ )
//                cout << Conflict_Vertice_Position[i] << "\t";
//            cout << endl;

            Move();
//            for( int i=0; i<N; i++ )
//                cout << Color[i] << "\t";
//            cout << endl;
//
//
//            for( int i=0; i<N; i++ )
//                cout << Adjacent_Color_Table[i][0] << "\t" << Adjacent_Color_Table[i][1] << "\t" << Adjacent_Color_Table[i][2] << endl;
//
//            for( int i=0; i<length; i++ )
//                cout << Conflict_Vertice[i] << "\t";
//            cout << endl;
//            for( int i=0; i<N; i++ )
//                cout << Conflict_Vertice_Position[i] << "\t";
//            cout << endl;

//            finish = clock();
//            runtime = (double)(finish-start)/CLOCKS_PER_SEC;
//            if( runtime > 600 )
//                break;
            if( CF_best < CF )
            {
                breakiter++;
                if( breakiter > max_iter)
                    break;
            }
            else
                breakiter = 0;

            if( CF_best > CF)
            {
                CF_best = CF;
                cout << CF_best << "  ";
            }
        }
        cout << endl;
        finish = clock();
        runtime = (double)(finish-start)/CLOCKS_PER_SEC;
        //写入日志
        Log_Record();
        //输出运行结果
        if( breakiter > max_iter )
        {
            cout << "cannot find solution for K=" << K << endl << "for this K, CF_best is:" << CF_best << endl;
            cout << "iter:" << iter << "\t" << "time:" << runtime << endl;
            cout << "the minimum of K is" << K+1 << endl;
        }
        else
        {
            cout << "K=" << K << " " << "\titeration:" << iter;
        }

        cout << "\ttime:" << runtime << endl;


        //cout << iter << " " << CF << endl;
        //--K;


        delete Graph;
        delete Color;
        delete Adjacent_Color_Table;
        delete Tabu_List;
    }

    return 0;
}
