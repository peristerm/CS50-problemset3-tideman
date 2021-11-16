#include <cs50.h>
#include <stdio.h>
#include <string.h>

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
 
    for (int i = 0; i < candidate_count; i++)

    {
        //if the name was defined in candidates update the ranks
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            rank++;
           
            return true;
        }

    }

    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i; j < candidate_count; j++)
        {
            //save ranks into preferences
            if (i < j)
            {
                preferences[ranks[i]][ranks[j]]++;

            }

        }

    }


    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)

{

    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            //check if one prefered over the other
            if (preferences[i][j] > preferences[j][i])
            { 
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;

            }
        }
    }
    return;
}




// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    int x = 0;
    
    
    while (x < pair_count)
    {
        //finding the pair that has the largest number of votes
        int index_of_largest = x;
        
        
        for (int i = x; i < pair_count; i++)
        {
          
            if (preferences[pairs[i].winner][pairs[i].loser] > preferences[pairs[index_of_largest].winner][pairs[index_of_largest].loser])
             
            {   
                index_of_largest = i;
            }
        }
        
        //swapping
        
        int tempwin = pairs[x].winner;
        int templos = pairs[x].loser;
        pairs[x].winner = pairs[index_of_largest].winner;
        pairs[x].loser = pairs[index_of_largest].loser; 
        pairs[index_of_largest].winner = tempwin;
        pairs[index_of_largest].loser = templos;
        
        x++;
        
        
    }

    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    int locked_count = 1;
    bool check = false;
    
    //lock 0th pair right away
    locked[pairs[0].winner][pairs[0].loser] = true;
    
    for (int i = 1; i < pair_count; i++)
    {
        check = false;
         
        //check if the pair creates a cycle  
        for (int j = 0; j < locked_count; j++)
        {
            //check if a loser wins against another candidate among locked pairs
            if (pairs[i].loser == pairs[j].winner && locked[pairs[j].winner][pairs[j].loser] == true)
            
            {
                for (int k = 0; k < locked_count; k++)
                {   
                    
                    if (pairs[j].loser == pairs[k].winner && pairs[k].loser == pairs[i].winner && locked[pairs[k].winner][pairs[k].loser] == true)
                    {
                        locked[pairs[i].winner][pairs[i].loser] = false;
                        check = true;
                    }
                        
                    else if (pairs[j].loser == pairs[k].winner && pairs[k].loser != pairs[i].winner && locked[pairs[k].winner][pairs[k].loser] == true)
                    {
                        for (int l = 0; l < locked_count; l++)
                        {
                            //if there is a cycle unlock the pair
                            if (pairs[k].loser == pairs[l].winner && pairs[l].loser == pairs[i].winner)
                            {
                                locked[pairs[i].winner][pairs[i].loser] = false;
                                check = true;
                            }
                                
                        }
                            
                    }
                }
                    
            }
        }
           
        //if there is no cycle lock the pair   
        if (check == false)
        {
            
            locked[pairs[i].winner][pairs[i].loser] = true;
            locked_count++;
        }
    }
   
    return;
}

// Print the winner of the election
void print_winner(void)
{
    int winnercount = 0;
    bool check = false;
    int k = -1;
    
    typedef struct
    {
        string name;
    }
    winner;
    winner person[candidate_count];
    
    //checking if the candidate wins against others 
    for (int i = 0; i < candidate_count; i++) 
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[i][j] == true && locked[j][i] == false)
            {
                winnercount++;
                check = true;
            }
        }
        //save the absolute winner to the struct
        if (winnercount == candidate_count - 1)
        {
            k++;
            person[k].name = candidates[i];
        }
        
        //if there is no absolute winner continue to save the tied winners
        else if (check == true)
        {
            k++;
            person[k].name = candidates[i];
        }
        
        winnercount = 0;
    }
    
    //print all winners
    for (int i = 0; i <= k; i++)
    {
        printf("%s\n", person[i].name);
    }
    
    return;
}

