
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(disable : 4996)

typedef struct {

    char name[20];
    int points;

} candidate;


typedef struct {

    int ballot_index;
    char candidate1_name[20];
    char candidate2_name[20];
    char candidate3_name[20];

} ballot;

void select_candidates(int amount, char* candidate_names[], candidate* candidates);

void vote(ballot* selection_ballot, int amount_of_votes, candidate* candidates, int amount_of_candidates);

void select_winner(candidate* candidates, int amount_of_candidates, candidate* elimination_list, int amount_of_votes, ballot* ballots);

int ballot_validity_check(ballot* current_ballot, candidate* candidates, int amount_of_candidates);

void ballot_vote(ballot* current_ballot, candidate* candidates, candidate* elimination_list, int amount_of_candidates, int amount_of_votes);

void eliminate_candidates(candidate* eliminated_candidate, candidate* elimination_list, int elimination_index);

int candidate_eliminated(candidate* check_candidate, candidate* elimination_list, int loop_count);

int which_ballot_vote(ballot* current_ballot, candidate* candidates, candidate* elimination_list, int amount_of_candidates);

void reset_all_points(int amount_of_candidates, candidate* candidates);

int main(int argc, char* argv[])
{
    int amount_of_candidates = argc - 1;
    int votes;
    printf("Amount of contestants: %d\n", argc - 1);
    printf("entry is: %s\n", argv[2]);

    // Get all the candidates
    candidate* candidates = (candidate*)malloc(amount_of_candidates * sizeof(candidate));
    select_candidates(amount_of_candidates, argv, candidates);

    // Make list of Eliminated candidates
    candidate* eliminated_candidates = (candidate*)malloc(amount_of_candidates * sizeof(candidate));


    // check all candidates
    for (int i = 0; i < amount_of_candidates; i++) {
        printf("candidate is: %s\n", candidates[i].name);
    }

    // Vote
    printf("Enter amount of votes: ");
    scanf("%d", &votes);

    // define ballots
    ballot* ballots = (ballot*)malloc(votes * sizeof(ballot));

    // start voting
    vote(ballots, votes, candidates, amount_of_candidates);

    // decide the winner
    select_winner(candidates, amount_of_candidates, eliminated_candidates, votes, ballots);

    free(candidates);
    free(ballots);
    free(eliminated_candidates);

    return 0;
}

void select_candidates(int amount, char* candidate_names[], candidate* candidates) {
    for (int i = 0; i < amount; i++) {
        strncpy(candidates[i].name, candidate_names[i + 1], sizeof(candidates[i].name) - 1);
        candidates[i].name[sizeof(candidates[i].name) - 1] = '\0'; // Null-terminate the string
        candidates[i].points = 0;
    }
}

int ballot_validity_check(ballot* current_ballot, candidate* candidates, int amount_of_candidates) {
    // if any match, then return 1, else return 0
    int valid_candidate = 0;
    if (strcmp(current_ballot->candidate1_name, current_ballot->candidate2_name)==0) {
        return 0;
    }
    if (strcmp(current_ballot->candidate2_name, current_ballot->candidate3_name)==0) {
        return 0;
    }
    if (strcmp(current_ballot->candidate1_name, current_ballot->candidate3_name)==0) {
        return 0;
    }

    for (int i = 0; i < amount_of_candidates; i++) {
        if (strcmp(current_ballot->candidate1_name, candidates[i].name)==0) {
            // give them a vote
            candidates[i].points += 1;
            valid_candidate++;
        }
        if (strcmp(current_ballot->candidate2_name, candidates[i].name) == 0) {
            // don't give vote
            valid_candidate++;
        }
        if (strcmp(current_ballot->candidate3_name, candidates[i].name) == 0) {
            // don't give vote
            valid_candidate++;
        }
    }

    if (valid_candidate == 3) {
        return 1;
    }
    else {
        return 0;
    }

}

void vote(ballot* selection_ballot, int amount_of_votes, candidate* candidates, int amount_of_candidates) {
    for (int i = 0; i < amount_of_votes; i++) {
        printf("Enter selection 1 name: ");
        scanf("%s", selection_ballot[i].candidate1_name);
        printf("Enter selection 2 name: ");
        scanf("%s", selection_ballot[i].candidate2_name);
        printf("Enter selection 3 name: ");
        scanf("%s", selection_ballot[i].candidate3_name);
        selection_ballot[i].ballot_index = i;

        if (ballot_validity_check(&selection_ballot[i], candidates, amount_of_candidates) == 0) {
            printf("Invalid names on ballot, please re-enter them\n");
            i--;
        }

    }

}

void select_winner(candidate* candidates, int amount_of_candidates, candidate* elimination_list, int amount_of_votes, ballot* ballots) {

    int we_got_winner = 1;
    int el_index = 0;
    int threshold = amount_of_votes / 2;
    if (amount_of_votes % 2 == 1) {
        threshold++;
    }

    while (we_got_winner == 1) {
        //check for winner
        int tie = 0;
        for (int i = 0; i < amount_of_candidates; i++) {
            if (candidates[i].points > 0) {
                tie++;
            }
            if (candidates[i].points >= threshold) {
            printf("Winner is: %s with %d points\n", candidates[i].name, candidates[i].points);
            we_got_winner = 0;
            break;
            }
        }
        if (tie == 0) {
            printf("tie!\n");
            break;
        }

        int points = INT_MAX;

        for (int i = 0; i < amount_of_candidates; i++) {
        printf("%d st/nd/th is: %s with %d points\n", i+1, candidates[i].name, candidates[i].points);
        }
        // find the last spot
        for (int i = 0; i < amount_of_candidates; i++) {
            if (candidate_eliminated(&candidates[i], elimination_list, el_index) == 1) {
                continue;
            }
            if (candidates[i].points < points ) {
                points = candidates[i].points;
            }
        }
        // eliminate the last spot
        for (int i = 0; i < amount_of_candidates; i++) {
            if (candidates[i].points == points) {
                eliminate_candidates(&candidates[i], elimination_list, el_index);
                el_index++;
            }
        }

        // recount ballots where eliminated users are excluded.
        ballot_vote(ballots, candidates, elimination_list, amount_of_candidates, amount_of_votes);

        // reset all votes
        reset_all_points(amount_of_candidates, candidates);
    }

}

void eliminate_candidates(candidate* eliminated_candidate, candidate* elimination_list, int elimination_index) {
    // Eliminated candidate gets allocated to elimination list at defined index.
    elimination_list[elimination_index] = *eliminated_candidate;
}

int candidate_eliminated(candidate* check_candidate, candidate* elimination_list, int loop_count) {
    for (int i = 0; i < loop_count; i++) {
        if (strcmp(check_candidate->name, elimination_list[i].name) == 0) {
            return 1;
        } 
    }
    return 0;
}

void ballot_vote(ballot* current_ballot, candidate* candidates, candidate* elimination_list, int amount_of_candidates, int amount_of_votes) {

    for (int i = 0; i < amount_of_votes; i++) {
        if (which_ballot_vote(&current_ballot[i], candidates, elimination_list, amount_of_candidates) == 1) {
            for (int j = 0; j < amount_of_candidates; j++) {
                if (strcmp(current_ballot[i].candidate1_name, candidates[j].name) == 0) {
                    // give them a vote
                    candidates[j].points += 1;
                }
            }
        }
        if (which_ballot_vote(&current_ballot[i], candidates, elimination_list, amount_of_candidates) == 2) {
            for (int j = 0; j < amount_of_candidates; j++) {
                if (strcmp(current_ballot[i].candidate2_name, candidates[j].name) == 0) {
                    // give them a vote
                    candidates[j].points += 1;
                }
            }
        }
        if (which_ballot_vote(&current_ballot[i], candidates, elimination_list, amount_of_candidates) == 3) {
            for (int j = 0; j < amount_of_candidates; j++) {
                if (strcmp(current_ballot[i].candidate3_name, candidates[j].name) == 0) {
                    // give them a vote
                    candidates[j].points += 1;
                }
            }
        }
        if (which_ballot_vote(&current_ballot[i], candidates, elimination_list, amount_of_candidates) == 4) {
            continue;
        }
    }
}

int which_ballot_vote(ballot* current_ballot, candidate* candidates, candidate* elimination_list, int amount_of_candidates) {
    // check each candidate
    int first_voted = 0;
    int second_voted = 0;
    int third_voted = 0;

    for (int i = 0; i < amount_of_candidates; i++) {
        // check first candidate
        if (strcmp(current_ballot->candidate1_name, candidates[i].name) == 0) {
            if (candidate_eliminated(&candidates[i], elimination_list, amount_of_candidates) == 1) {
                first_voted = 1;
            }
        }
        // check second candidate
        if (strcmp(current_ballot->candidate2_name, candidates[i].name) == 0) {
            if (candidate_eliminated(&candidates[i], elimination_list, amount_of_candidates) == 1) {
                second_voted = 1;
            }
        }
        // check third candidate
        if (strcmp(current_ballot->candidate3_name, candidates[i].name) == 0) {
            if (candidate_eliminated(&candidates[i], elimination_list, amount_of_candidates) == 1) {
                third_voted = 1;
            }
        }
    }

    if (first_voted == 0) {
        return 1;
    }
    else {
        if (second_voted == 0) {
            return 2;
        }
        else {
            if (third_voted == 0) {
                return 3;
            }
            else {
                return 4;
            }
        }
    }
}

void reset_all_points(int amount_of_candidates, candidate* candidates) {
    for (int i = 0; i < amount_of_candidates; i++) {
        candidates[i].points = 0;
    }
}
