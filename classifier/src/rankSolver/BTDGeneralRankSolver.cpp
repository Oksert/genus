#include "rankSolver/BTDGeneralRankSolver.h"
#include "decompositionMatrix/DecompositionMatrix.h"

#include "rankSolver/SimpleTeamRankSolver.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <cstring>

const NumberType	BTDGeneralRankSolver::eps_ps			= 0.0001;
const NumberType	BTDGeneralRankSolver::eps_theta			= 0.0001;
const NumberType	BTDGeneralRankSolver::eps_step			= 0.0005;
const int			BTDGeneralRankSolver::max_iterations	= 1000;

NumberType* BTDGeneralRankSolver::solveRanks(const DecompositionMatrix& D, NumberType* probs, int* docs_count)
{
	store_data_localy(D);
	prepare_place();
	makeBTDModelParameters(nr_binary * 2, probs, docs_count, r, r_sh, t);
	prepare_cash();
	define_I_plus_minus_sets();
	
	SimpleTeamRankSolver simoleRS;
	
	p = simoleRS.solveRanks(D, probs, docs_count);

	theta_t = nr_class;
	count_teams_skills();
	prepare_iteration_cash();

	iterations = 0;
	small_steps = 0;
	do {
		find_next_p_t();
		normalize_p_t();
		count_teams_skills();
		prepare_iteration_cash();
		find_next_theta_t();
		iterations++;
		//TODO
		//	I suppose, we shouldn`t make only one iteration before checking the covergence
	} while ( ! covergence() );
	
	clear_place();
	return p;
}


void BTDGeneralRankSolver::prepare_place()
{
	r	= new NumberType[nr_binary];						//	under victories
	r_sh= new NumberType[nr_binary];
	t	= new NumberType[nr_binary];

	rt	= new NumberType[nr_binary];						//	under cash
	rsht= new NumberType[nr_binary];
	rt_qtq		= new NumberType[nr_binary];						//	under cash
	rsht_qtq	= new NumberType[nr_binary];
	
	q	= new NumberType[nr_binary];						//	under `skills`
	q_p	= new NumberType[nr_binary];
	q_m	= new NumberType[nr_binary];

	I_s_p = new int*[nr_class];								//	under sets
	I_s_m = new int*[nr_class];
	for(int c=0; c < nr_class; ++c)
	{
		I_s_p[c] = new int[nr_binary + 1];
		I_s_m[c] = new int[nr_binary + 1];
	}
}


//	store input data
void BTDGeneralRankSolver::store_data_localy(const DecompositionMatrix& D) {
	nr_binary	= D.getBinaryCount() / 2;
	nr_class	= D.getClassCount();
	m			= new binLable*[nr_binary];
	
	for(int b=0; b < nr_binary; ++b)
	{
		m[b] = new binLable[nr_class];
		
		for(int c=0; c < nr_class; ++c)
			m[b][c] = D[b][c];
	}
}


//	compute most common expressions and store them
void BTDGeneralRankSolver::prepare_cash() {
	int			b;
	
	sum2ti = 0;
		
	//	cash of expressions, which are common for ALL iterations
	for( b=0; b<nr_binary; b++ ){
		rt[b] = r[b] + t[b];
		rsht[b] = r_sh[b] + t[b];
		sum2ti += t[b];
	}
	sum2ti *= 2;
	//TODO
	//	add cash of coeficients of theta-polynom
}


void BTDGeneralRankSolver::count_teams_skills() {
	int		b, c;
	
	memset(q_p, 0, nr_binary * sizeof(NumberType));
	memset(q_m, 0, nr_binary * sizeof(NumberType));
	for( b=0; b<nr_binary; b++){
		for( c=0; c<nr_class; c++){
			if( m[b][c] == ONLY_CLASS )
				q_p[b] += p[c];
			if( m[b][c] == REST_CLASS )
				q_m[b] += p[c];
		}
		q[b] = q_p[b] + q_m[b];
	}
}


void BTDGeneralRankSolver::prepare_iteration_cash(){
	for( int b=0; b < nr_binary; b++){
		rt_qtq[b]	= rt[b] / (q_p[b] + theta_t * q_m[b]);
		rsht_qtq[b]	= rsht[b] / (theta_t * q_p[b] + q_m[b]);
	}
}


void BTDGeneralRankSolver::find_next_p_t() {
	int		s, l, b;
//	NumberType	S_rt_p, S_rsht_p, S_rt_m, S_rsht_m;
	NumberType	t1, t2, b1, b2, b3, b4;
	
	s = iterations % nr_class;
	
	//	to speed up computations of current iteration I have splited
	//	my formula into six subformulas and four common expressions
	//	as shown below:
	//	
	//	                            S_rt_p * ()  +  S_rsht_m * ()
	//	___________________________________________________________________________________ * p_t
	//	S_rt_p * ()  +  S_rsht_p * theta_t * ()  +  S_rt_m * theta_t * ()  +  S_rsht_m * ()
	//	
	//	using this we:
	//	1) won`t compute the same expressions 2 times
	//	2) will use two cycles instead of six
	
	t1 = t2 = b1 = b2 = b3 = b4 = 0;
	
	//	cycle over 'pozitive' set - I_s_p
	l = 0;
	while( -1 != I_s_p[s][l] ){
		//	index
		b = I_s_p[s][l];
		
		//	other parts
		t1			+= rt[b] / q_p[b];
		b1			+= rt_qtq[b];
		b2			+= rsht_qtq[b];
		
		l++;
	}
	
	//	cycle over 'negative' set - I_s_m
	l = 0;
	while( -1 != I_s_m[s][l] ){
		//	index
		b = I_s_m[s][l];

		//	common expressions
//		S_rt_m		+= rt[b];
//		S_rsht_m	+= rsht[b];
		
		//	other parts
		t2			+= rsht[b] / q_m[b];
		b3			+= rt_qtq[b];
		b4			+= rsht_qtq[b];
		
		l++;
	}
	
	NumberType p_old = p[s];
	p[s] *= t1 + t2;
	p[s] /= b1  +  theta_t * b2  +  theta_t * b3  +  b4;
	
//	std::cout << fabs(p[s] - p_old)/(p[s] + p_old) << std::endl;
	
	if( fabs(p[s] - p_old)/(p[s] + p_old) < eps_step )
		small_steps++;
	else
		small_steps = 0;
	
	l = 0;
	while( -1 != I_s_p[s][l] ){
		b = I_s_p[s][l];
		q_p[b] += p[s] - p_old;
		l++;
	}
	
	l = 0;
	while( -1 != I_s_m[s][l] ){
		b = I_s_m[s][l];
		q_m[b] += p[s] - p_old;
		l++;
	}
}


void BTDGeneralRankSolver::define_I_plus_minus_sets() {
	int		b, c;
	int		p_index = 0;
	int		m_index = 0;
	
	//	I_s_p and I_s_m contains for each player numbers of teams,
	//	which player played in and has won or has lost.
	
	//	gather I_s_p and I_s_m
	for( c = 0; c < nr_class; c++ ){
		p_index = 0;
		m_index = 0;
		memset(I_s_p[c], -1, (nr_binary + 1)* sizeof(int));
		memset(I_s_m[c], -1, (nr_binary + 1)* sizeof(int));
		for( b = 0; b < nr_binary; b++ ){
			if( m[b][c] == ONLY_CLASS )
				I_s_p[c][p_index++] = b;
			if( m[b][c] == REST_CLASS )
				I_s_m[c][m_index++] = b;
		}
	}
}


void BTDGeneralRankSolver::normalize_p_t() {
	NumberType	sum		= VectorHelper::componentSum(nr_class, p);
	int			i;
	
	for( i=0; i<nr_class; i++ )
		p[i] /= sum;
	
}


void BTDGeneralRankSolver::find_next_theta_t() {
	NumberType c_t = 1/ (2 * next_c_t());
	theta_t = c_t + sqrt(1 + c_t * c_t);
}


NumberType BTDGeneralRankSolver::next_c_t() {
	NumberType	l_sum		= 0;
	NumberType	r_sum		= 0;
	int		i;
	
	for( i=0; i<nr_binary; i++ ){
		l_sum += rt_qtq[i] * q_m[i];
		r_sum += rsht_qtq[i] * q_p[i];
	}
	
	return (l_sum + r_sum) / sum2ti;
}

//TODO
//	remove fabs()

//TODO
//	add conclusion 1
bool BTDGeneralRankSolver::covergence() {
	if( iterations > max_iterations ){
		std::cout << "iterations = " << max_iterations << std::endl;
		return true;
	}
	
	if (small_steps!=nr_class)
		return false;
	
	std::cout << "iterations=" << iterations << std::endl;
	
	for( int c=(iterations-1) % nr_class, steps = 0; steps<nr_class; c=(c+1)%nr_class, ++steps )
		if( fabs(partial_ps(c)) > eps_ps )
			return false;

/*
	if( fabs( partial_theta() ) > eps_theta )
		return false;
*/
	std::cout << "iterations=" << iterations << std::endl;
	return true;
}


NumberType BTDGeneralRankSolver::l_likelihood(){
	NumberType ll = 0;
	
	for( int b=0; b<nr_binary; b++){
		ll += (r[b] + t[b])		* log( q_p[b] / (q_p[b] + theta_t * q_m[b]));
		ll += (r_sh[b] + t[b])	* log( q_m[b] / (theta_t  * q_p[b] + q_m[b]));
	}
	
	return ll + log(theta_t*theta_t - 1) * VectorHelper::componentSum(nr_binary, t);
}


NumberType BTDGeneralRankSolver::partial_ps(int s){
	int			l, b;
	NumberType	p, p_theta, m, m_theta;
	
	p = p_theta = m = m_theta = 0;
	//	cycle over 'pozitive' set - I_s_p
	l = 0;
	while( -1 != I_s_p[s][l] ){
		//	index
		b = I_s_p[s][l];
		
		//	parts
		p			-= rt[b] / q_p[b];
		p			+= rt_qtq[b];
		p_theta		+= rsht_qtq[b];
		
		l++;
	}
	
	//	cycle over 'negative' set - I_s_m
	l = 0;
	while( -1 != I_s_m[s][l] ){
		//	index
		b = I_s_m[s][l];

		//	other parts
		m			-= rsht[b] / q_m[b];
		m_theta		+= rt_qtq[b];
		m			+= rsht_qtq[b];
		
		l++;
	}
	
	return m + p + theta_t * (m_theta + p_theta);
}

//TODO
//	to think about the first partial to compare with zero
NumberType BTDGeneralRankSolver::partial_theta(){
	NumberType	sum		= 0;
	
	for( int b=0; b<nr_binary; b++ ){
		sum += 2 * theta_t * t[b]	/ (theta_t * theta_t - 1);
		sum -= rt_qtq[b] * q_m[b];
		sum -= rsht_qtq[b] * q_p[b];
	}
	
	return sum;
}


void BTDGeneralRankSolver::clear_place()
{
	delete [] r;
	delete [] r_sh;
	delete [] t;

	delete [] rt;
	delete [] rsht;
	delete [] rt_qtq;
	delete [] rsht_qtq;

	delete [] q;
	delete [] q_p;
	delete [] q_m;

	for(int c=0; c < nr_class; ++c)
	{
		delete [] I_s_p[c];
		delete [] I_s_m[c];
	}
	delete [] I_s_p;
	delete [] I_s_m;

	for(int b=0; b < nr_binary; ++b)
		delete [] m[b];
	
	delete [] m;
}
