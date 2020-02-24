/*
dispatcher.c

Student Name : Ali Bokhari
Student ID # : 0970498

Dispatch Algorithm : SRT
*/

#include <string.h>
#include <stddef.h>
#include <stdio.h>

#define MAX_LINE_LENGTH 100

#include "LinkedListAPI.h"

int hrd;

struct Process {
  int start_time;
  int run_time;
  int process_id;
  int num_exchanges;
  List *exchanges;
  int t_runtime;
  int t_readytime;
  int t_blockedtime;
} typedef Process;

struct cpu {
  int time;
  Process *p;
} typedef cpu;

struct hdd {
  int time;
  Process *p;
} typedef hdd;

char *dummyprint(void *toBePrinted) {
  return NULL;
}

void dummydelete(void *toBeDeleted) {
  return;
}

int processCmp(const void *c1, const void *c2) {
  Process *p1 = (Process*)c1;
  Process *p2 = (Process*)c2;

  if (p1->process_id == p2->process_id) {
    return 0;
  }

  if(p2->process_id == 0) {
    return -1;
  }

  if ((p1->run_time + (p1->num_exchanges * hrd)) < (p2->run_time + (p2->num_exchanges * hrd))) {
    return -1;
  }
  return 1;
}

int intCmp(const void *c1, const void *c2) {
  int *p1 = (int*)c1;
  int *p2 = (int*)c2;

  if (*p1 == *p2) {
    return 0;
  }

  if(*p1 < *p2) {
    return -1;
  }
  return 1;
}

void incrementR(List *list) {
  if (list == NULL) {
    return;
  }
  void *elem2;
  ListIterator iter2 = createIterator(list);
  while((elem2 = nextElement(&iter2)) != NULL) {
    Process* tmpName2 = (Process*)elem2;
    (tmpName2->t_runtime)++;
  }
}

void incrementRdy(List *list) {
  if (list == NULL) {
    return;
  }
  void *elem2;
  ListIterator iter2 = createIterator(list);
  while((elem2 = nextElement(&iter2)) != NULL) {
    Process* tmpName2 = (Process*)elem2;
    (tmpName2->t_readytime)++;
  }
}

void incrementB(List *list) {
  if (list == NULL) {
    return;
  }
  void *elem2;
  ListIterator iter2 = createIterator(list);
  while((elem2 = nextElement(&iter2)) != NULL) {
    Process* tmpName2 = (Process*)elem2;
    (tmpName2->t_blockedtime)++;
  }
}

void dispatcher(FILE *fd, int harddrive){
    /*
        Your code here.
        You may edit the following code
    */
    hrd = harddrive;
    char line_buffer[MAX_LINE_LENGTH];
    int start_time, run_time, process_id, num_exchanges, exchange_time;
    char *token;
    List *new_queue, *ready_queue, *blocked_queue, *finished;
    new_queue = initializeList(dummyprint, dummydelete, processCmp);
    ready_queue = initializeList(dummyprint, dummydelete, processCmp);
    blocked_queue = initializeList(dummyprint, dummydelete, processCmp);
    finished = initializeList(dummyprint, dummydelete, processCmp);
    //Process simulation input line by line
    while (fgets(line_buffer, MAX_LINE_LENGTH, fd) != NULL && line_buffer[0] != '\n'){
      Process *newp = malloc(sizeof(Process));
      newp->exchanges = initializeList(dummyprint, dummydelete, intCmp);
      newp->t_runtime = 0;
      newp->t_readytime = 0;
      newp->t_blockedtime = 0;

      token = strtok(line_buffer, " ");
      sscanf(token, "%d",&start_time);
      newp->start_time = start_time;

      token = strtok(NULL, " ");
      sscanf(token, "%d",&process_id);
      newp->process_id = process_id;

      token = strtok(NULL, " ");
      sscanf(token, "%d",&run_time);
      newp->run_time = run_time;

      num_exchanges = 0;
      token = strtok(NULL, " ");
      while ( token != NULL ){
          num_exchanges += sscanf(token, "%d",&exchange_time);
          insertBack(newp->exchanges, &exchange_time);
          token = strtok(NULL, " ");
      }
      newp->num_exchanges = num_exchanges;
     //printf("Process %3d wants to start at %6dms and run for %6dms and has %3d hard drive exchanges\n",  process_id, start_time, run_time, num_exchanges);
     insertBack(new_queue, newp);
  }

  // void *elem, *elem2;
  //
	// ListIterator iter = createIterator(new_queue);
	// while ((elem = nextElement(&iter)) != NULL){
	// 	Process* tmpName = (Process*)elem;
	// 	printf("%d %d %d %d", tmpName->start_time, tmpName->process_id, tmpName->run_time, tmpName->num_exchanges);
  //     ListIterator iter2 = createIterator(tmpName->exchanges);
  //     while((elem2 = nextElement(&iter2)) != NULL) {
  //       int* tmpName2 = (int*)elem2;
  //       printf(" %d", *tmpName2);
  //     }
  //     printf("\n");
	// }
  void *elem;
  ListIterator iter;

  int newsize = getLength(new_queue);

  Process *p0 = malloc(sizeof(Process));
  p0->process_id = 0;
  p0->t_runtime = 0;

  cpu *pcpu = malloc(sizeof(cpu));
  hdd *phdd = malloc(sizeof(hdd));
  pcpu->p = p0;
  pcpu->time = 0;
  Process *tmpp;
  int *tmpi;
  for (int time = 0; getLength(finished) != newsize; time++) {
    if(pcpu->p->process_id != 0) {
      if (pcpu->time - pcpu->p->run_time == 0) {
        pcpu->p->t_runtime += pcpu->time;
        //printf("%d finished\n", pcpu->p->process_id);
        insertBack(finished, pcpu->p);
        if ((tmpp = getFromFront(ready_queue)) != NULL) {
          pcpu->p = tmpp;
          pcpu->time = 0;
          deleteDataFromList(ready_queue, tmpp);
        } else {
          pcpu->p = p0;
          pcpu->time = 0;
        }
      } else if ((tmpi = getFromFront(pcpu->p->exchanges)) != NULL && (pcpu->p->t_runtime)+(pcpu->time) == *tmpi) {
        pcpu->p->t_runtime += pcpu->time;
        pcpu->p->run_time -= pcpu->time;
        //printf("correct\n");
        deleteDataFromList(pcpu->p->exchanges, tmpi);
        (pcpu->p->num_exchanges)--;
        insertBack(blocked_queue, pcpu->p);
        if ((tmpp = getFromFront(ready_queue)) != NULL) {
          pcpu->p = tmpp;
          pcpu->time = 0;
          deleteDataFromList(ready_queue, tmpp);
        } else {
          pcpu->p = p0;
          pcpu->time = 0;
        }
      }
    }

    if (phdd->p != NULL && phdd->time == harddrive) {
      phdd->p->t_blockedtime += harddrive;
      pcpu->p->t_runtime += pcpu->time;
      pcpu->p->run_time -= pcpu->time;
      pcpu->time = 0;
      if (pcpu->p->process_id == 0) {
        pcpu->p = phdd->p;
      } else if (processCmp(phdd->p, pcpu->p) <= 0) {
        insertSorted(ready_queue, pcpu->p);
        pcpu->p = phdd->p;
      } else {
        insertSorted(ready_queue, phdd->p);
      }
      phdd->p = NULL;
    }

    if ((tmpp = getFromFront(new_queue)) != NULL) {
      if (tmpp->start_time == time) {
        if (pcpu->p->process_id == 0) {
          pcpu->p->t_runtime += pcpu->time;
          pcpu->p->run_time -= pcpu->time;
          pcpu->p = tmpp;
          pcpu->time = 0;
        } else if (processCmp(tmpp, pcpu->p) <= 0) {
          pcpu->p->t_runtime += pcpu->time;
          pcpu->p->run_time -= pcpu->time;
          insertSorted(ready_queue, pcpu->p);
          pcpu->p = tmpp;
          pcpu ->time = 0;
        } else {
          //printf("inserted %d\n", tmpp->process_id);
          insertSorted(ready_queue, tmpp);
        }
        deleteDataFromList(new_queue, tmpp);
      }
    }
    if (phdd->p == NULL) {
      if ((tmpp = getFromFront(blocked_queue)) != NULL) {
        phdd->p = tmpp;
        phdd->time = 0;
        deleteDataFromList(blocked_queue, tmpp);
      }
    }

    pcpu->time ++;
    phdd->time ++;
    incrementRdy(ready_queue);
    incrementB(blocked_queue);
    //printf("%d %d %d\n", pcpu->p->process_id, pcpu->p->t_runtime, time);
  }

  printf("%d %d\n", p0->process_id, p0->t_runtime);
  iter = createIterator(finished);
	while ((elem = nextElement(&iter)) != NULL){
		Process* tmpName = (Process*)elem;
		printf("%d %d %d %d\n", tmpName->process_id, tmpName->t_runtime, tmpName->t_readytime, tmpName->t_blockedtime);
	}
}
