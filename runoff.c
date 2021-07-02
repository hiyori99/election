#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max voters and candidates
#define MAX_VOTERS 100
#define MAX_CANDIDATES 9

// preferences[i][j] is jth preference for voter i
int preferences[MAX_VOTERS][MAX_CANDIDATES];

// Candidates have name, vote count, eliminated status
typedef struct
{
    string name;
    int votes;
    bool eliminated;
}
candidate;

// Array of candidates
candidate candidates[MAX_CANDIDATES];

// Numbers of voters and candidates
int voter_count;
int candidate_count;

// Function prototypes
bool vote(int voter, int rank, string name);
void tabulate(void);
bool print_winner(void);
int find_min(void);
bool is_tie(int min);
void eliminate(int min);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: runoff [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX_CANDIDATES)
    {
        printf("Maximum number of candidates is %i\n", MAX_CANDIDATES);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i].name = argv[i + 1];
        candidates[i].votes = 0;
        candidates[i].eliminated = false;
    }

    voter_count = get_int("Number of voters: ");
    if (voter_count > MAX_VOTERS)
    {
        printf("Maximum number of voters is %i\n", MAX_VOTERS);
        return 3;
    }

    // Keep querying for votes
    for (int i = 0; i < voter_count; i++)
    {

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            // Record vote, unless it's invalid
            if (!vote(i, j, name))
            {
                printf("Invalid vote.\n");
                return 4;
            }
        }

        printf("\n");
    }

    // Keep holding runoffs until winner exists
    while (true)
    {
        // Calculate votes given remaining candidates
        tabulate();

        // Check if election has been won
        bool won = print_winner();
        if (won)
        {
            break;
        }

        // Eliminate last-place candidates
        int min = find_min();
        bool tie = is_tie(min);

        // If tie, everyone wins
        if (tie)
        {
            for (int i = 0; i < candidate_count; i++)
            {
                if (!candidates[i].eliminated)
                {
                    printf("%s\n", candidates[i].name);
                }
            }
            break;
        }

        // Eliminate anyone with minimum number of votes
        eliminate(min);

        // Reset vote counts back to zero
        for (int i = 0; i < candidate_count; i++)
        {
            candidates[i].votes = 0;
        }
    }
    return 0;
}

// Record preference if vote is valid
bool vote(int voter, int rank, string name)
{
    int i;
    // name が有効な候補者の名前と一致する場合
    // グローバル配列preferencesを更新
    for (i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i].name, name) == 0)
        {
            preferences[voter][rank] = i;
            return true;
        }
    }
    return false;
}

// Tabulate votes for non-eliminated candidates
void tabulate(void)
{
    int i, j;
    // 決選投票の現段階での得票数votesを更新する
    // preferencesを元に得票数を格納する
    printf("%d\n", voter_count);
    for (i = 0; i < voter_count; i++)
    {
        //候補者が削除されているとき
        if (candidates[preferences[i][0]].eliminated == true)
        {
            if (candidates[preferences[i][1]].eliminated == false)
            {
                candidates[preferences[i][1]].votes++;
            }
            //Rank2の候補者も削除されている時
            else if (candidates[preferences[i][1]].eliminated == true)
            {
                candidates[preferences[i][2]].votes++;
            }
        }
        //候補者が生き残っているとき
        if (candidates[preferences[i][0]].eliminated == false)
        {
            candidates[preferences[i][0]].votes++;
        }
    }
    return;
}

// Print the winner of the election, if there is one
bool print_winner(void)
{
    int i;
    // 過半数の票を持っている候補者がいればprint
    for (i = 0;  i < candidate_count; i++)
    {
        if (candidates[i].votes > voter_count / 2)
        {
            printf("%s\n", candidates[i].name);
            return true;
        }
    }
    return false;
}

// Return the minimum number of votes any remaining candidate has
int find_min(void)
{
    int i, min;
    // まだ選挙に残っている候補者の最小投票総数を返す
    min = 100;
    for (i = 0; i < candidate_count; i++)
    {
        if ((candidates[i].eliminated == false) && (min > candidates[i].votes))
        {
            min = candidates[i].votes;
        }
    }
    return min;
}

// Return true if the election is tied between all candidates, false otherwise
bool is_tie(int min)
{
    int i, j, vote, count;
    vote = 0;
    count = 0;
    // 選挙に残っているすべての候補者の投票数が同じ場合はtrueを返す
    for (i = 0; i < voter_count; i++)
    {
        if (candidates[i].eliminated == false)
        {
            vote = candidates[i].votes; //有効な投票数を入れる
            break;
        }
    }
    for (i = 0; i < candidate_count; i++)
    {
        if ((candidates[i].eliminated == false) && (candidates[i].votes != vote))
        {
            //voteと投票数が違う候補者がいればfalse
            return false;
            break;
        }
    }
    return true;
}

// Eliminate the candidate (or candidates) in last place
void eliminate(int min)
{
    int i;
    // 得票数minの候補者を除外する
    for (i = 0; i < voter_count; i++)
    {
        if (candidates[i].votes == min)
        {
            candidates[i].eliminated = true;
        }
    }
    return;
}
