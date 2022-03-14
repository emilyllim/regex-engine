#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "nfa.h"

// function to create an nfa, malloc space for it
// and its transition list, and set each index to NULL
NFA* createNFA(){
	int i = 0;

	NFA *nfa = malloc(sizeof(NFA));
	nfa->start = 0; // start state number
	nfa->accept = 0; // accept state number
	nfa->trans = malloc(sizeof(Transition) * 1000);
	// set each index in the transition array to NULL
	while(i < 1000){
		nfa->trans[i] = NULL;
		i++;
	}

	return nfa;
}

// function to create a transition, malloc space for it,
// and set all of it's values to 0
Transition* createTrans(){
	Transition *trans = malloc(sizeof(Transition));
	trans->state1 = 0;
	trans->state2 = 0;
	trans->symbol = 0;
	trans->trans = 0;

	return trans;
}