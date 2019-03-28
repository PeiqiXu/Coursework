#include "election.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
//include any other headers you need here...
//test for ':' and '\0': in case no input before ':' or '\0'
void testinput(const char * line, int i, char t) {
  if (line[i] == t) {
    printf("Invalid input!\n");
    exit(EXIT_FAILURE);
  }
  return;
}

state_t parseLine(const char * line) {
  state_t stateline;
  int i = 0;
  testinput(line, i, ':');
  //test if the statename has no valid inputs
  for (int a = 0; line[a] != ':'; a++) {
    if (line[a] == '\n' || line[a] == '\0') {
      printf("Invalid Input For Format 1!\n");
      exit(EXIT_FAILURE);
    }
  }
  //put line element into stateline and end with '\0'
  for (; line[i] != ':'; i++) {
    stateline.name[i] = line[i];
  }
  stateline.name[i] = '\0';
  i++;
  testinput(line, i, ':');
  //test to make sure the number is valid
  int j = i;
  for (; line[j] != ':'; j++) {
    if (!isdigit(line[j])) {
      printf("Invalid Input Format 2!\n");
      exit(EXIT_FAILURE);
    }
  }
  //put number element into population and end with '\0'
  char number[j - i + 1];
  int k = 0;
  for (; line[i] != ':'; i++) {
    number[k] = line[i];
    k++;
  }
  number[k] = '\0';
  stateline.population = atoi(number);
  i++;
  j = i;
  testinput(line, i, '\0');
  //test to make sure the number is valid
  for (; line[j] != '\0'; j++) {
    if (!isdigit(line[j])) {
      printf("Invalid Input Format 3!\n");
      exit(EXIT_FAILURE);
    }
  }
  //put number element into electoralvotes and end with '\0'
  char number_2[j - i + 1];
  k = 0;
  for (; line[i] != '\0'; i++) {
    number_2[k] = line[i];
    k++;
  }
  number_2[k] = '\0';
  stateline.electoralVotes = atoi(number_2);
  //test if the number of electoralvotes and population are logical
  if (stateline.electoralVotes > stateline.population) {
    printf("Wrong Inputs!\n");
    exit(EXIT_FAILURE);
  }
  return stateline;
}

//STEP 1: write me

unsigned int countElectoralVotes(state_t * stateData, uint64_t * voteCounts, size_t nStates) {
  unsigned int count = 0;  //the total votes
  for (size_t i = 0; i < nStates; i++) {
    if (((1.0 * (voteCounts[i])) / (stateData[i].population)) > 0.5) {
      if (voteCounts[i] > stateData[i].population) {
        printf("Wrong votes!\n");
        exit(EXIT_FAILURE);
      }
      count += stateData[i].electoralVotes;
    }
  }

  //STEP 2: write me
  return count;
}

void printRecounts(state_t * stateData, uint64_t * voteCounts, size_t nStates) {
  for (size_t i = 0; i < nStates; i++) {
    if ((voteCounts[i] >= 0.4950 * stateData[i].population) &&
        (voteCounts[i] <= 0.5050 * stateData[i].population)) {
      printf("%s requires a recount (Candidate A has %.2f%% of the vote)\n",
             stateData[i].name,
             100.00 * voteCounts[i] / stateData[i].population);
    }
  }
  return;

  //STEP 3: write me
}

void printLargestWin(state_t * stateData, uint64_t * voteCounts, size_t nStates) {
  double max = 100.00 * voteCounts[0] / stateData[0].population;
  size_t k = 0;
  for (size_t i = 0; i < nStates; i++) {
    if ((100.00 * voteCounts[i] / (stateData[i].population)) > max) {
      max = 100.00 * voteCounts[i] / stateData[i].population;
    }
  }
  // test if exists more than one has the same max
  for (size_t j = 0; j < nStates; j++) {
    if ((100.00 * voteCounts[j] / (stateData[j].population)) == max) {
      k = j;
      printf("Candidate A won %s with %.2f%% of the vote\n", stateData[k].name, max);
    }
  }
  return;

  //STEP 4: write me
}
