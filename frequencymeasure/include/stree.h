//
// When a new tree is added to the table, we step
// through all the currently defined suffixes from
// the active point to the end point.  This structure
// defines a Suffix by its final character.
// In the canonical representation, we define that last
// character by starting at a node in the tree, and
// following a string of characters, represented by
// first_char_index and last_char_index.  The two indices
// point into the input string.  Note that if a suffix
// ends at a node, there are no additional characters
// needed to characterize its last character position.
// When this is the case, we say the node is Explicit,
// and set first_char_index > last_char_index to flag
// that.
//

#pragma once

#include "Types.h"

class Suffix {
public :
	int origin_node;
	int first_char_index;
	int last_char_index;
	Suffix( int node, int start, int stop )
		: origin_node( node ),
		first_char_index( start ),
		last_char_index( stop ){};
	int Explicit(){ return first_char_index > last_char_index; }
	int Implicit(){ return last_char_index >= first_char_index; }
	void Canonize();
};

//
// The suffix tree is made up of edges connecting nodes.
// Each edge represents a string of characters starting
// at first_char_index and ending at last_char_index.
// Edges can be inserted and removed from a hash table,
// based on the Hash() function defined here.  The hash
// table indicates an unused slot by setting the
// start_node value to -1.
//

class Edge {
public :
	int first_char_index;
	int last_char_index;
	int end_node;
	int start_node;
	void Insert();
	void Remove();
	Edge *next;
	Edge();
	Edge( int init_first_char_index,
		int init_last_char_index,
		int parent_node );
	int SplitEdge( Suffix &s );
	static Edge Find( int node, int c );
	static int Hash( int node, int c );
};


//
//  The only information contained in a node is the
//  suffix link. Each suffix in the tree that ends
//  at a particular node can find the next smaller suffix
//  by following the suffix_node link to a new node.  Nodes
//  are stored in a simple array.
//
static int Count;

class Node {
public :
	int suffix_node;
	Node() { suffix_node = -1; Count = 1;}
};

static int MAX_LENGTH = 40000;
static int HASH_TABLE_SIZE = 80100;

class BuildSuffixTree {
public:
	BuildSuffixTree();
	~BuildSuffixTree();
	int max_len;
	int hash_t_s;
	Edge **save;
	Suffix active;
	void BuildTree(size_t *str, size_t size, size_t max_l);
	void validate();
	int walk_tree( int start_node, int last_char_so_far );
	void dump_edges( int current_n );
	void AddPrefix( Suffix &active, int last_char_index );
	int size_of_current_string;
};
