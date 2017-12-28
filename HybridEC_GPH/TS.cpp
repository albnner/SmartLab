#include "TS.h"

using namespace std;

extern int N,K,E;
extern int **Graph;

int CF_T,CF_T_best;
int **Adjacent_Color_Table;
int **Tabu_List;
int iter_t=0,  max_iter_t = 40000;
int *Color;

int *Best_Solution_T;

Neighborstep::Neighborstep(int v1, int si1, int di1)
{
    v = v1;
    si = si1;
    di = di1;
}

Neighborstep::Neighborstep()
{
    v=0; si=0; di=0;
}

void Inde_To_Arr( vector <int> *S )
{
    for( int i=0; i<K; i++ )
        for( unsigned int j=0; j<S[i].size(); j++ )
            Color[S[i][j]] = i;
}

void Arr_To_Inde( vector <int> *S )
{
    for( int i=0; i<K; i++ )
        S[i].clear();
    for( int j=0; j<N; j++ )
        S[Best_Solution_T[j]].push_back(j);
}

void Color_Array_Generate()
{
    Color = new int [N];
    Best_Solution_T = new int [N];
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
    {
        for(int j=0; j<K; j++)
        {
            Adjacent_Color_Table[i][j] = 0;
        }
    }
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
}

void ConflictNum_Init()
{
    CF_T=0;
    int i;
    for(i=0; i<N; i++)
    {
        CF_T = CF_T + Adjacent_Color_Table[i][Color[i]];
    }
    CF_T = CF_T / 2;
    CF_T_best = CF_T;
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
    Neighborstep step_best;
    int i=0,delta_Tabu,delta_NoTabu,delta_Tabu_min=K,delta_NoTabu_min=K;
    for(i=0; i<N; i++)
    {
        if( Adjacent_Color_Table[i][Color[i]] != 0 )   //冲突数不为0则需动作
        {
            int j=0;
            while(j<K)
            {
                if( j==Color[i] )
                {
                    j++;
                    continue;
                }
                if( Tabu_List[i][j] > iter_t )              //是否为禁忌的动作
                {
                    delta_Tabu = Adjacent_Color_Table[i][j] - Adjacent_Color_Table[i][Color[i]];
                    if( delta_Tabu < delta_Tabu_min )           //存储禁忌动作的最优值
                    {
                        delta_Tabu_min = delta_Tabu;
                        step_best_t.clear();
                        step_best_t.push_back(Neighborstep(i, Color[i], j));
                    }
                    else if( delta_Tabu == delta_Tabu_min )
                    {
                        step_best_t.push_back(Neighborstep(i, Color[i], j));
                    }
                }
                else
                {
                    delta_NoTabu = Adjacent_Color_Table[i][j] - Adjacent_Color_Table[i][Color[i]];
                    if( delta_NoTabu < delta_NoTabu_min )        //存储非禁忌动作的最优值
                    {
                        delta_NoTabu_min = delta_NoTabu;
                        step_best_nt.clear();
                        step_best_nt.push_back(Neighborstep(i, Color[i], j));
                    }
                    else if( delta_NoTabu == delta_NoTabu_min )
                    {
                        step_best_nt.push_back(Neighborstep(i, Color[i], j));
                    }
                }
                j++;
            }
        }
    }
    iter_t++;
    if( delta_Tabu_min + CF_T < CF_T_best && delta_Tabu_min < delta_NoTabu_min )       //释放准则：禁忌动作是历史最优解
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
    int i;
    //更新Adjacent_Color_Table
    for( i=0; i<N; i++)
    {
        if( Graph[step_move.v][i] )
        {
            Adjacent_Color_Table[i][step_move.si]--;
            Adjacent_Color_Table[i][step_move.di]++;
        }
    }
    //更新冲突数
    CF_T =  CF_T + delta;
    //更新禁忌表
    Tabu_List[step_move.v][step_move.si] = CF_T + iter_t + (rand() % 10);
    //更新颜色对应表
    Color[step_move.v] = step_move.di;
}

int Conflict_Calculate(vector <int> *S)
{
    int CF_T_2 = 0;
    Color_Array_Generate();
    Inde_To_Arr(S);
    AdjacentColor_Table_Init();
    for( int i=0; i<N; i++ )
    {
        CF_T_2 = CF_T_2 + Adjacent_Color_Table[i][Color[i]];
    }
    CF_T_2 = CF_T_2 / 2;
    delete Color;
    delete Adjacent_Color_Table;
    return CF_T_2;
}

int Tabu_Search(vector <int> *S)
{
    //禁忌搜索初始化准备
    iter_t = 0;
    Color_Array_Generate();
    /*for(int i=0; i<N; i++)
    {
        //srand (time(0));
        Color[i] = rand() % K;
        //cout << Color[i] << "\t";
    }*/
    Inde_To_Arr(S);
    AdjacentColor_Table_Init();
    ConflictNum_Init();
    //cout << CF_T_best << endl;
    Tabu_Init();
    //cout << "init ok" << endl;
    //cout << CF_T << "\t" << CF_T_best << endl;

    //开始搜索
    int breakiter = 0;
    while( CF_T_best )
    {
        Move();
        if( CF_T_best > CF_T)
        {
            CF_T_best = CF_T;
            for( int i=0; i<N; i++ )
                Best_Solution_T[i] = Color[i];
            //cout << CF_T_best << " ";
        }
        if( iter_t > max_iter_t )
            break;

        /*if( CF_T_best < CF_T )
        {
            breakiter ++;
            if( breakiter > max_iter_t )
                break;
        }
        else
            breakiter = 0;*/
    }
    Arr_To_Inde(S);
    //cout << CF_T_best << "\t";
    //cout << CF_T << "\t" << CF_T_best << "\t";
    //cout << iter_t << endl;
    delete Color;
    delete Adjacent_Color_Table;
    delete Tabu_List;
    delete Best_Solution_T;

    return CF_T_best;
    //cout << "4" << "\t" << Color[Solution[4].front()] << endl;
    //cout << "7" << "\t" << Color[Solution[7].front()] << endl;
}
