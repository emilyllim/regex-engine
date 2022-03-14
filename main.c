#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "nfa.h"

// structure for a stack entry, which will hold a
// pointer to an nfa and the next entry in the stack 
typedef struct _stackEntry{
	NFA *nfa;
	struct _stackEntry *next;
} StackEntry;

// creates a new stack entry on top that holds the nfa
void push(StackEntry **top, NFA *n){
	StackEntry* entry = malloc(sizeof(StackEntry));

	// new entry holds the nfa
	entry->nfa = n;
	// top entry is the new next entry
	entry->next = *top;
	// new entry is now on top
	*top = entry;
}

// pops the latest entry in the stack
NFA* pop(StackEntry **top){
	StackEntry *entry;
	NFA* n;

	// nfa to be popped is in the top entry
	entry = *top;
	// top is now the next entry
	*top = (*top)->next;
	// entry's nfa will be returned
	n = entry->nfa;
	free(entry);

	return n;
}

// compares the transitions of the nfa, using state1
int compareTrans(const void *a, const void *b){
    Transition** a1 = (Transition**)a;
    Transition** b1 = (Transition**)b;

    if ((*b1)->state1 < (*a1)->state1){
        return 1;
    } else if ((*b1)->state1 > (*a1)->state1){
        return -1;
    } else {
        return 0;
    }
}

// prints the transitions of the nfa, along with
// their start and accept states
void printNFA(NFA *n){
	int i = 0;
	int j = 0;
	// to store the number of transitions
	int transNum = 0;

	printf("Start: q%d\n", n->start);
	printf("Accept: q%d\n", n->accept);

	// count the number of transitions in the nfa
	while (n->trans[i]){
		transNum++;				
		i++;
	}

	// sort the transition list
	qsort(n->trans, transNum, sizeof(NFA*), compareTrans);

	// print each transition in the nfa
	while (j < transNum){
		if (n->trans[j] != NULL){
			printf("(q%d, %c) -> q%d\n", n->trans[j]->state1, n->trans[j]->symbol, n->trans[j]->state2);
			free(n->trans[j]);
		}
		j++;
	}

	free(n->trans);
	free(n);
}

// takes in an RE and builds its nfa
NFA* buildNFA(char *RE){
	int i = 0;
	int j = 0;
	int k = 0;
	// to store the number of transitions created
	int transNum = 0;
	// to store the number of states that are being created
	int stateCount = 0;
	// to store the length of RE
	int length = 0;
	// to store old start state in kleene star
	int oldStart;	
	// nfas that will be created, popped, or pushed
	NFA *nfa;
	NFA *nfa1;
	NFA *nfa2;
	// stack's top entry is currently NULL
	StackEntry *top = NULL;
	// for transitions that will be added to the nfa
	Transition *trans1;
	Transition *trans2;
	Transition *trans3;
	Transition *trans4;

	// Get the length of the RE
	while (RE[j] != '\0'){
		length++;
		j++;
	}

	// while not at end of postfix expression
	while (i < length){ 
		if (RE[i] == '&'){
			// Concatenation
			transNum = 0;
			if (top == NULL){
				printf("Error. Invalid input found.\n");
				return NULL;
			}
			nfa2 = pop(&top);
			if (top == NULL){
				printf("Error. Invalid input found.\n");
				return NULL;
			}
			nfa1 = pop(&top);

			nfa = createNFA();
			// nfa start state is the start state of nfa1
			nfa->start = nfa1->start;
			// nfa accept state is the accept state of nfa2
			nfa->accept = nfa2->accept;

			// add existing transitions of nfa1 and nfa2 to
			// nfa's transition list
			k = 0;
			while (nfa1->trans[k]){
				transNum++;				
				nfa->trans[transNum-1] = nfa1->trans[k];
				k++;
			}
			k = 0;
			while (nfa2->trans[k]){
				transNum++;
				nfa->trans[transNum-1] = nfa2->trans[k];
				k++;
			}		

			// add E transition from nfa1 accept state to
			// nfa2 start state
			trans1 = createTrans();
			trans1->state1 = nfa1->accept;
			trans1->state2 = nfa2->start;
			trans1->symbol = 'E';
			trans1->trans = nfa->trans;
			transNum++;
			nfa->trans[transNum-1] = trans1;			

			free(nfa1->trans);
			free(nfa1);
			free(nfa2->trans);
			free(nfa2);

			// push into stack an nfa that accepts concatenation
			// of L(nfa1) and L(nfa2)
			push(&top, nfa);
		} else if (RE[i] == '|'){
			// Union
			transNum = 0;
			if (top == NULL){
				printf("Error. Invalid input found.\n");
				return NULL;
			}
			nfa2 = pop(&top);
			if (top == NULL){
				printf("Error. Invalid input found.\n");
				return NULL;
			}
			nfa1 = pop(&top);

			nfa = createNFA();
			// new start state
			stateCount++;
			nfa->start = stateCount;
			// new accept state
			stateCount++;
			nfa->accept = stateCount;

			// add existing transitions of nfa1 and nfa2 to
			// nfa's transition list
			k = 0;
			while (nfa1->trans[k]){
				transNum++;				
				nfa->trans[transNum-1] = nfa1->trans[k];
				k++;
			}
			k = 0;
			while (nfa2->trans[k]){
				transNum++;
				nfa->trans[transNum-1] = nfa2->trans[k];
				k++;
			}

			// new E transition from new start state to nfa1's start state
			trans1 = createTrans();
			trans1->state1 = nfa->start;
			trans1->state2 = nfa1->start;
			trans1->symbol = 'E';
			trans1->trans = nfa->trans;
			transNum++;
			nfa->trans[transNum-1] = trans1;			

			// new E trans from new start state to nfa2's start state
			trans2 = createTrans();
			trans2->state1 = nfa->start;
			trans2->state2 = nfa2->start;
			trans2->symbol = 'E';
			trans2->trans = nfa->trans;
			transNum++;
			nfa->trans[transNum-1] = trans2;

			// new E trans from nfa1's accept state to nfa's new accept state
			trans3 = createTrans();
			trans3->state1 = nfa1->accept;
			trans3->state2 = nfa->accept;
			trans3->symbol = 'E';
			trans3->trans = nfa->trans;
			transNum++;
			nfa->trans[transNum-1] = trans3;

			// new E trans from nfa2's accept state to nfa's new accept state
			trans4 = createTrans();
			trans4->state1 = nfa2->accept;
			trans4->state2 = nfa->accept;
			trans4->symbol = 'E';
			trans4->trans = nfa->trans;
			transNum++;
			nfa->trans[transNum-1] = trans4;

			free(nfa1->trans);
			free(nfa1);
			free(nfa2->trans);
			free(nfa2);

			// push into stack an nfa that accepts union
			// of L(nfa1) and L(nfa2)			
			push(&top, nfa);	
		} else if (RE[i] == '*'){
			// Kleene Star
			transNum = 0;
			// count how many transitions already exists within the nfa
			k = 0;
			while (nfa->trans[k]){
				transNum++;				
				k++;
			}

			nfa = pop(&top);
			oldStart = nfa->start;
			// add a new start state
			stateCount++;
			nfa->start = stateCount;

			// new E transition from new start state to old start state
			trans1 = createTrans();
			trans1->state1 = nfa->start;
			trans1->state2 = oldStart;
			trans1->symbol = 'E';
			trans1->trans = nfa->trans;
			transNum++;
			nfa->trans[transNum-1] = trans1;

			// new E transition from accept state of nfa to new start state
			trans2 = createTrans();
			trans2->state1 = nfa->accept;
			trans2->state2 = nfa->start;
			trans2->symbol = 'E';
			trans2->trans = nfa->trans;
			transNum++;
			nfa->trans[transNum-1] = trans2;

			// nfa's accept state is now the new start state
			nfa->accept = nfa->start;

			// push into stack an nfa that accepts kleene star of L(nfa)
			push(&top, nfa);
		} else if ((RE[i] == 'a') || (RE[i] == 'b') || (RE[i] == 'c')
					|| (RE[i] == 'd') || (RE[i] == 'e') || (RE[i] == 'E')){
			// Any of the valid input operands
			// If operand is last character, than input is invalid
			if (i+1 == length){
				printf("Error. Invalid input found.\n");
				return NULL;
			}

			transNum = 0;

			nfa = createNFA();
			// nfa has a start state and accept state
			stateCount++;
			nfa->start = stateCount;
			stateCount++;
			nfa->accept = stateCount;

			// a transition is created, going from nfa's start state
			// to nfa's accept state. The symbol is RE[i] and the 
			// transition has the same transition list as the nfa
			trans1 = createTrans();
			trans1->state1 = nfa->start;
			trans1->state2 = nfa->accept;
			trans1->symbol = RE[i];
			trans1->trans = nfa->trans;
			// add the new transition to the nfa's transition list
			transNum++;
			nfa->trans[transNum-1] = trans1;

			// push into stack this new nfa that accepts the character RE[i]
			push(&top, nfa);
		} else {
			// Input is invalid, so print an error
			printf("Error. Invalid input found.\n");
			return NULL;
		}
		i++;
	}

	// if the top of the stack is not the last entry left,
	// print an error
	if (top->next != NULL){
		printf("Error. Invalid input found.\n");
		return NULL;		
	} else {
		// return the top of the stack
		return pop(&top);	
	}
}

int main(int argc, char const *argv[]) {
	char currentRE[1000];
	FILE *fp;
	char c;
	int i = 0;
	int j = 0;
	NFA *nfa;

	while (j < 1000){
		currentRE[j] = '\0';
		j++;
	}
	j = 0;

	// Open the file
	fp = fopen(argv[1], "r");
	if (fp){
		// get next character in the file
		c = fgetc(fp);
		// while not at the end of file
		while (c != EOF){
			//  if not at end of line
			if (c != '\n'){
				// build up the current RE
				currentRE[i] = c;
				i++;
			// if at end of line, build NFA from RE
			} else {
				printf("RE: %s\n", currentRE);
				nfa = buildNFA(currentRE);
				// if RE is valid and an nfa was built
				if (nfa){
					// print the nfa
					printNFA(nfa);
				}
				printf("\n");
				// reset the current RE
				while (j < 1000){
					currentRE[j] = '\0';
					j++;
				}
				i = 0;
				j = 0;
			}
			c = fgetc(fp);
		}
	} else {
		printf("Error. File not given or cannot be opened.\n");
		exit(1);
	}

	fclose(fp);
	return 0;
}
