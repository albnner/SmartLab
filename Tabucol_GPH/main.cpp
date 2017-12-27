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
FILE *fp;
int N,E,K,CF,CF_best;
int **Graph;
int *Color;
int **Adjacent_Color_Table;
int **Tabu_List;
int iter=0, max_iter=3000000;

int iter_temp = 0;
double ** f_repeat;
double max_f = 0;

double runtime;


class Neighborstep{
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
        v=0; si=0; di=0;
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

/*class Adjacent_List{
    public:
        int v_value;
        Adjacent_List *next;

        Adjacent_List()
        {
            v_value = 0;
            next = NULL;
        }
};*/
/*typedef struct Adjacent_List{
         int v_value ;
         struct Adjacent_List *next ;
         }Adjacent ;
Adjacent * *Ad_list ;

//Adjacent_List **Ad_list;

void Graph_Generate()
{
    int i,v1,v2;
    Adjacent *a, *b;
    ifstream F;
    F.open(file_name);
    char blank1[100],blank2[50];
    F >> blank1;
    while( !F.eof() )
    {
        if( strcmp( blank1, "p" ) == 0)
        {
            F >> blank1 >> N >> E;
            cout << "Number of vertexes:" << N << endl;
            cout << "Number of edges:" << E << endl;
            Ad_list = new Adjacent *[N];
            for( i=0; i<N; i++)
            {
                Ad_list[i] = new Adjacent;
                Ad_list[ i ]->v_value = 0 ;
                Ad_list[ i ]->next = NULL ;
            }

            for(i=0; i<E; i++)
            {
                F >> blank2;
                F >> v1 >> v2;
                v1--;v2--;
                a = Ad_list[v1];
                while( a->next != NULL )
                    a = a->next;
                b = new Adjacent;
                b->v_value = v2;
                b->next = NULL;
                a->next = b;
                //cout << (a->next)->v_value;

                a = Ad_list[v2];
                while( a->next != NULL )
                    a = a->next;
                b = new Adjacent;
                b->v_value = v1;
                b->next = NULL;
                a->next = b;
            }
        }
        F >> blank1;
    }
    F.close();

    /*F.open(file_name);
    F >> blank2;

    Adjacent_List *a,*b;
    while( !F.eof() )
    {
        if( strcmp( blank2, "e" ) == 0 )
        {
            F >> v1 >> v2;
            v1--;
            v2--;
            a = Ad_list[v1];
            while( a->next != NULL )
                a = a->next;
            b = new Adjacent_List;
            b->v_value = v2;
            b->next = NULL;
            a->next = b;
            //cout << (a->next)->v_value;

            a = Ad_list[v2];
            while( a->next != NULL )
                a = a->next;
            b = new Adjacent_List;
            b->v_value = v1;
            b->next = NULL;
            a->next = b;
        }
        F >> blank1;
    }
    F.close();
}*/

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
    for( int i=0; i<N; i++ )
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
                if( Tabu_List[i][j] > iter)              //是否为禁忌的动作
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
    iter++;
    if( delta_Tabu_min + CF < CF_best && delta_Tabu_min < delta_NoTabu_min )       //释放准则：禁忌动作是历史最优解
    {
        //srand (time(0));
        int k = (rand() % step_best_t.size());
        step_best = step_best_t[k];
        Update(step_best, delta_Tabu_min);
    }
    else                                              //按非禁忌最优动作进行动作
    {
        int k = (rand() % step_best_nt.size());
        //cout << k << "\t";
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
            //f_repeat[i][step_move.si]++;
            //max_f = ( max_f < f_repeat[i][step_move.si] ) ? f_repeat[i][step_move.si] : max_f;
            //f_repeat[i][step_move.di] /= iter;
        }
    }
    //更新频率数
    //f_repeat[step_move.v][step_move.di]++;
    //max_f = ( max_f < f_repeat[step_move.v][step_move.di] ) ? f_repeat[step_move.v][step_move.di] : max_f;
    //更新冲突数
    CF =  CF + delta;
    //更新禁忌表
    Tabu_List[step_move.v][step_move.si] = CF + iter + (rand() % 15);
    //更新颜色对应表
    Color[step_move.v] = step_move.di;

    //根据重复频率动态调整禁忌步长

    /*iter_temp ++;
    if( iter_temp > 100000  )
    {
        iter_temp = 0;
        for( int i=0; i<N; i++ )
            for( int j=0; j<K; j++ )
                f_repeat[i][j] = 0;
    }*/
    //double max_f = 0;
    /*for( int i=0; i<N; i++ )
        for( int j=0; j<K; j++ )
            max_f = ( max_f < f_repeat[i][j] ) ? f_repeat[i][j] : max_f;*/
    //if( f_repeat[step_move.v][step_move.di] > iter/10 )
    //Tabu_List[step_move.v][step_move.di] += ( f_repeat[step_move.v][step_move.di] / max_f ) * 15;
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
    for( int i=0; i<500; i++ )
    {
        iter =0;
        file_name = "DSJC500.1.col";
        Graph_Generate();
        K = 12;

        f_repeat = new double *[N];
        for( int i=0; i<N; i++ )
        {
            f_repeat[i] = new double [K];
        }

        srand (time(0));
        while(K)
        {
            int breakiter = 0;
            max_f = 0;
            clock_t start,finish;
            //srand (time(0));
            ColorArray_Generate();
            AdjacentColor_Table_Init();
            ConflictNum_Init();
            Tabu_Init();
            cout << CF_best << "\t";
            start = clock();
            while( CF_best )
            {
                Move();
                if( CF_best > CF)
                {
                    cout << CF_best << "  ";
                    CF_best = CF;
                }
                if( CF_best < CF )
                {
                    breakiter++;
                    if( breakiter > max_iter)
                        break;
                }
                else
                    breakiter = 0;
            }
            cout << endl;

            finish = clock();
            runtime = (double)(finish-start)/CLOCKS_PER_SEC;

            Log_Record();

            if( breakiter > max_iter )
            {
                cout << "cannot find solution for K=" << K << endl << "for this K, CF_best is:" << CF_best << endl;
                cout << "iter:" << iter << "\t" << "time:" << runtime << endl;
                cout << "the minimum of K is" << K+1 << endl;
                break;
            }
            else
            {
                cout << "K=" << K << " " << "\titeration:" << iter;
            }

            cout << "\ttime:" << runtime << endl;


            //cout << iter << " " << CF << endl;
            //--K;
            K = 0;
        }
    }
    //cout << Graph[2][0] << endl;
    /*cout << Adjacent_Color_Table[0][0] << " " << Adjacent_Color_Table[0][1] << " " << Adjacent_Color_Table[0][2] << endl;
    cout << Adjacent_Color_Table[1][0] << " " << Adjacent_Color_Table[1][1] << " " << Adjacent_Color_Table[1][2] << endl;
    cout << Adjacent_Color_Table[2][0] << " " << Adjacent_Color_Table[2][1] << " " << Adjacent_Color_Table[2][2] << endl;
    cout << Adjacent_Color_Table[3][0] << " " << Adjacent_Color_Table[3][1] << " " << Adjacent_Color_Table[3][2] << endl;
    cout << Adjacent_Color_Table[4][0] << " " << Adjacent_Color_Table[4][1] << " " << Adjacent_Color_Table[4][2] << endl;
    cout << Adjacent_Color_Table[5][0] << " " << Adjacent_Color_Table[5][1] << " " << Adjacent_Color_Table[5][2] << endl;
    cout << Adjacent_Color_Table[6][0] << " " << Adjacent_Color_Table[6][1] << " " << Adjacent_Color_Table[6][2] << endl;
    cout << Adjacent_Color_Table[7][0] << " " << Adjacent_Color_Table[7][1] << " " << Adjacent_Color_Table[7][2] << endl;
    cout << Adjacent_Color_Table[8][0] << " " << Adjacent_Color_Table[8][1] << " " << Adjacent_Color_Table[8][2] << endl;
*/
    //cout << CF << endl;
    //cout << iter << endl;
    //cout << Tabu_List[0][0] << endl;
    //cout << Tabu_Move.v << Tabu_Move.si << Tabu_Move.di << endl;
    //cout << NoTabu_Move.v << NoTabu_Move.si << NoTabu_Move.di << endl;
    return 0;
}
