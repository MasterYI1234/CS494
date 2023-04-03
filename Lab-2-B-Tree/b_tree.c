#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include "jdisk.h"
#include "b_tree.h"

#define newTreeNode (Tree_Node*)malloc(sizeof(Tree_Node))
#define intSize sizeof(int)
#define uintSize sizeof(unsigned int)
#define charSize sizeof(char)
#define ucharSize sizeof(unsigned char)
#define ulongSize sizeof(unsigned long)
#define is_leaf(node) (((int)(node->internal) == 0)?1:0)
#define c2i(c) ((int)(c))

//b tree node
typedef struct tnode {
	unsigned char nkeys; /* Number of keys in the node */
	unsigned char flush; /* Should I flush this to disk at the end of b_tree_insert()? */
	unsigned char internal; /* Internal or external node */
	unsigned int lba; /* LBA when the node is flushed */
	unsigned char **keys; /* Pointers to the keys.  Size = MAXKEY+1 */
	unsigned int *lbas; /* Pointer to the array of LBA's.  Size = MAXKEY+2 */
	struct tnode *parent; /* Pointer to my parent -- useful for splitting */
	int parent_index; /* My index in my parent */
	struct tnode **children;
} Tree_Node;

//b tree

typedef struct {
	int key_size; /* These are the first 16/12 bytes in sector 0 */
	unsigned int root_lba;
	unsigned long first_free_block;
	void *disk; /* The jdisk */
	unsigned long size; /* The jdisk's size */
	unsigned long num_lbas; /* size/JDISK_SECTOR_SIZE */
	int MAXKEY;/* MAXKEY */
	int MAXVALS;/* MAXKEY+1 */
	int keys_per_block;
	int lbas_per_block;
	struct tnode *root;

	int flush; /* Should I flush sector[0] to disk after b_tree_insert() */
} B_Tree;
//write 0 block into jdisk
void write_sector_0(B_Tree *b_tree) {
	void *buf;
	buf = (void *) malloc(sizeof(char) * JDISK_SECTOR_SIZE);
	memcpy(buf, (void *) b_tree,
	intSize + uintSize + ulongSize);
	memcpy(buf + intSize + uintSize + ulongSize, (void *) &b_tree->size,
	ulongSize);
	jdisk_write(b_tree->disk, 0, buf);
	free(buf);
	return;
}
//write node info to jdisk
void write_node(B_Tree *b_tree, Tree_Node *node, unsigned int lba) {
	void *buf;
	buf = (void *) malloc(sizeof(char) * JDISK_SECTOR_SIZE);
	memcpy(buf, &(node->internal), 1);
	memcpy(buf + 1, &(node->nkeys), 1);

	for (int i = 0; i < node->nkeys; i++) {
		memcpy(buf + 2 + (i * b_tree->key_size), node->keys[i],
				b_tree->key_size);
	}

	for (int i = 0; i < node->nkeys + 1; i++) {
		memcpy(
				buf
						+ ((JDISK_SECTOR_SIZE - ((b_tree->MAXKEY + 1) * 4))
								+ (4 * i)), &(node->lbas[i]), 4);
	}

	jdisk_write(b_tree->disk, lba, buf);
	free(buf);
}
// create a blank tree node
Tree_Node * blank_tree_node(Tree_Node *parent, B_Tree* bt, int parent_index,
		unsigned int lba) {
	Tree_Node *root = newTreeNode;
	root->nkeys = 0;
	root->internal = 0;
	root->lba = lba;
	root->parent = NULL;
	root->keys = (unsigned char **) malloc(
			sizeof(unsigned char *) * bt->MAXKEY);
	for (int i = 0; i < bt->MAXKEY; i++) {
		root->keys[i] = (unsigned char *) malloc(ucharSize * bt->key_size);
	}
	root->lbas = (unsigned int *) malloc(uintSize * (bt->MAXVALS + 1));
	root->children = (Tree_Node **) malloc(
			sizeof(Tree_Node *) * (bt->MAXVALS + 1));
	root->lbas[0] = 0;

	return root;
}
// create a empty b tree
void *b_tree_create(char *filename, long size, int key_size) {
	B_Tree *tree;
	tree = (B_Tree *) malloc(sizeof(B_Tree));
	tree->disk = jdisk_create(filename, size);
	tree->key_size = key_size;
	tree->root_lba = 1;
	tree->first_free_block = 2;
	tree->size = size;
	tree->num_lbas = size / JDISK_SECTOR_SIZE;
	tree->MAXKEY = (JDISK_SECTOR_SIZE - 6) / (key_size + 4);
	tree->MAXVALS = tree->MAXKEY + 1;
	tree->root = blank_tree_node(NULL, tree, 0, 1);
	write_sector_0(tree);
	write_node(tree, tree->root, tree->root->lba);

	return (void *) tree;
}
// get b tree from jdisk
void *b_tree_attach(char *filename) {
	B_Tree *tree;
	void *buf;

	tree = (B_Tree *) malloc(sizeof(B_Tree));
	buf = (void *) malloc(sizeof(char) * JDISK_SECTOR_SIZE);

	tree->disk = jdisk_attach(filename);
	jdisk_read(tree->disk, 0, buf);
	memcpy((void *) tree, buf,
			sizeof(int) + sizeof(unsigned int) + sizeof(unsigned long));
	memcpy((void *) &tree->size,
			buf + sizeof(int) + sizeof(unsigned int) + sizeof(unsigned long),
			sizeof(unsigned long));
	tree->num_lbas = tree->size / JDISK_SECTOR_SIZE;
	tree->MAXKEY = (JDISK_SECTOR_SIZE - 6) / (tree->key_size + 4);
	tree->MAXVALS = tree->MAXKEY + 1;

	free(buf);
	return tree;
}
// get node from jdisk
Tree_Node *get_node_from_jdisk(B_Tree *b_tree, unsigned int lba) {
	Tree_Node *node;
	void *buf;

	node = blank_tree_node(NULL,b_tree,0,lba);
	buf = (void *) malloc(sizeof(char) * JDISK_SECTOR_SIZE);

	jdisk_read(b_tree->disk, lba, buf);

	memcpy(&node->internal, buf, 1);
	memcpy(&node->nkeys, buf + 1, 1);

	for (int i = 0; i < b_tree->MAXKEY; i++) {
		if (i < node->nkeys)
			memcpy(node->keys[i], buf + 2 + (i * b_tree->key_size),
					b_tree->key_size);
	}

	for (int i = 0; i < node->nkeys + 1; i++) {
		memcpy(&(node->lbas[i]),
				buf
						+ ((JDISK_SECTOR_SIZE - ((b_tree->MAXKEY + 1) * 4))
								+ (4 * i)), 4);
	}
	node->parent = NULL;

	free(buf);
	return node;
}

void add_key_and_split_node(B_Tree *b_tree, Tree_Node *root, void *key, int lba) {
	int i, j, insertion_index;
	unsigned char **cpKyes;
	unsigned int *cpLbas;
	Tree_Node *right;
	Tree_Node *new_root;

	if (root->nkeys == b_tree->MAXKEY) {

		cpKyes = (unsigned char **) malloc(
				sizeof(unsigned char *) * (b_tree->MAXKEY + 1));
		cpLbas = (unsigned int *) malloc(
				sizeof(unsigned int) * (b_tree->MAXVALS + 1));

		for (i = 0; i < b_tree->MAXKEY + 1; i++) {
			cpKyes[i] = (unsigned char *) malloc(
					sizeof(unsigned char) * b_tree->key_size);
			if (i < b_tree->MAXKEY) {
				memcpy((void *) cpKyes[i], (void *) root->keys[i],
						b_tree->key_size);
			}
		}

		for (i = 0; i < b_tree->MAXVALS; i++) {
			cpLbas[i] = root->lbas[i];
		}

		for (i = 0; i < b_tree->MAXKEY; i++) {
			if (memcmp(key, (void *) cpKyes[i], b_tree->key_size) < 0) {

				break;
			}
		}

		for (j = b_tree->MAXKEY; j > i; j--) {
			memcpy(cpKyes[j], cpKyes[j - 1], b_tree->key_size);
		}

		for (j = b_tree->MAXVALS; j > i; j--) {
			cpLbas[j] = cpLbas[j - 1];
		}

		memcpy(cpKyes[i], key, b_tree->key_size);

		cpLbas[i] = lba;

		right = blank_tree_node(root->parent, b_tree, 0,
				b_tree->first_free_block);
		right->nkeys = b_tree->MAXKEY - ((b_tree->MAXKEY + 1) / 2);
		right->internal = root->internal;
		right->lba = b_tree->first_free_block;
		right->parent = root->parent;
		b_tree->first_free_block++;

		for (i = ((b_tree->MAXKEY + 1) / 2) + 1; i < b_tree->MAXKEY + 1; i++) {
			memcpy((void *) right->keys[i - (((b_tree->MAXKEY + 1) / 2) + 1)],
					(void *) cpKyes[i], b_tree->key_size);
		}

		root->nkeys = (b_tree->MAXKEY + 1) / 2;
		for (i = 0; i < (b_tree->MAXKEY + 1) / 2; i++) {
			memcpy((void *) root->keys[i], (void *) cpKyes[i],
					b_tree->key_size);
		}

		for (i = 0; i < ((b_tree->MAXKEY + 1) / 2) + 1; i++)
			root->lbas[i] = cpLbas[i];

		for (i = ((b_tree->MAXKEY + 1) / 2) + 1; i < b_tree->MAXVALS + 1; i++)
			right->lbas[i - (((b_tree->MAXKEY + 1) / 2) + 1)] = cpLbas[i];

		if (root->parent == NULL) {
			new_root = blank_tree_node(NULL, b_tree, 0,
					b_tree->first_free_block);
			new_root->nkeys = 1;
			new_root->internal = 1;
			new_root->lba = b_tree->first_free_block;
			b_tree->first_free_block++;
			b_tree->root_lba = new_root->lba;
			for (i = 0; i < b_tree->MAXKEY; i++) {
				new_root->keys[i] = (unsigned char *) malloc(
						sizeof(unsigned char) * b_tree->key_size);
			}
			memcpy((void *) new_root->keys[0],
					(void *) cpKyes[(b_tree->MAXKEY + 1) / 2],
					b_tree->key_size);
			new_root->lbas[0] = root->lba;
			new_root->lbas[1] = right->lba;
			new_root->children[0] = root;
			new_root->children[1] = right;
			new_root->parent = NULL;
			root->parent = new_root;
			right->parent = new_root;
			root->parent_index = 0;
			right->parent_index = 1;

			write_node(b_tree, root, root->lba);
			write_node(b_tree, right, right->lba);
			write_node(b_tree, new_root, new_root->lba);
			write_sector_0(b_tree);
			b_tree->root = new_root;
		} else {

			for (i = 0; i < root->parent->nkeys + 1; i++) {
				if (root->parent->lbas[i] == root->lba) {
					root->parent->lbas[i] = right->lba;
					break;
				}
			}
			add_key_and_split_node(b_tree, root->parent,
					(void *) cpKyes[(b_tree->MAXKEY + 1) / 2], root->lba);
			right->parent = root->parent;
			write_node(b_tree, root, root->lba);
			write_node(b_tree, right, right->lba);
		}

		for (i = 0; i < b_tree->MAXKEY + 1; i++)
			free(cpKyes[i]);
		free(cpKyes);
		free(cpLbas);
	} else {
		insertion_index = 0;

		for (i = 0; i < root->nkeys; i++) {
			if (memcmp(key, (void *) root->keys[i], b_tree->key_size) < 0) {

				break;
			}
		}

		insertion_index = i;

		for (j = root->nkeys; j > insertion_index; j--) {
			memcpy((void *) root->keys[j], (void *) root->keys[j - 1],
					b_tree->key_size);
		}

		for (j = root->nkeys + 1; j > insertion_index; j--) {
			root->lbas[j] = root->lbas[j - 1];
		}

		memcpy((void *) root->keys[insertion_index], key, b_tree->key_size);
		root->lbas[insertion_index] = lba;
		root->nkeys++;
		write_node(b_tree, root, root->lba);
	}
	return;
}
//get node lba
unsigned int get_node_val(B_Tree *b_tree, unsigned int lba) {
	Tree_Node *node;
	int return_lba;

	node = NULL;

	node = get_node_from_jdisk(b_tree, lba);
	while (node->internal) {
		node = get_node_from_jdisk(b_tree, node->lbas[node->nkeys]);
	}
	return_lba = node->lbas[node->nkeys];
	return return_lba;
}
//print tree node BSF
void print_node(void *b_tree, Tree_Node*root) {
	if (root == NULL)
		return;
	printf("LBA ox%-8x.  Internal: %-2d\n", root->lba, c2i(root->internal));
	for (int i = 0; i < c2i(root->nkeys); i++) {
		printf("  Entry %d: Key: %-32sLBA: ox%-8x\n", i,
				(char*) (root->keys + i), root->lbas[i]);
	}
	printf("  Entry %d:%s%-32sLBA: ox%-8x\n", c2i(root->nkeys), "      ", "",
			root->lbas[c2i(root->nkeys)]);
	if (root->internal == 0)
		return;
	for (int i = 0; i < c2i(root->nkeys) + 1; i++) {
		print_node(b_tree, get_node_from_jdisk(b_tree, root->lbas[i]));
	}
}
//print b tree
void b_tree_print_tree(void *b_tree) {
	print_node(b_tree, ((B_Tree*) b_tree)->root);
}
//inser key value into b tree
unsigned int b_tree_insert(void *b_tree, void *key, void *record) {
	int i;
	Tree_Node *node;
	Tree_Node *prev;
	B_Tree* bt = (B_Tree *) b_tree;
	unsigned long lba;


	if (bt->first_free_block >= bt->size / JDISK_SECTOR_SIZE)
		return 0;

	node = get_node_from_jdisk(bt, bt->root_lba);
	while (node->internal) {
		for (i = 0; i < node->nkeys + 1; i++) {
			if (i == node->nkeys) {
				prev = node;
				node = get_node_from_jdisk(bt, node->lbas[i]);
				node->parent = prev;
				break;
			}
			if (memcmp(key, (void *) node->keys[i],
					bt->key_size) < 0) {
				prev = node;
				node = get_node_from_jdisk(bt, node->lbas[i]);
				node->parent = prev;
				break;
			}
			if (memcmp(key, (void *) node->keys[i],
					bt->key_size) == 0) {
				lba = get_node_val(bt, node->lbas[i]);
				jdisk_write(bt->disk, lba, record);
				return lba;
			}
		}
	}

	for (i = 0; i < node->nkeys; i++) {
		if (memcmp(key, (void *) node->keys[i], bt->key_size)
				== 0) {
			jdisk_write(bt->disk, node->lbas[i], record);
			lba = node->lbas[i];
			return lba;
		}
	}

	lba = bt->first_free_block;
	bt->first_free_block++;
	jdisk_write(bt->disk, lba, record);
	add_key_and_split_node(bt, node, key, lba);

	write_sector_0(bt);
	return lba;
}
//find node
unsigned int find_node(void *b_tree, Tree_Node*root, void *key) {
	if (root == NULL)
		return 0;
	for (int i = 0; i < root->nkeys; i++) {
		int cmp = memcmp(key, (void *) root->keys[i],
				((B_Tree *) b_tree)->key_size);
		if (cmp == 0) {
			if (root->internal == 0) {
				return root->lbas[i];
			}
			return get_node_val((B_Tree *) b_tree, root->lbas[i]);
		} else if (cmp < 0) {
			return find_node(b_tree,
					get_node_from_jdisk((B_Tree *) b_tree, root->lbas[i]), key);
		}
	}
	if (c2i(root->internal) == 0) {
		return 0;
	}
	return find_node(b_tree,
			get_node_from_jdisk((B_Tree *) b_tree, root->lbas[root->nkeys]),
			key);
}
// find key not find return 0 found return lba
unsigned int b_tree_find(void *b_tree, void *key) {
	return find_node(b_tree,
			get_node_from_jdisk((B_Tree *) b_tree,
					((B_Tree *) b_tree)->root_lba), key);
}
// get jdisk
void *b_tree_disk(void *b_tree) {
	return ((B_Tree *) b_tree)->disk;
}
// get key size
int b_tree_key_size(void *b_tree) {
	return ((B_Tree *) b_tree)->key_size;
}
