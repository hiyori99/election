// コンドルセの投票方法による選挙選出の実装

#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;
int u; //=pairs[i].winner

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    int i;
    // 投票者のi番目選考の候補者を格納
    for (i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i], name) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    int i, j;
    // ranks配列を元にpreferencesに得票数を格納
    for (i = 0; i < candidate_count; i++)
    {
        for (j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    for (i = 0; i < candidate_count; i++)
    {
        for (j = i + 1; j < candidate_count; j++)
        {
            // preferences[ranks[i]][ranks[j]]++;
            printf("%d", preferences[ranks[i]][ranks[j]]);
        }
        printf("\n");
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    int i, j;
    pair_count = 0;
    // preferencesを元に構造体pairsにペアを格納
    for (i = 0; i < candidate_count; i++)
    {
        for (j = 0; j < candidate_count; j++)
        {
            //引き分けの候補のペアは配列に追加しない
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i; //candidates[i]
                pairs[pair_count].loser = j;  //candidates[j]
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    int i, j, count, str[pair_count];
    int win, lose;
    count = 0;
    // pairsのwinnerの得票数を元に降順に並べ替える
    // まずはpreferencesを1次元配列にする
    for (i = 0; i < pair_count; i++)
    {
        for (j = 0; j < pair_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                str[count] = preferences[i][j];
                count++;
            }
        }
    }
    // 作った1次元配列を元に構造体を降順ソート
    for (i = 0; i < pair_count; i++)
    {
        for (j = i + 1; j < pair_count; j++)
        {
            if (str[i] > str[j])
            {
                win =  pairs[i].winner;
                pairs[i].winner = pairs[j].winner;
                pairs[j].winner = win;
                lose =  pairs[i].loser;
                pairs[i].loser = pairs[j].loser;
                pairs[j].loser = lose;
            }
        }
    }
    for (i = 0; i < pair_count; i++)
    {
        printf("%d %d\n", pairs[i].winner, pairs[i].loser);
    }

    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    int i, j, v;

    //ロックされたグラフlockedを作成
    //矢印がサイクルを作成しない
    //隣接行列でループを持てば終了
    //順番にロックしていく
    for (i = 0; i < pair_count; i++)
    {
        u = pairs[i].winner;
        v = pairs[i].loser;
        // u -> v に辺をはる
        // v からスタートして、u にたどり着く

        bool exist_loop = false;
        // current_node := 現在の頂点
        int current_node = v;
        while (true)
        {
            // u にたどり着いたら、ループ発見
            if (current_node == u)
            {
                exist_loop = true;
                break;
            }

            // locked[i][j] := i -> j に辺がある
            // 辺を探す
            bool find = false;
            for (int to = 0; to < pair_count; to++)
            {
                if (locked[current_node][to])
                {
                    // current_node -> to に辺が存在したら
                    current_node = to;
                    find = true;
                    break;
                }
            }

            // 何も見つからなければ、break;
            if (!find)
            {
                break;
            }
        }

        // 辺をはる
        if (!exist_loop)
        {
            locked[u][v] = true;
            // printf("%d %d\n", u, v);
        }
    }

    return;
}

// Print the winner of the election
void print_winner(void)
{
    // グラフの開始点である候補者の名前を出力する
    printf("%s\n", candidates[u]);
    return;
}

