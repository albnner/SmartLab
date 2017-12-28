#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include "TS.h"
#include <time.h>
#include <ctype.h>

using namespace std;

char *file_name;
int N,K,E;
int **Graph;
int CF_best = 0;
int *CF;
int gen_conf;

vector <int> ** Population;
int P_num;                       //种群大小

vector <int> * Best_Solution;

vector <int> * parent1;
vector <int> * parent2;
vector <int> * gen;

int iter = 0, max_iter = 2000000, breakiter = 0;

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
};

Adjacent_List **Ad_list;

void Graph_Generate()
{
    int i,v1,v2;
    fstream F;
    F.open(file_name);
    char blank1[50];
    F >> blank1;
    while( !F.eof() )
    {
        if( strcmp( blank1, "p" ) == 0)
        {
            F >> blank1 >> N >> E;
            cout << "Number of vertexes:" << N << endl;
            cout << "Number of edges:" << E << endl;
            Ad_list = new Adjacent_List *[N];
            for( i=0; i<N; i++)
            {
                Ad_list[i] = new Adjacent_List;
            }
        }
        F >> blank1;
    }

    Adjacent_List *a,*b;
    while( !F.eof() )
    {
        if( strcmp( blank1, "e" ) == 0 )
        {
            F >> v1 >> v2;
            v1--;
            v2--;
            a = Ad_list[v1];
            while( a->next )
                a = a->next;
            b = new Adjacent_List;
            b->v_value = v2;
            b->next = NULL;
            a->next = b;

            a = Ad_list[v2];
            while( a->next )
                a = a->next;
            b = new Adjacent_List;
            b->v_value = v1;
            b->next = NULL;
            a->next = b;
        }
        F >> blank1;
    }
}*/

void Population_Init()
{
    //生成解集合
    Population = new vector<int>* [P_num];
    for( int i=0; i<P_num; i++ )
        Population[i] = new vector<int> [K];
    //随机初始化解集合
    for( int i=0; i<P_num; i++ )
        for( int j=0; j<N; j++ )
        {
            Population[i][rand() % K].push_back(j);
        }
}

void Best_Find()
{
    vector <int> Best_Label;
    //CF = new int [P_num];
    /*for( int i=0; i<P_num; i++ )
    {
        CF[i] = Conflict_Calculate( Population[i] );
        //cout << CF[i] << "\t";
    }*/
    //cout << endl;
    //CF[0] = 2;
    //CF[1] = 4;
    for( int j=0; j<P_num; j++ )
    {
        if( CF[j] < CF_best )
        {
            CF_best = CF[j];
            Best_Label.clear();
            Best_Label.push_back(j);
        }
        else if( CF[j] == CF_best )
        {
            Best_Label.push_back(j);
        }
    }
    Best_Solution = Population[ Best_Label[ rand() % Best_Label.size() ] ];
}

void Crossover( vector<int>*p1, vector<int>*p2, vector<int>*gen )
{

    int i,j,t;
    unsigned int k,p;
    int break_sig = 0;
    unsigned int max_size = 0, max_size_label = 0;

    //copy p1 to parent1
    for( i=0; i<K; i++ )
        parent1[i].assign( p1[i].begin(), p1[i].end() );
    //copy p2 to parent2
    for( i=0; i<K; i++ )
        parent2[i].assign( p2[i].begin(), p2[i].end() );

    //crossover
    for( i=0; i<K; i++ )
    {
        if( i % 2 == 0 )
        {
            //find a max cardinality
            max_size = 0;
            for( j=0; j<K; j++ )
            {
                if( parent1[j].size() > max_size )
                {
                     max_size = parent1[j].size();
                     max_size_label = j;
                }
            }
            //transmit to next generation
            for( p=0; p<max_size; p++ )
                gen[i].push_back( parent1[max_size_label].at(p) );
            //remove from p1 and p2
            parent1[max_size_label].clear();
            for( p=0; p<max_size; p++ )
            {
                for( t=0; t<K; t++ )
                {
                    break_sig = 0;
                    for( k=0; k<parent2[t].size(); k++ )
                        {
                            if( parent2[t].at(k) == gen[i].at(p) )
                            {
                                parent2[t].erase( parent2[t].begin()+k );
                                break_sig = 1;
                                break;
                            }
                        }
                    if( break_sig )
                        break;
                }
            }
        }
        else
        {
            //find a max cardinality
            max_size = 0;
            for( j=0; j<K; j++ )
            {
                if( parent2[j].size() > max_size )
                {
                     max_size = parent2[j].size();
                     max_size_label = j;
                }
            }
            //transmit to next generation
            for( p=0; p<max_size; p++ )
                gen[i].push_back( parent2[max_size_label].at(p) );
            //remove from p1 and p2
            parent2[max_size_label].clear();
            for( p=0; p<max_size; p++ )
            {
                for( t=0; t<K; t++ )
                {
                    break_sig = 0;
                    for( k=0; k<parent1[t].size(); k++ )
                        {
                            if( parent1[t].at(k) == gen[i].at(p) )
                            {
                                parent1[t].erase( parent1[t].begin()+k );
                                break_sig = 1;
                                break;
                            }
                        }
                    if( break_sig )
                        break;
                }
            }
        }
    }
    //assign randomly the rest of vertex
    for( i=0; i<K; i++ )
    {
        if( parent1[i].size() )
        {
            for( p=0; p<parent1[i].size(); p++ )
            {
                //srand (time(NULL));
                gen[ rand() % K ].push_back( parent1[i].at(p) );
            }
            parent1[i].clear();
        }
        if( parent2[i].size() )
            parent2[i].clear();
    }
}

void Pool_Update( vector<int>*g )
{
    int CF_worst = 0;
    vector <int> Worst_Label;
    //find worst solution
    for( int j=0; j<P_num; j++ )
    {
        if( CF[j] > CF_worst )
        {
            CF_worst = CF[j];
            Worst_Label.clear();
            Worst_Label.push_back(j);
        }
        else if( CF[j] == CF_worst )
        {
            Worst_Label.push_back(j);
        }
    }
    //update
    int k = rand() % Worst_Label.size();
    if( gen_conf <= CF_worst )
    {
        CF[ Worst_Label.at(k) ] = gen_conf;
        breakiter++;
        for( int i=0; i<K; i++ )
        {
            //swap( Population[ Worst_Label.at(k) ][i], g[i] );
            swap( Population[ Worst_Label.at(k) ][i], g[i] );
        }
    }
    else
        breakiter = 0;
    for( int i=0; i<K; i++ )
        g[i].clear();
}

int main( int argc,char*argv[] )
{
    file_name = "DSJC500.5.col";
    P_num = 3;
    K = 48;
    srand (time(0));

    clock_t start,finish,start_t,finish_t;
    double runtime,runtime_t;

    //vector <int> * p1;
    //vector <int> * p2;
    Graph_Generate();

    CF = new int [P_num];

    CF_best = E;
    Population_Init();
    double t_temp;
    clock_t t_s, t_f;
    for( int i=0; i<P_num; i++ )
    {
        t_s = clock();
        CF[i] = Tabu_Search(Population[i]);
        t_f = clock();
        t_temp = (double)(t_f-t_s)/CLOCKS_PER_SEC;
        cout << "solution" << i << " local search finish!" << "\t" ;
        cout << t_temp << "s" <<endl;
    }

    Best_Find();
    for( int i=0; i<P_num; i++ )
    {
        cout << CF[i] << "\t";
    }
    cout << endl;

    gen = new vector <int> [K];
    parent1 = new vector <int> [K];
    parent2 = new vector <int> [K];

    start = clock();

    while( CF_best )
    {
        int k1,k2;
        k1 = rand() % P_num;
        k2 = rand() % P_num;
        while( k1 == k2 )
            k2 = rand() % P_num;
        Crossover(Population[k1], Population[k2], gen);

        start_t = clock();

        gen_conf = Tabu_Search(gen);
        cout << "tabu_finish" << "\t";
        //gen_conf = Conflict_Calculate(gen);
        cout << "gen_conf:" << gen_conf << "\t";
        finish_t = clock();

        runtime_t = (double)(finish_t - start_t)/CLOCKS_PER_SEC;
        cout << "tabu_time:" << runtime_t << endl;

        Pool_Update(gen);
        Best_Find();

        for( int i=0; i<P_num; i++ )
        {
            cout << CF[i] << "\t";
        }
        cout << endl;

        /*if( gen_conf < CF_best )
        {
            Best_Solution = gen;
            CF_best = gen_conf;
            Pool_Update(gen);
            Best_Find();
        }
        else if( gen_conf == CF_best )
        {
            Pool_Update(gen);
            Best_Find();
        }
        else
        {

        }Pool_Update(gen);*/

        for( int i=0; i<K; i++ )
            gen[i].clear();

        iter ++;
        if( breakiter > max_iter )
            break;
    }

    finish = clock();
    runtime = (double)(finish - start)/CLOCKS_PER_SEC;
    cout << "iter:" << iter << "\t" << "runtime" << runtime;
    /*vector <int> *S;
    S = new vector<int> [K];
    for( int i=0; i<N; i++ )
    {
        int n = rand() % K;
        S[n].push_back(i);
    }
    for(int i=0; i<K; i++)
    {
        cout << S[i].front() << "\t";
    }*/
    //int cf_tem = Conflict_Calculate(S);
    //cout << cf_tem << "\t";
    //cf_tem = Conflict_Calculate(S);
    //cout << cf_tem << endl;
    return 0;
}

/*int main()
{
    P_num = 2;
    K = 3;
    file_name = "Test_Data.txt";
    srand (time(0));

    Graph_Generate();
    CF_best = E;
    Population = new vector<int>* [P_num];
    for( int i=0; i<P_num; i++ )
        Population[i] = new vector<int> [K];

    //vector <int> *p1 = new vector<int> [K];
    //vector <int> *p2 = new vector<int> [K];
    vector <int> *gen = new vector<int> [K];
    Population_Init();


    /*Population[0][0].push_back(2);
    Population[0][0].push_back(4);
    Population[0][1].push_back(0);
    Population[0][1].push_back(6);
    Population[0][1].push_back(8);
    Population[0][1].push_back(5);
    Population[0][2].push_back(1);
    Population[0][2].push_back(3);
    Population[0][2].push_back(7);

    Population[1][0].push_back(0);
    Population[1][0].push_back(2);
    Population[1][0].push_back(3);
    Population[1][1].push_back(1);
    Population[1][1].push_back(6);
    Population[1][1].push_back(7);
    Population[1][2].push_back(4);
    Population[1][2].push_back(5);
    Population[1][2].push_back(8);

    int i,j;
    unsigned int k;

    for( j=0; j<K; j++ )
    {
        for( k=0; k<Population[0][j].size(); k++ )
            cout << Population[0][j].at(k);
        cout << "\t";
    }
    cout << endl;

    for( j=0; j<K; j++ )
    {
        for( k=0; k<Population[1][j].size(); k++ )
            cout << Population[1][j].at(k);
        cout << "\t";
    }
    cout << endl;

    Tabu_Search(Population[0]);

    Best_Find();

    parent1 = new vector <int> [K];
    parent2 = new vector <int> [K];

    Crossover(Population[0], Population[1], gen);


    for( j=0; j<K; j++ )
    {
        for( k=0; k<Population[0][j].size(); k++ )
            cout << Population[0][j].at(k);
        cout << "\t";
    }
    cout << endl;

    for( j=0; j<K; j++ )
    {
        for( k=0; k<Population[1][j].size(); k++ )
            cout << Population[1][j].at(k);
        cout << "\t";
    }
    cout << endl;

    for( j=0; j<K; j++ )
    {
        for( k=0; k<gen[j].size(); k++ )
            cout << gen[j].at(k);
        cout << "\t";
    }
    cout << endl;

    Pool_Update(gen);
    for( j=0; j<K; j++ )
    {
        for( k=0; k<Population[0][j].size(); k++ )
            cout << Population[0][j].at(k);
        cout << "\t";
    }
    cout << endl;

    for( j=0; j<K; j++ )
    {
        for( k=0; k<Population[1][j].size(); k++ )
            cout << Population[1][j].at(k);
        cout << "\t";
    }
    cout << endl;


    return 0;

}
*/
