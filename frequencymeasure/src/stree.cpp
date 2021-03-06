//
// STREE.CPP - Suffix tree creation
//
// Mark Nelson, April, 1996
//
// This code has been tested with Borland C++ and
// Microsoft Visual C++.
//
// This program asks you for a line of input, then
// creates the suffix tree corresponding to the given
// text. Additional code is provided to validate the
// resulting tree after creation.
//

#include "stree.h"

using namespace std;



/*int  T[ MAX_LENGTH ];
int N;

int CurrentString[ MAX_LENGTH ];
int GoodSuffixes[ MAX_LENGTH ];
int BranchCount[ MAX_LENGTH * 2 ] = { 0 };
Edge Edges[ HASH_TABLE_SIZE ];
Node Nodes[ MAX_LENGTH * 2 ];*/

int  *T;
int N;

int *CurrentString;
int *GoodSuffixes;
int *BranchCount;
Edge **Edges;
Node *Nodes;

BuildSuffixTree::BuildSuffixTree() : active(0,0,-1) {

	Count = 1;
	hash_t_s = 0;
	max_len = 0;
}


BuildSuffixTree::~BuildSuffixTree() {
	Edge *pointer;
	Edge *pointer_temp;
	for (int i = 0 ; i < hash_t_s; i++) {
		pointer = save[i];
		while (pointer != NULL) {
			pointer_temp = pointer->next;
			delete pointer;
			pointer = pointer_temp;
		}
	}
	delete [] save;
}


extern int Count;
//
// The maximum input string length this program
// will handle is defined here.  A suffix tree
// can have as many as 2N edges/nodes.  The edges
// are stored in a hash table, whose size is also
// defined here.
//


//
// This is the hash table where all the currently
// defined edges are stored.  You can dump out
// all the currently defined edges by iterating
// through the table and finding edges whose start_node
// is not -1.
//


//
// The array of defined nodes.  The count is 1 at the
// start because the initial tree has the root node
// defined, with no children.
//


//
// The input buffer and character count.  Please note that N
// is the length of the input string -1, which means it
// denotes the maximum index in the input buffer.
//


//
// The default ctor for Edge just sets start_node
// to the invalid value.  This is done to guarantee
// that the hash table is initially filled with unused
// edges.
//


Edge::Edge()
{
    start_node = -1;
	next = NULL;
}

//
// I create new edges in the program while walking up
// the set of suffixes from the active point to the
// endpoint.  Each time I create a new edge, I also
// add a new node for its end point.  The node entry
// is already present in the Nodes[] array, and its
// suffix node is set to -1 by the default Node() ctor,
// so I don't have to do anything with it at this point.
//

Edge::Edge( int init_first, int init_last, int parent_node )
{
    first_char_index = init_first;
    last_char_index = init_last;
    start_node = parent_node;
    end_node = Count++;
	next = NULL;
}


//
// Edges are inserted into the hash table using this hashing
// function.
//

int Edge::Hash( int node, int c )
{///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    return ( ( node << 11 ) + c ) % HASH_TABLE_SIZE;  /////////////////////////////  8!!!////////////////////////
}///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// A given edge gets a copy of itself inserted into the table
// with this function.  It uses a linear probe technique, which
// means in the case of a collision, we just step forward through
// the table until we find the first unused slot.
//

void Edge::Insert()
{
    int i = Hash( start_node, T[ first_char_index ] );
	Edge *point = Edges[i];

	if (point == NULL) {
		Edges[i] = new Edge;
		Edges[i]->start_node = this->start_node;
		Edges[i]->end_node = this->end_node;
		Edges[i]->first_char_index = this->first_char_index;
		Edges[i]->last_char_index = this->last_char_index;
		//      Edge
		Edges[i]->next = new Edge;
		Edges[i]->next->start_node = -1;
		Edges[i]->next->next = NULL;
		return;
	}

	while (point->start_node != -1) {
		point = point->next;
	}

	point->start_node = this->start_node;
	point->end_node = this->end_node;
	point->first_char_index = this->first_char_index;
	point->last_char_index = this->last_char_index;
	//      Edge
	point->next = new Edge;
	point->next->start_node = -1;
	point->next->next = NULL;
	//memAllocated += sizeof(Edge);
}

//
// Removing an edge from the hash table is a little more tricky.
// You have to worry about creating a gap in the table that will
// make it impossible to find other entries that have been inserted
// using a probe.  Working around this means that after setting
// an edge to be unused, we have to walk ahead in the table,
// filling in gaps until all the elements can be found.
//
// Knuth, Sorting and Searching, Algorithm R, p. 527
//

void Edge::Remove()
{
    int i = Hash( start_node, T[ first_char_index ] );
	Edge *point = Edges[i];
	Edge *point_prev = point;
    while ( point->start_node != start_node || point->first_char_index != first_char_index ) {
		point_prev = point;
		point = point->next;
	}
	point_prev->next = point->next;
	if (point_prev != point) delete [] point;
	else {
		if (point->next == NULL) point->start_node = -1;
		else {
			point->start_node = point->next->start_node;
			point->end_node = point->next->end_node;
			point->first_char_index = point->next->first_char_index;
			point->last_char_index = point->next->last_char_index;
			point_prev = point->next->next;
			delete [] point->next; /////////////??????????????????????????????
			point->next = point_prev;
		}
	}
	//memAllocated -= sizeof(Edge);
}

//
// The whole reason for storing edges in a hash table is that it
// makes this function fairly efficient.  When I want to find a
// particular edge leading out of a particular node, I call this
// function.  It locates the edge in the hash table, and returns
// a copy of it.  If the edge isn't found, the edge that is returned
// to the caller will have start_node set to -1, which is the value
// used in the hash table to flag an unused entry.
//

Edge Edge::Find( int node, int c )
{
    int i = Hash( node, c );
	Edge *point = Edges[i];
	if (Edges[i] == NULL) {
		Edges[i] = new Edge;
		Edges[i]->start_node = -1;
		Edges[i]->next = NULL;
		return *Edges[i];
	}
	while (point->start_node != -1) {
		if ( (point->start_node == node) && (c == T[point->first_char_index]) )
			return *point;
		point = point->next;
	}
	return *point;

	/*
    for ( ; ; ) {
        if ( Edges[ i ].start_node == node )
            if ( c == T[ Edges[ i ].first_char_index ] )
                return Edges[ i ];
        if ( Edges[ i ].start_node == -1 )
            return Edges[ i ];
        i = ++i % HASH_TABLE_SIZE;
    }*/
}

//
// When a suffix ends on an implicit node, adding a new character
// means I have to split an existing edge.  This function is called
// to split an edge at the point defined by the Suffix argument.
// The existing edge loses its parent, as well as some of its leading
// characters.  The newly created edge descends from the original
// parent, and now has the existing edge as a child.
//
// Since the existing edge is getting a new parent and starting
// character, its hash table entry will no longer be valid.  That's
// why it gets removed at the start of the function.  After the parent
// and start char have been recalculated, it is re-inserted.
//
// The number of characters stolen from the original node and given
// to the new node is equal to the number of characters in the suffix
// argument, which is last - first + 1;
//
int Number_of_nodes = 0;
int Edge::SplitEdge( Suffix &s )
{
    Remove();
    Edge *new_edge =
      new Edge( first_char_index,
                first_char_index + s.last_char_index - s.first_char_index,
                s.origin_node );
	Number_of_nodes++;
	/*if (Number_of_nodes >= 2000) {
		cout << "KFHLSDKGHLSDKGHSLDKGHLKSGHL:GHK" << endl;
		exit(0);
	}*/
    new_edge->Insert();
    Nodes[ new_edge->end_node ].suffix_node = s.origin_node;
    first_char_index += s.last_char_index - s.first_char_index + 1;
    start_node = new_edge->end_node;
    Insert();
    return new_edge->end_node;
}

//
// This routine prints out the contents of the suffix tree
// at the end of the program by walking through the
// hash table and printing out all used edges.  It
// would be really great if I had some code that will
// print out the tree in a graphical fashion, but I don't!
//

void BuildSuffixTree::dump_edges( int current_n )
{
    /*cout << " Start  End  Suf  First Last  String\n";
    for ( int j = 0 ; j < HASH_TABLE_SIZE ; j++ ) {
        Edge *s = Edges + j;
        if ( s->start_node == -1 )
            continue;
        cout << setw( 5 ) << s->start_node << " "
             << setw( 5 ) << s->end_node << " "
             << setw( 3 ) << Nodes[ s->end_node ].suffix_node << " "
             << setw( 5 ) << s->first_char_index << " "
             << setw( 6 ) << s->last_char_index << "  ";
        int top;
        if ( current_n > s->last_char_index )
            top = s->last_char_index;
        else
            top = current_n;
        for ( int l = s->first_char_index ;
                  l <= top;
                  l++ )
            cout << T[ l ];
        cout << "\n";
    }*/
}

//
// A suffix in the tree is denoted by a Suffix structure
// that denotes its last character.  The canonical
// representation of a suffix for this algorithm requires
// that the origin_node by the closest node to the end
// of the tree.  To force this to be true, we have to
// slide down every edge in our current path until we
// reach the final node.

void Suffix::Canonize()
{
    if ( !Explicit() ) {
        Edge edge = Edge::Find( origin_node, T[ first_char_index ] );
        int edge_span = edge.last_char_index - edge.first_char_index;
        while ( edge_span <= ( last_char_index - first_char_index ) ) {
            first_char_index = first_char_index + edge_span + 1;
            origin_node = edge.end_node;
            if ( first_char_index <= last_char_index ) {
               edge = Edge::Find( edge.end_node, T[ first_char_index ] );
                edge_span = edge.last_char_index - edge.first_char_index;
            };
        }
    }
}

//
// This routine constitutes the heart of the algorithm.
// It is called repetitively, once for each of the prefixes
// of the input string.  The prefix in question is denoted
// by the index of its last character.
//
// At each prefix, we start at the active point, and add
// a new edge denoting the new last character, until we
// reach a point where the new edge is not needed due to
// the presence of an existing edge starting with the new
// last character.  This point is the end point.
//
// Luckily for use, the end point just happens to be the
// active point for the next pass through the tree.  All
// we have to do is update it's last_char_index to indicate
// that it has grown by a single character, and then this
// routine can do all its work one more time.
//
int Number_of_it = 0;

void BuildSuffixTree::AddPrefix( Suffix &active, int last_char_index )
{
    int parent_node;
    int last_parent_node = -1;

    for ( ; ; ) {
		Number_of_it++;
        Edge edge;
	cout << "check1" << endl;
        parent_node = active.origin_node;
//
// Step 1 is to try and find a matching edge for the given node.
// If a matching edge exists, we are done adding edges, so we break
// out of this big loop.
//
        if ( active.Explicit() ) {
		cout << "Trying find1" << endl;
            edge = Edge::Find( active.origin_node, T[ last_char_index ] );
            if ( edge.start_node != -1 )
                break;
        } else { //implicit node, a little more complicated
		cout << "Trying find2" << endl;
            edge = Edge::Find( active.origin_node, T[ active.first_char_index ] );
            int span = active.last_char_index - active.first_char_index;
            if ( T[ edge.first_char_index + span + 1 ] == T[ last_char_index ] )
                break;
            parent_node = edge.SplitEdge( active );
        }
	cout << "check2" << endl;
//
// We didn't find a matching edge, so we create a new one, add
// it to the tree at the parent node position, and insert it
// into the hash table.  When we create a new node, it also
// means we need to create a suffix link to the new node from
// the last node we visited.
//
        Edge *new_edge = new Edge( last_char_index, N, parent_node );
		Number_of_nodes++;
		/*if (Number_of_nodes >= 2000) {
			cout << "KFHLSDKGHLSDKGHSLDKGHLKSGHL:GHK" << endl;
			exit(0);
		}*/
        new_edge->Insert();
        if ( last_parent_node > 0 )
            Nodes[ last_parent_node ].suffix_node = parent_node;
        last_parent_node = parent_node;
//
// This final step is where we move to the next smaller suffix
//

	cout << "check3" << endl;
        if ( active.origin_node == 0 )
            active.first_char_index++;
        else
            active.origin_node = Nodes[ active.origin_node ].suffix_node;
        active.Canonize();
    }
    if ( last_parent_node > 0 )
        Nodes[ last_parent_node ].suffix_node = parent_node;
    active.last_char_index++;  //Now the endpoint is the next active point
    active.Canonize();
};


//
// The validation code consists of two routines.  All it does
// is traverse the entire tree.  walk_tree() calls itself
// recursively, building suffix strings up as it goes.  When
// walk_tree() reaches a leaf node, it checks to see if the
// suffix derived from the tree matches the suffix starting
// at the same point in the input text.  If so, it tags that
// suffix as correct in the GoodSuffixes[] array.  When the tree
// has been traversed, every entry in the GoodSuffixes array should
// have a value of 1.
//
// In addition, the BranchCount[] array is updated while the tree is
// walked as well.  Every count in the array has the
// number of child edges emanating from that node.  If the node
// is a leaf node, the value is set to -1.  When the routine
// finishes, every node should be a branch or a leaf.  The number
// of leaf nodes should match the number of suffixes (the length)
// of the input string.  The total number of branches from all
// nodes should match the node count.
//


void BuildSuffixTree::validate()
{
	int i = 0;
    for ( i = 0 ; i < N ; i++ )
        GoodSuffixes[ i ] = 0;
    walk_tree( 0, 0 );
    int error = 0;
    for ( i = 0 ; i < N ; i++ )
        if ( GoodSuffixes[ i ] != 1 ) {
//            cout << "Suffix " << i << " count wrong!\n";
            error++;
        }
    if ( error == 0 );
 //       cout << "All Suffixes present!\n";
    int leaf_count = 0;
    int branch_count = 0;
    for ( i = 0 ; i < Count ; i++ ) {
        if ( BranchCount[ i ] == 0 ) {;}
            //cout << "Logic error on node "
            //     << i
             //    << ", not a leaf or internal node!\n";
        else if ( BranchCount[ i ] == -1 )
            leaf_count++;
        else
            branch_count += BranchCount[ i ];
    }
    //cout << "Leaf count : "
    //     << leaf_count
    //     << ( leaf_count == ( N + 1 ) ? " OK" : " Error!" )
    //     << "\n";
    //cout << "Branch count : "
    //     << branch_count
     //    << ( branch_count == (Count - 1) ? " OK" : " Error!" )
     //    << endl;
}

int BuildSuffixTree::walk_tree( int start_node, int last_char_so_far )
{
    int edges = 0;
    for ( int i = 0 ; i < 2048; i++ ) { ////////////////////   256!!!!!!!!!!!!!!!!!!!!!!!!!
        Edge edge = Edge::Find( start_node, i );
        if ( edge.start_node != -1 ) {
            if ( BranchCount[ edge.start_node ] < 0 )
                cerr << "Logic error on node "
                     << edge.start_node
                     << '\n';
            BranchCount[ edge.start_node ]++;
            edges++;
            int l = last_char_so_far;
            for ( int j = edge.first_char_index ; j <= edge.last_char_index ; j++ )
                CurrentString[ l++ ] = T[ j ];
			size_of_current_string = l;
            CurrentString[ l ] = '0';
            if ( walk_tree( edge.end_node, l ) ) {
                if ( BranchCount[ edge.end_node ] > 0 )
                        cerr << "Logic error on node "
                             << edge.end_node
                             << "\n";
                BranchCount[ edge.end_node ]--;
            }
        }
    }
//
// If this node didn't have any child edges, it means we
// are at a leaf node, and can check on this suffix.  We
// check to see if it matches the input string, then tick
// off it's entry in the GoodSuffixes list.
//
    if ( edges == 0 ) {
//        cout << "Suffix : ";
        /*for ( int m = 0 ; m < last_char_so_far ; m++ )
            cout << CurrentString[ m ];
       cout << "\n";*/
        //GoodSuffixes[ strlen( CurrentString ) - 1 ]++;
		GoodSuffixes[ size_of_current_string - 1 ]++;
  /*      cout << "comparing: " << ( T + N - size_of_current_string + 1 )
             << " to " << CurrentString << endl;*/
        //if ( strcmp(T + N - size_of_current_string + 1, CurrentString ) != 0 )
          //  cout << "Comparison failure!\n";
        return 1;
    } else
        return 0;
}


void BuildSuffixTree::BuildTree(size_t *str, size_t size, size_t max_l) {
	cout << "Blondinkam privet!" << endl;
	/*ofstream ff("c.txt",ios::app);
	for (int i = 0; i < size; i++) {
		ff << str[i] << " ";
	}
	ff << endl << endl << size << endl << endl;*/
	MAX_LENGTH = max_l;
	max_len = max_l;
	HASH_TABLE_SIZE = MAX_LENGTH*2.01;
	hash_t_s = HASH_TABLE_SIZE;
	T = new int[MAX_LENGTH];
	CurrentString = new int[MAX_LENGTH];
	GoodSuffixes = new int[MAX_LENGTH];
	BranchCount = new int[MAX_LENGTH*2];
	Edges = new Edge*[HASH_TABLE_SIZE];
	Nodes = new Node[MAX_LENGTH*2];
	for (int i = 0; i < MAX_LENGTH; i++) {
		CurrentString[i] = 0;
		GoodSuffixes[i] = 0;
		BranchCount[i] = 0;
		Nodes[i].suffix_node = -1;
	}

	int s = MAX_LENGTH*2;
	for (int i = MAX_LENGTH; i< s; i++) {
		BranchCount[i] = 0;
		Nodes[i].suffix_node = -1;
	}

	for (int i = 0; i < HASH_TABLE_SIZE; i++) {
		Edges[i] = NULL;
	}


	for (int i = 0; i < size; i++) {
		T[i] = str[i];
	}
	N = size - 1;
	cout << "Slozhnyi cycle" << endl;
	for ( int i = 0 ; i <= N ; i++ ) {
		//cout << i << endl;
		AddPrefix( active, i );
	}

	cout << "No korotkii!" << endl;
	save = Edges;
	delete [] GoodSuffixes;
	delete [] BranchCount;
	delete [] Nodes;
	delete [] CurrentString;
	delete [] T;
	return;

}
