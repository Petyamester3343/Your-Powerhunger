#include "DLL_container.h"

#include <string.h>

#define PWRHNGRDEFINE static inline

// Checks if a certain position is already occupied
PWRHNGRDEFINE bool isPosOccupied(unsigned int col, unsigned int row, Player* p, OBJ_DLL* ol, FOE_DLL* fl) {
	if (col == p->E.pos.col && row == p->E.pos.row && p->E.pos.occupied)
		return true;

	ObjNode* currObj = ol->head;
	while (currObj != NULL) {
		if (currObj->o->pos.col == col && currObj->o->pos.row == row && currObj->o->pos.occupied)
			return true;
		currObj = currObj->next;
	}

	FoeNode* currFoe = fl->head;
	while (currFoe != NULL) {
		if (currFoe->f->E.pos.col == col && currFoe->f->E.pos.row == row && currFoe->f->E.pos.occupied)
			return true;
		currFoe = currFoe->next;
	}

	return false;
}

// A random position generator
PWRHNGRDEFINE Pos generateRandomPos(Player* p, OBJ_DLL* ol, FOE_DLL* fl) {
	Pos pos = { 0, 0 };

	do {
		pos.col = (unsigned int)rand() % 15;
		pos.row = (unsigned int)rand() % 15;
	} while ((pos.col == 14 && pos.row == 14) && isPosOccupied(pos.col, pos.row, p, ol, fl) && (pos.col == p->E.pos.col && pos.row == p->E.pos.row));

	pos.occupied = true;
	return pos;
}

// Constructor
PWRHNGRDEFINE OBJ_DLL* createObjList() {
	OBJ_DLL* list = (OBJ_DLL*)malloc(sizeof(OBJ_DLL));
	if (list != NULL) {
		list->head = NULL;
		list->tail = NULL;
	}
	return list;
}

// Finds an object by a given position
PWRHNGRDEFINE ObjNode* findObjByPos(OBJ_DLL* list, Pos p) {
	ObjNode* curr = list->head;
	while (curr != NULL) {
		if (curr->o->pos.col == p.col && curr->o->pos.row == p.row) {
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}

// Inserts an instance of object into the list
PWRHNGRDEFINE bool insertObjIntoList(OBJ_DLL* olist, FOE_DLL* flist, char sign, unsigned int eff, Player* p) {
	Object* o = (Object*)malloc(sizeof(Object));
	if (o == NULL)
        return false;

	o->sign = sign;
	o->eff = eff;
	o->pos = generateRandomPos(p, olist, flist);
	o->pos.occupied = true;

	ObjNode* neu = (ObjNode*)malloc(sizeof(ObjNode));
	if (neu == NULL) {
        free(o);
        return false;
    }

    neu->o = o;
	neu->prev = NULL;
	neu->next = NULL;

	if (olist->head == NULL) {
		olist->head = neu;
		olist->tail = neu;
	}
	else {
		olist->tail->next = neu;
		neu->prev = olist->tail;
		olist->tail = neu;
	}

	return true;
}

// Empties the object list
PWRHNGRDEFINE void emptyObjList(OBJ_DLL* list) {
	ObjNode* curr = list->head;
	ObjNode* next;

	while (curr != NULL) {
		next = curr->next;
		free(curr);
		curr = next;
	}
}

// Destructor
PWRHNGRDEFINE void destroyObjList(OBJ_DLL* list) {
	emptyObjList(list);
	free(list);
}

// Removes an instance of Object from the list
PWRHNGRDEFINE void removeObjNode(OBJ_DLL* list, ObjNode* node) {
    node->o->pos.occupied = false;
    if (node == list->head && node == list->tail) {
        // Case: Only one node in the list
        list->head = NULL;
        list->tail = NULL;
    }
    else if (node == list->head) {
        // Case: Deleting the head node
        list->head = node->next;
        list->head->prev = NULL;
    }
    else if (node == list->tail) {
        // Case: Deleting the tail node
        list->tail = node->prev;
        list->tail->next = NULL;
    }
    else {
        // Case: Deleting a node in between
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    // Free the memory allocated to the node
    free(node);
}


// Constructor
PWRHNGRDEFINE FOE_DLL* createFoeList() {
	FOE_DLL* list = (FOE_DLL*)malloc(sizeof(FOE_DLL));
	if (list != NULL) {
		list->head = NULL;
		list->tail = NULL;
	}
	return list;
}

// Creates an instance of Foe inside the list
PWRHNGRDEFINE FoeNode* createFoe(Foe* f) {
	FoeNode* neu = (FoeNode*)malloc(sizeof(FoeNode));
	if (neu == NULL)
        return NULL;

	neu->f = f;
	neu->prev = NULL;
	neu->next = NULL;
    return neu;
}

// Finds a foe by its name
PWRHNGRDEFINE FoeNode* findFoeByName(FOE_DLL* list, const char* name) {
	FoeNode* curr = list->head;
	while (curr != NULL) {
		if (strcmp(curr->f->E.name, name) == 0) {
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}

// Inserts a Foe into the list
PWRHNGRDEFINE bool insertFoeIntoList
(Player* p, OBJ_DLL* ol, FOE_DLL* fl,
 const char* name,
 unsigned int hp, const unsigned int atk, unsigned int def,
 const unsigned int xp, const unsigned int loot) {
	Foe* f = (Foe*)malloc(sizeof(Foe));
	if (f == NULL)
        return false;

	f->E.name = strdup(name);
	f->E.hp = hp;
	f->E.atk = atk;
	f->E.def = def;
	f->E.xp = xp;
	f->E.pos = generateRandomPos(p, ol, fl);
	f->E.dead = false;
	f->E.fled = false;
	f->loot = loot;


	FoeNode* neu = createFoe(f);

	if(neu == NULL) {
        free(f);
        return false;
	}

	if (fl->tail == NULL) {
		fl->head = neu;
		fl->tail = neu;
	}
	else {
		fl->tail->next = neu;
		neu->prev = fl->tail;
		fl->tail = neu;
	}

	return true;
}

// Removes an instance of Foe from the list
PWRHNGRDEFINE void removeFoeNode(FOE_DLL* list, FoeNode* node) {
	node->f->E.pos.occupied = false;

	if (node->prev != NULL) {
		node->prev->next = node->next;
	}
	else {
		list->head = node->next;
	}

	if (node->next != NULL) {
		node->next->prev = node->prev;
	}
	else {
		list->tail = node->prev;
	}

	free(node->f);
	free(node);
}

// Finds a foe by a given position
PWRHNGRDEFINE FoeNode* findFoeByPos(FOE_DLL* list, Pos p) {
	FoeNode* curr = list->head;
	while (curr != NULL) {
		if (curr->f->E.pos.col == p.col && curr->f->E.pos.row == p.row) {
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}

// Finds a foe by its state of living
PWRHNGRDEFINE FoeNode* findFoeByStatus(FOE_DLL* list) {
	FoeNode* curr = list->head;
	while (curr != NULL) {
		if (curr->f->E.dead) {
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}

// Removes a foe if it is dead
PWRHNGRDEFINE void removeFoeByStatus(FOE_DLL* list) {
	FoeNode* node = findFoeByStatus(list);
	if (node != NULL) {
		removeFoeNode(list, node);
	}
}

// Gets the number of foes inside the foes' doubly linked list
PWRHNGRDEFINE const unsigned int getFoeCount(FOE_DLL* list) {
	unsigned int size = 0;
	FoeNode* curr = list->head;
	while (curr != NULL) {
		size++;
		curr = curr->next;
	}
	return size;
}

// Gets the number of specific(!) foes inside the foes' doubly linked list
PWRHNGRDEFINE const unsigned int getFoeCountByName(FOE_DLL* list, const char* name) {
    unsigned int count = 0;
    FoeNode* curr = list->head;
    while(curr != NULL) {
        if(strcmp(curr->f->E.name, name) == 0) {
            count++;
        }
        curr = curr->next;
    }
    return count;
}

// Empties the doubly linked list
PWRHNGRDEFINE void emptyFoeList(FOE_DLL* list) {
	FoeNode* curr = list->head;
	FoeNode* next;

	while (curr != NULL) {
		next = curr->next;
		free(curr);
		curr = next;
	}
}

// Destructor
PWRHNGRDEFINE void destroyFoeList(FOE_DLL* list) {
	emptyFoeList(list);
	free(list);
}
