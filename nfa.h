#ifndef NFA_H
#define NFA_H

#include <stdio.h>

// structure for the nfa, including its start state,
// accept state, and list of transitions
typedef struct _nfa{
	int start;
	int accept;
	struct _transition **trans;
} NFA;

// structure for the transitions, including its first
// and second state, its symbol, and the list of transitions
// that its nfa has
typedef struct _transition{
	int state1;
	int state2;
	char symbol;
	struct _transition **trans;
} Transition;

NFA* createNFA();

Transition* createTrans();

#endif