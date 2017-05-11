#include "lib1617.h"

void leftRotate(NODO** root, NODO* x)
{
	NODO* T = *root;
	NODO* y = x->right; //Create new NODO and assign to x.right
	x->right = y->left; //Move left sub-tree (y) on the x sub-tree

	if (y->left != T->parent) y->left->parent = x;

	y->parent = x->parent; //Connect parent of x to y

	if (x->parent == T->parent)
		*root = y;
	else if (x == x->parent->left)
		x->parent->left = y;

	else x->parent->right = y;

	y->left = x; //Move x on the left of y
	x->parent = y;
}

void rightRotate(NODO** root, NODO* y)
{
	NODO* T = *root;
	NODO* x = y->left; //Create new NODO and assign to x.right
	y->left = x->right; //Move left sub-tree (y) on the x sub-tree

	if (x->right != T->parent) x->right->parent = y;

	x->parent = y->parent; //Connect parent of x to y

	if (y->parent == T->parent)
		*root = x;
	else if (y == y->parent->right)
		y->parent->right = x;

	else y->parent->left = x;

	x->right = y; //Move x on the left of y
	y->parent = x;
}

void insertFixUp(NODO** root, NODO** node) {
	NODO* T = *root;
	NODO* z = *node; //Puntatore temporaneo al nodo
	NODO* y = NULL;

	while (z->parent->isBlack == false)	{
		if (z->parent == z->parent->parent->left) {
			y = z->parent->parent->left;
			if (y->isBlack == false) {
				z->parent->isBlack = true;
				y->isBlack = true;
				z->parent->parent->isBlack = false;
				z = z->parent->parent;
			} else {
				if (z == z->parent->right) {
					z = z->parent;
					leftRotate(root, z);
				}
				z->parent->isBlack = true;
				z->parent->parent->isBlack = false;
				rightRotate(root, z->parent->parent);
			}
		} else {
			y = z->parent->parent->left;
			if (y->isBlack == false) {
				z->parent->isBlack = true;
				y->isBlack = true;
				z->parent->parent->isBlack = false;
				z = z->parent->parent;
			} else {
				if (z == z->parent->left) {
					z = z->parent;
					rightRotate(root, z);
				}
				z->parent->isBlack = true;
				z->parent->parent->isBlack = false;
				leftRotate(root, z->parent->parent);
			}
		}
	}

	T->isBlack = true;
}

void insertRBT(NODO** root, NODO* node) {
	NODO* y = NULL;
	NODO* x = NULL;
	NODO* T = *root;
	//========= To Finsh =========//
}

NODO * createFromFile(char * nameFile)
{
	return NULL;
}

void printDictionary(NODO * dictionary)
{
}

int countWord(NODO * dictionary)
{
	return 0;
}

int insertWord(NODO ** dictionary, char * word)
{
	return 0;
}

int cancWord(NODO ** dictionary, char * word)
{
	return 0;
}

char * getWordAt(NODO * dictionary, int index)
{
	return NULL;
}

int insertDef(NODO * dictionary, char * word, char * def)
{
	return 0;
}

char * searchDef(NODO * dictionary, char * word)
{
	return NULL;
}

int saveDictionary(NODO * dictionary, char * fileOutput)
{
	return 0;
}

NODO * importDictionary(char * fileInput)
{
	return NULL;
}

int searchAdvance(NODO * dictionary, char * word, char ** primoRis, char ** secondoRis, char ** terzoRis)
{
	return 0;
}

int compressHuffman(NODO * dictionary, char * fileOutput)
{
	return 0;
}

int decompressHuffman(char * fileInput, NODO ** dictionary)
{
	return 0;
}
