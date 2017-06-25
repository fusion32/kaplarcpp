#include "log.h"
#include "scheduler.h"
#include "work.h"
#include "system.h"
#include <stdio.h>

#include "avl_tree.h"

template<typename T>
struct node{
	T key;
	int height;
	node *parent;
	node *left;
	node *right;
};

template<typename T>
struct avl_tree_struct{
	node<T> *root;
};

template<typename T>
static void print_node(node<T> *n)
{
	if(n == nullptr) return;
	if(n->height > 2){
		print_node(n->left);
		print_node(n->right);
	}
	else{
		if(n->left != nullptr)	LOG("%d", n->left->key);
					LOG("%d", n->key);
		if(n->right != nullptr)	LOG("%d", n->right->key);
	}
}

int main(int argc, char **argv)
{
	// hack to retrieve the tree root
	kp::avl_tree<int, 256> tree;
	auto ptr = (avl_tree_struct<int>*)&tree;
	for(int i = 0; i < 256; ++i)
		tree.insert((i*1337)%64);
	print_node(ptr->root);
	getchar();
	return 0;
}

/*

int main(int argc, char **argv)
{
	work_init();
	scheduler_init();

	kp::sch_entry *entry[4096];
	for(int i = 0; i < 4096; ++i){
		entry[i] = scheduler_add((i+1)*1000, [](void){
			LOG("hello");
		});
	}

	for(int i = 0; i < 4096; ++i){
		scheduler_pop(entry[i]);
	}

	getchar();
	scheduler_shutdown();
	work_shutdown();
	return 0;
}

*/