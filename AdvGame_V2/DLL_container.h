#pragma once

#include "entities.h"

// OBJECT-STORING DOUBLY LINKED LIST

typedef struct ObjNode {
	Object* o;
	struct ObjNode* prev;
	struct ObjNode* next;
} ObjNode;

typedef struct OBJ_DLL {
	ObjNode* head, * tail;
} OBJ_DLL;

// ENEMY-STORING DOUBLY LINKED LIST

typedef struct FoeNode {
	Foe* f;
	struct FoeNode* prev;
	struct FoeNode* next;
} FoeNode;

typedef struct FOE_DLL {
	FoeNode* head, * tail;
} FOE_DLL;
