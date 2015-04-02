
#include "KSpectrum.h"

using namespace std;
using namespace Similarity;

int NNN = 0;
int MMM = 0;
const float lambda = 1;
const int SIZE_INT = sizeof(int);
const int SIZE_CHAR = sizeof(char);
const int SIZE_OF_T = sizeof(size_t);


KSpectrum::KSpectrum() {
	number_of_examples = 0;
	ex = NULL;
	strings_from_docs = NULL;
	strings_from_docs_size = NULL;
}

KSpectrum::KSpectrum(size_t k, Document **docs, size_t cache_size) {
	kernel = k;
	number_of_examples = 0;
	ex = NULL;
	strings_from_docs = NULL;
	strings_from_docs_size = NULL;
	this->docs = docs;
	MakeCache(cache_size);
}

KSpectrum::~KSpectrum() {
	clear();
}

void KSpectrum::DocsToStrings(size_t **strings_from_docs, Document **docs) {

	for (int i = 0; i < number_of_examples; i++) {
		size_t str_size;
		str_size = docs[i]->size()+1;
		strings_from_docs[i] = new size_t[str_size];
		strings_from_docs_size[i] = str_size;
		docs[i]->read(strings_from_docs[i],str_size-1);
		strings_from_docs[i][str_size-1] = 0;
	}
}

real KSpectrum::similarity(Document &doc1, Document &doc2) {
	BuildSuffixTree *tree1 = new BuildSuffixTree();
	BuildSuffixTree *tree2 = new BuildSuffixTree();
	size_t str_size1 = doc1.size()+1;
	size_t *str1 = new size_t[str_size1];
	doc1.read(str1,str_size1-1);
	str1[str_size1-1] = 0;

	size_t str_size2 = doc2.size()+1;
	size_t *str2 = new size_t[str_size2];
	doc2.read(str2,str_size2-1);
	str2[str_size2-1] = 0;

	CreateSuffixTree(str1,str_size1,tree1);
	CreateSuffixTree(str2,str_size2,tree2);
	float u = 0;
	double a = 0;

	u = CountWithTrees(str1,str_size1,tree1,str1)*CountWithTrees(str2,str_size2,tree2,str2);
	if (u == 0) a = CountWithTrees(str1,str_size1,tree2,str2);
	else a = CountWithTrees(str1,str_size1,tree2,str2)/sqrt(u);
	delete [] str1;
	delete [] str2;
	delete tree1;
	delete tree2;
	return 0;
}


bool KSpectrum::IntStrcmp(size_t *str1, size_t *str2) {
 	for (int i = 0; i < kernel; i++) {
		if (str1[i] != str2[i]) return false;
	}
	return true;
}

double Schetchik = 0;
double a_time;
double b_time;


float KSpectrum::CountWithTrees(size_t *doc, size_t str_size, BuildSuffixTree *tree, size_t *source) {

	int i = 0;
	int o = 0;
	int hash_size = 0;
	float result = 0;
	Edge *ed;
	int end_node = 0;
	bool check = true;


	hash_size = tree->hash_t_s;
	Edge **temp = tree->save;

	while (kernel+i <= str_size-1) {
		check = true;
		end_node = 0;
		//checking string from i to kernel+i
		for (int j = 0; j < kernel; j++) {

			//the same as Edge::Find, but adapted for me
			o = ((end_node << 11) + doc[i+j]) % hash_size;
			ed = temp[o];
			if (ed == NULL) {
				check = false;
				break;
			}
			while (ed->start_node != -1) {
				if ( (ed->start_node == end_node) && (doc[i+j] == source[ed->first_char_index]) )
					break;
				ed = ed->next;
			}
			if ( ed->start_node == -1) { //if such node doesn't exist, break
				check = false;
				break;
			}

			//if such node exists
			for (int p = ed->first_char_index; p < ed->last_char_index+1; p++) {
				if (j >= kernel) { //already checked this whole substring
					if (doc[i+j] == 0) break;
					result++;
					check = false;
					break;
				}
				if (doc[i+j] != source[p]) {
					check = false;
					break;
				}
				j++;
			}
			j--;
			if (!check) break;
			end_node = ed->end_node;
			
		}
		if (check) {
			if (doc[i+kernel-1] != 0) {
				result++;
			}
		}
		i++;
	}
	return result;
}

void KSpectrum::similarity(Document &doc, real *dest) {
	if (ex == NULL) {
		for (int i = 0; i < number_of_examples; i++)
			dest[i] = this->similarity(doc, *docs[i]);
	}
	else {
		size_t str_size;
		str_size = doc.size()+1;
		size_t *str = new size_t[str_size];
		doc.read(str,str_size-1);
		str[str_size-1] = 0;
		BuildSuffixTree *tree = new BuildSuffixTree();
		CreateSuffixTree(str,str_size,tree);

		real u = 0;
		real value = CountWithTrees(str,str_size,tree,str);
		for (int i = 0; i < number_of_examples; i++) {
			u = value*CountWithTrees(strings_from_docs[i],strings_from_docs_size[i],ex[i],strings_from_docs[i]);
			if (u == 0) dest[i] = CountWithTrees(str,str_size,ex[i],strings_from_docs[i]);
			else dest[i] = CountWithTrees(str,str_size,ex[i],strings_from_docs[i])/sqrt(u);
		}
		delete [] str; 
		delete tree;
	}
}


void KSpectrum::preload(Document **docs, size_t number) {

	//cout << "Began preloading" << endl;
	number_of_examples = number;
	strings_from_docs = new size_t*[number_of_examples];
	strings_from_docs_size = new size_t[number_of_examples];
	DocsToStrings(strings_from_docs,docs);

	ex = new BuildSuffixTree*[number];

	for (int i = 0; i < number; i++) {
		if ((i % 50) == 0) {
		}
		ex[i] = new BuildSuffixTree();
		CreateSuffixTree(strings_from_docs[i],strings_from_docs_size[i],ex[i]);
	}
	//cout << "Preloading finished" << endl;
}

size_t KSpectrum::preloadedSetSize() {
	return number_of_examples;
}



void KSpectrum::CreateSuffixTree(size_t *doc, size_t size, BuildSuffixTree *tree) {

	int i = 1;
	while ((i / (size)) == 0) i *= 2;
	tree->BuildTree(doc,size,i);
	return;
}

real KSpectrum::similarity(size_t i1, size_t i2) {
	if (cache_size != 0) return GetElementOfMatrix(i1,i2);
	else {
		real u = CountWithTrees(strings_from_docs[i1],strings_from_docs_size[i1],ex[i1],strings_from_docs[i1])*CountWithTrees(strings_from_docs[i2],strings_from_docs_size[i2],ex[i2],strings_from_docs[i2]);
		if (u == 0) return CountWithTrees(strings_from_docs[i1],strings_from_docs_size[i1],ex[i2],strings_from_docs[i2]);
		else return CountWithTrees(strings_from_docs[i1],strings_from_docs_size[i1],ex[i2],strings_from_docs[i2])/sqrt(u);
	}
}

void KSpectrum::similarity(size_t i1, real *dest) {
	for (size_t i = 0; i < number_of_examples; i++)
		dest[i] = similarity(i1,i);
}


//restores all the suffix trees that makes train set from file PreloadedSetFilename
void KSpectrum::readFromBinaryFile(const char *preloadedSetFilename) {

	//cout << "Reading previously saved model" << endl;
	FILE *stream;
	stream = fopen(preloadedSetFilename,"rb");
	if (stream == 0) {
		//cout << "Can't open file!" << endl;
		return;
	}
	//deleting previous information (ex and strings_from_docs)
	clear();

	fread(&kernel,SIZE_OF_T,1,stream);
	fread(&cache_size,SIZE_OF_T,1,stream);
	MakeCache(cache_size);

	int number = 0;
	fread(&number,SIZE_OF_T,1,stream);
	number_of_examples = number;

	//allocate memory for main arrays
	ex = new BuildSuffixTree*[number_of_examples];
	for (int i = 0; i < number_of_examples; i++) {
		ex[i] = new BuildSuffixTree();
	}
	strings_from_docs = new size_t*[number_of_examples];
	strings_from_docs_size = new size_t[number_of_examples];

	//filling main arrays
	int s = 0;
	int size_of_elem_of_save = 0;
	Edge **save_temp;

	for (int i = 0; i < number_of_examples; i++) {
		//filling ex
		fread(&(ex[i]->max_len),SIZE_INT,1,stream);
		ex[i]->hash_t_s = ex[i]->max_len*2.01;
		ex[i]->save = new Edge*[ex[i]->hash_t_s];
		save_temp = ex[i]->save;
		for (int p = 0; p < ex[i]->hash_t_s; p++) save_temp[p] = NULL;

		//filling ex[i]->save
		int j = 0;
		while (true) {
			fread(&j,SIZE_INT,1,stream);
			if (j == ex[i]->hash_t_s) break;
			fread(&size_of_elem_of_save,SIZE_INT,1,stream);
			//filling ex[i]->save[j]
			Edge *p;
			bool check = false;
			save_temp[j] = new Edge;
			for (int l = 0; l < size_of_elem_of_save; l++) {
				if (!check) {
					check = true;
					p = save_temp[j];
				}
				fread(&(save_temp[j]->first_char_index),SIZE_INT,1,stream);
				fread(&(save_temp[j]->last_char_index),SIZE_INT,1,stream);
				fread(&(save_temp[j]->start_node),SIZE_INT,1,stream);
				fread(&(save_temp[j]->end_node),SIZE_INT,1,stream);
				save_temp[j]->next = new Edge;
				save_temp[j] = save_temp[j]->next;
			}
			save_temp[j]->start_node = -1;
			save_temp[j]->next = NULL;
			if (check) save_temp[j] = p;
		}

		//filling strings_from...
		fread(&strings_from_docs_size[i],SIZE_OF_T,1,stream);
		strings_from_docs[i] = new size_t[strings_from_docs_size[i]];
		for (int j = 0; j < strings_from_docs_size[i]-1; j++) {
			fread(&strings_from_docs[i][j],SIZE_OF_T,1,stream);
		}
		strings_from_docs[i][strings_from_docs_size[i]-1] = 0;
	}
	fclose(stream);
	//cout << "Reading finished" << endl;
}


//saves all the suffix trees from train set to file PreloadedSetFilename
void KSpectrum::saveToBinaryFile(const char *preloadedSetFilename) {

	//cout << "Saving model" << endl;
	int size_of_elem_of_save = 0;
	FILE *stream;
	char c = 0;
    stream = fopen(preloadedSetFilename,"wb");
    if (stream == 0) {
		//cout << "Can't open file!" << endl;
		return;
	}

	fwrite(&kernel,SIZE_OF_T,1,stream);

	fwrite(&cache_size,SIZE_OF_T,1,stream);

	fwrite(&number_of_examples,SIZE_OF_T,1,stream);
	//running through all trees
	for (int i = 0; i < number_of_examples; i++) {
		Edge **save_temp = ex[i]->save;
		int size = ex[i]->max_len;
		fwrite(&size,SIZE_INT,1,stream);
		size = ex[i]->hash_t_s;
		//running through all edges of tree
		for (int j = 0; j < size; j++) {
			Edge *pointer = save_temp[j];
			if (pointer == NULL) {
				continue;
			}
			size_of_elem_of_save = 0;
			while (pointer->start_node != -1) {
				pointer = pointer->next;
				size_of_elem_of_save++;
			}
			fwrite(&j,SIZE_INT,1,stream);
			fwrite(&size_of_elem_of_save,SIZE_INT,1,stream);
			pointer = save_temp[j];
			//writing information about single edge
			while (pointer->start_node != -1) {
				fwrite(&(pointer->first_char_index),SIZE_INT,1,stream);
				fwrite(&(pointer->last_char_index),SIZE_INT,1,stream);
				fwrite(&(pointer->start_node),SIZE_INT,1,stream);
				fwrite(&(pointer->end_node),SIZE_INT,1,stream);
				pointer = pointer->next;
			}
		}
		int j = size;
		fwrite(&j,SIZE_INT,1,stream);

		int temp_size = strings_from_docs_size[i];
		fwrite(&temp_size,SIZE_OF_T,1,stream);
		for (int j = 0; j < temp_size-1; j++) {
			fwrite(&(strings_from_docs[i][j]),SIZE_OF_T,1,stream);
		}
	}
	fclose(stream);
	//cout << "Saving finished" << endl;
}


////////////////////////////////////////////////////
///                 class CacheClass			////
////////////////////////////////////////////////////

void KSpectrum::RaisePriority(size_t i) {
	size_t temp;
	temp = cache_priority[i];
	for (int p = 0; p < actual_size; p++) {
		if (cache_priority[p] >= temp) cache_priority[p]--;
	}
	cache_priority[i] = actual_size - 1;
}

size_t KSpectrum::ChangeElementInCache(size_t i) {

	size_t index = 0;
	bool index_here = false;

	//if we can add new row to cache
	if (actual_size < cache_size) {
		index = actual_size;
		cache_priority[index] = actual_size;
		actual_size++;
	}
	//if not
	else {
		for (int p = 0; p < cache_size; p++) {
			if ((!index_here) && (cache_priority[p] == 0)) {
				index = p;
				cache_priority[p] = cache_size;
			}
			cache_priority[p]--;
		}
		index = 0;
	}
	//changing cache_class.cache_dependency
	cache_dependency[index] = i;
	//changing cache
	float u = 0;
	for (int p = 0; p < actual_size; p++) {
		if (index > p) {
			u = CountWithTrees(strings_from_docs[i],strings_from_docs_size[i],ex[i],strings_from_docs[i])*CountWithTrees(strings_from_docs[cache_dependency[p]],strings_from_docs_size[cache_dependency[p]],ex[ cache_dependency[p] ],strings_from_docs[ cache_dependency[p] ]);
			if (u == 0) cache[index][p] = CountWithTrees(strings_from_docs[i],strings_from_docs_size[i],ex[ cache_dependency[p] ],strings_from_docs[ cache_dependency[p] ]);
			else cache[index][p] = CountWithTrees(strings_from_docs[i],strings_from_docs_size[i],ex[ cache_dependency[p] ],strings_from_docs[ cache_dependency[p] ])/sqrt(u);
		}
		else {
			u = CountWithTrees(strings_from_docs[i],strings_from_docs_size[i],ex[i],strings_from_docs[i])*CountWithTrees(strings_from_docs[cache_dependency[p]],strings_from_docs_size[cache_dependency[p]],ex[ cache_dependency[p] ],strings_from_docs[ cache_dependency[p] ]);
			if (u == 0) cache[p][index] = CountWithTrees(strings_from_docs[i],strings_from_docs_size[i],ex[ cache_dependency[p] ],strings_from_docs[ cache_dependency[p] ]);
			else cache[p][index] = CountWithTrees(strings_from_docs[i],strings_from_docs_size[i],ex[ cache_dependency[p] ],strings_from_docs[ cache_dependency[p] ])/sqrt(u);
		}
		//CountWithTrees(cache_class.ex[i], cache_class.ex[cache_class.cache_dependency[p]]);
	}
	return index;
}


int Counter = 0;
int Bounter = 0;
float KSpectrum::GetElementOfMatrix(size_t ii, size_t jj) {
	size_t i = 0, j = 0;
	if (ii < jj) {
		i = jj;
		j = ii;
	}
	else {
		i = ii;
		j = jj;
	}
	size_t index_i = 0;
	size_t index_j = 0;
	bool i_here = false;
	bool j_here = false;

	for (int p = 0; p < actual_size; p++) {
		if (cache_dependency[p] == i) {
			index_i = p;
			i_here = true;
		}
		if (cache_dependency[p] == j) {
			index_j = p;
			j_here = true;
		}
	}
	//if required element exists in cache
	if (i_here && j_here) {

		//необходимо указать, что эти два элемента по приоритету становятся первыми (так как они были запрошены последними)
		RaisePriority(index_i);
		RaisePriority(index_j);

		return cache[index_i][index_j];
	}

	//необходимо указать, что эти два элемента по приоритету становятся первыми (так как они были запрошены последними)
	if (i_here) {
		RaisePriority(index_i);
	}

	if (j_here) {
		RaisePriority(index_j);
	}
	//если элемента с номером i нет в кэше
	if (!i_here) {
		index_i = ChangeElementInCache(i);
	}

	//если элемента с номером j нет в кэше
	if (i == j) index_j = index_i;
	else {
		if (!j_here) {
			index_j = ChangeElementInCache(j);
		}
	}

	if (index_i > index_j) return cache[index_i][index_j];
	else return cache[index_j][index_i];
}




void KSpectrum::clear() {
	if (ex != NULL) {
		for (int i = 0; i < number_of_examples; i++) {
			delete ex[i];
		}
		delete [] ex;
		ex = NULL;
	}
	if (strings_from_docs_size != NULL) {
		delete [] strings_from_docs_size;
		strings_from_docs_size = NULL;
	}
	if (strings_from_docs != NULL) {
		for (int i = 0; i < number_of_examples; i++) {
			delete [] strings_from_docs[i];
		}
		delete [] strings_from_docs;
		strings_from_docs = NULL;
	}

	if (cache != NULL) {
		for (int i = 0; i < cache_size; i++) {
			delete [] cache[i];
		}	
		delete [] cache;
		cache = NULL;

		delete [] cache_dependency;
		cache_dependency = NULL;
		delete [] cache_priority;
		cache_priority = NULL;
	}
}


void KSpectrum::MakeCache(size_t cache_size) {
	this->cache_size = cache_size;
	if (cache_size > 0) {
		this->actual_size = 0;
		cache = new float*[cache_size];
		for (int i = 0; i < cache_size; i++) {
			cache[i] = new float[i+1];
			for (int j = 0; j < i+1; j++)
				cache[i][j] = 0;
		}
		cache_dependency = new size_t[cache_size];
		cache_priority = new size_t[cache_size];
		for (int i = 0; i < cache_size; i++) {
			cache_priority[i] = 0;
			cache_dependency[i] = 0;
		}
	}
	else {
		cache = NULL;
		cache_dependency = NULL;
		cache_priority = NULL;
	}
}
