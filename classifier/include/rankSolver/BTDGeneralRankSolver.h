#ifndef BTDGENERALRANKSOLVER_H_
#define BTDGENERALRANKSOLVER_H_

#include "helper/VectorHelper.h"
#include "rankSolver/AbstractRankSolver.h"

class BTDGeneralRankSolver : public AbstractRankSolver
{
public:
	virtual NumberType* solveRanks(const DecompositionMatrix& D, NumberType*  probs, int* docs_count) ;
	
private:
//TODO
//	find right eps_ps and eps_step, taking into account their
//	dependecy from number of class
	static const NumberType	eps_ps;
	static const NumberType	eps_theta;
	static const NumberType	eps_step;
	static const int		max_iterations;
	
	NumberType			*p;						//	answer
	binLable			**m;					//	matrix ("players x games" in terms of BT)
	
	//	common variables (they`re comforatable to be common)
	int					nr_binary, nr_class;	//	class ~ 'player', binary problem ~ 'team'
	NumberType			*r, *r_sh, *t;
	NumberType			*q;						//	skills of 'teams'
	NumberType			theta_t;
	int					**I_s_p, **I_s_m;		//	for each 'player' array of numbers of his 'teams'
	int					iterations;
	NumberType			*q_p;
	NumberType			*q_m;
	int					small_steps;
//	int					s;
	
	//	cashes
	NumberType			*rt;
	NumberType			*rsht;
	NumberType			sum2ti;
	NumberType			*rt_qtq;
	NumberType			*rsht_qtq;

	void		store_data_localy(const DecompositionMatrix& D) ;

	void 		prepare_place() ;

	void 		prepare_cash() ;

	void		count_teams_skills() ;

	void 		prepare_iteration_cash() ;
	void		find_next_p_t() ;
	void		define_I_plus_minus_sets() ;
	void		normalize_p_t() ;
	void		find_next_theta_t() ;
	NumberType	next_c_t() ;
		
	bool		covergence() ;
	NumberType 	l_likelihood() ;
	NumberType	partial_ps(int s);
	NumberType	partial_theta();

	void 		clear_place() ;

};

#endif /*BTDGENERALRANKSOLVER_H_*/
