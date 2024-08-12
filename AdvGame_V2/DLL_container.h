#pragma once

#include "entities.h"

// OBJECT-STORING DOUBLY LINKED LIST

PWRHNGR_STRUCT ObjNode {
	Object* o;
	struct ObjNode* prev;
	struct ObjNode* next;
} ObjNode;

PWRHNGR_STRUCT OBJ_DLL {
	ObjNode* head, * tail;
} OBJ_DLL;

// ENEMY-STORING DOUBLY LINKED LIST

PWRHNGR_STRUCT FoeNode {
	Foe* f;
	struct FoeNode* prev;
	struct FoeNode* next;
} FoeNode;

PWRHNGR_STRUCT FOE_DLL {
	FoeNode* head, * tail;
} FOE_DLL;
