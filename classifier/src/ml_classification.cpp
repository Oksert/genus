#include "ml_classification.h"
#include <Context.h>
#include <algorithm> 
#include <vector>
//#include <log4cpp/Category.hh>
#include <cstring>

#include "rankSolver/BTOneVsAllRankSolver.h"
#include "rankSolver/SimpleTeamRankSolver.h"

#define LOG_PATH "Classifier.MLClassification"

///////////////////////////////
#ifndef _NO_NAMESPACE
using namespace std;
#define STD std
#else
#define STD
#endif

int count_docs = 0;
Binary_Model *b_models;
int load_models_flag =0;
int current_context_number=0;
Model *model_load;
float * ALS_load;

void MLClassification::load_models()
{
	if (model_load!=0) delete model_load;
	if  (threshold_method==1) {if (ALS_load!=0) delete [] ALS_load;}
	if (b_models!=0) delete [] b_models;

	string s = Context::getThreadContext()(" ");
	int context_number=0;
	sscanf(&s.c_str()[7],"%d",&context_number);
	cout << "current_context_number = "<<current_context_number<<endl;
	cout << "s = " <<s<<"context_number= "<<context_number<<endl;
	//cout << "current_context_number = "<<current_context_number<<endl;	
	current_context_number = context_number;


	model_load = new Model;
	model_load->read_model("model.txt");
	int num_of_binary_problems = model_load->get_number_of_binary_problems();

	if  (threshold_method==1)
	{
		if (threshold_function==0)
		{
			ALS_load = new float [model_load->get_number_of_topics()+1];
			read_vector (ALS_load, model_load->get_number_of_topics()+1, "LLS.txt");
		}
		else
		{
			ALS_load = new float [model_load->get_number_of_SV()+1];
			read_vector (ALS_load, model_load->get_number_of_SV()+1, "LLS.txt");
		}
	}

	b_models = new Binary_Model [num_of_binary_problems];
	for (int k=0;k<num_of_binary_problems;k++)
	{
		//чтение бинарной модели
		read_b_model (&b_models[k],k);
	}
	load_models_flag =1;
	cout <<"OK";

};

MLClassification::MLClassification()
{
	decomposition_type=1;
	method=SVM;//SVM либо Kern_Perc
    reg_param = 1.0;
	do_Platt_probabilities=1;
	cutting=1;
	threshold=0.5;//0.0099;
	threshold_method=1;//1;
	heuristics=1;
	threshold_function=0;//0 - с результатами ранжирования, 1 - в пространстве признаков

	model = new Model;

	do_cross_validation=0;//1;//1;
	C_start=10;
	C_finish=1000.1;
	step=10;
};

MLClassification::~MLClassification()
{
	//std::cout<<"~MLClassification" <<std::endl;
	//delete [] ranking;
	delete model;
	std::cout<<"~MLClassification" <<std::endl;
};

//-----------------------------------------------------------------

/*********************************************************************/
#define Malloc(type,r) (type *)malloc((r)*sizeof(type))
/**
 * метод Платта для вычисления вероятностного вывода по результатам бинарной классификации
 */
void MLClassification::sigmoid_train(int l, float *dec_values, 
				int *labels, float& A, float& B)
{
	double prior1=0, prior0 = 0;
	int i;

	for (i=0;i<l;i++)
		if (labels[i] > 0) prior1+=1;
		else prior0+=1;
	
	int max_iter=100; 	// максимальное количество итераций
	double min_step=1e-10;	//минимальный шаг в линейном поиске
	double sigma=1e-3;
	double eps=1e-5;
	double hiTarget=(prior1+1.0)/(prior1+2.0);
	double loTarget=1/(prior0+2.0);
	double *t=Malloc(double,l);
	double fApB,p,q,h11,h22,h21,g1,g2,det,dA,dB,gd,stepsize;
	double newA,newB,newf,d1,d2;
	int iter; 
	
	//начальная точка
	A=0.0; B=log((prior0+1.0)/(prior1+1.0));
	//начальное значение функции
	double fval = 0.0;

	for (i=0;i<l;i++)
	{
		if (labels[i]>0) t[i]=hiTarget;
		else t[i]=loTarget;
		fApB = dec_values[i]*A+B;
		if (fApB>=0)
			fval += t[i]*fApB + log(1+exp(-fApB));
		else
			fval += (t[i] - 1)*fApB +log(1+exp(fApB));
	}
	for (iter=0;iter<max_iter;iter++)
	{
		// модифицировать градиент и Hessian (использовать H' = H + sigma I)
		h11=sigma;
		h22=sigma;
		h21=0.0;g1=0.0;g2=0.0;
		for (i=0;i<l;i++)
		{
			fApB = dec_values[i]*A+B;
			if (fApB >= 0)
			{
				p=exp(-fApB)/(1.0+exp(-fApB));
				q=1.0/(1.0+exp(-fApB));
			}
			else
			{
				p=1.0/(1.0+exp(fApB));
				q=exp(fApB)/(1.0+exp(fApB));
			}
			d2=p*q;
			h11+=dec_values[i]*dec_values[i]*d2;
			h22+=d2;
			h21+=dec_values[i]*d2;
			d1=t[i]-p;
			g1+=dec_values[i]*d1;
			g2+=d1;
		}

		//критерий завершения
		if (fabs(g1)<eps && fabs(g2)<eps)
			break;

		// нахождение направления для метода Ньютона: -inv(H') * g
		det=h11*h22-h21*h21;
		dA=-(h22*g1 - h21 * g2) / det;
		dB=-(-h21*g1+ h11 * g2) / det;
		gd=g1*dA+g2*dB;


		stepsize = 1; 		// линейный поиск
		while (stepsize >= min_step)
		{
			newA = A + stepsize * dA;
			newB = B + stepsize * dB;

			//новое значение функции
			newf = 0.0;
			for (i=0;i<l;i++)
			{
				fApB = dec_values[i]*newA+newB;
				if (fApB >= 0)
					newf += t[i]*fApB + log(1+exp(-fApB));
				else
					newf += (t[i] - 1)*fApB +log(1+exp(fApB));
			}
			//проверка завершения процесса
			if (newf<fval+0.0001*stepsize*gd)
			{
				A=newA;B=newB;fval=newf;
				break;
			}
			else
				stepsize = stepsize / 2.0;
		}

		if (stepsize < min_step)
		{
			//cout<<"Line search fails in two-class probability estimates"<<endl;
			break;
		}
	}

	if (iter>=max_iter)
		//cout <<"Reaching maximal iterations in two-class probability estimates"<<endl;
	free(t);
}
//--------------------------------------------------------------------------------------
float MLClassification::sigmoid_predict(float decision_value, float A, float B)
{
	double fApB = decision_value*A+B;
	if (fApB >= 0)
		return exp(-fApB)/(1.0+exp(-fApB));
	else
		return 1.0/(1+exp(fApB));
}

//--------------------------------------------------------------------------------------

void MLClassification::read_b_model(Binary_Model *b_model, int k)
{
	char ch,num[10];
	char *f_model;
	
	sprintf(num,"%5.5d",k);
	string s,s1;
	s1.erase();
	for (int i=0; i<5; i++)
	{
		ch=num[i];
		s1=s1+ch;
	}
	s="bmodel"+s1+".txt";
	f_model=(char *) s.c_str();

	b_model->read(f_model);
}

void MLClassification::save_b_model(Binary_Model *b_model, int k)
{
	char ch,num[10];
	char *f_model;
	
	sprintf(num,"%5.5d",k);
	string s,s1;
	s1.erase();
	for (int i=0; i<5; i++)
	{
		ch=num[i];
		s1=s1+ch;
	}
	s="bmodel"+s1+".txt";
	f_model=(char *) s.c_str();

	b_model->save(f_model);
}

/*********************************************************************/
float MLClassification::HammingLoss (int *rtv, int *predict, int num_of_tops)
{
	float HammingLoss=0;
	int card_union=0, card_intersect=0;

	for (int i=0; i<num_of_tops; i++)
	{
		if (rtv[i]==1) card_union++;
		if (predict[i]==1) card_union++;
		if ((rtv[i]==1)&&(predict[i]==1)) card_intersect++;
	}	
	HammingLoss=card_union-2*card_intersect;
	HammingLoss=HammingLoss/num_of_tops;
	//log4cpp::Category::getInstance("classifier.MLClassification").infoStream() << "HammingLoss=" << HammingLoss << log4cpp::CategoryStream::ENDLINE;
	return HammingLoss;
}

int MLClassification::OneError(int *rtv, int* R, int num_of_tops)
{
	int One_Err=0, i=0;

	while ((i<num_of_tops)&&(One_Err==0))
	{
		if (R[i]==0)
		{
			if (rtv[i]==0) One_Err=1;
		}
		i++;
	}	
	//log4cpp::Category::getInstance("classifier.MLClassification").infoStream() << "One_Error=" << One_Err << log4cpp::CategoryStream::ENDLINE;
	return One_Err;
}

int MLClassification::Coverage(int *rtv, int* R, int num_of_tops)
{
	int max=0;

	for (int i=0; i<num_of_tops; i++)
	{
		if (rtv[i]==1) 
		{
			if (R[i]>max) max=R[i];
		}
	}	
	//log4cpp::Category::getInstance("classifier.MLClassification").infoStream() << "Coverage=" << max << log4cpp::CategoryStream::ENDLINE;
	return max;
}

float MLClassification::AveragePrecision(int *rtv, int* R, int num_of_tops)
{
	float avgp=0, sum;
	int num_relev_tops=0;
	
	for (int i=0; i<num_of_tops; i++)
	{
		if (rtv[i]==1) 
		{
			num_relev_tops++;
			sum=0;
			for (int j=0; j<num_of_tops; j++)
			{
				if (rtv[j]==1)
				{
					if (R[j]<=R[i]) sum++;
				}
			}
			avgp=avgp+sum/(R[i]+1);
		}
	}
	avgp=(float)avgp/num_relev_tops;
	//log4cpp::Category::getInstance("classifier.MLClassification").infoStream() << "AveragePrecision=" << avgp << log4cpp::CategoryStream::ENDLINE;
	return avgp;
}

float MLClassification::RankingLoss(int *rtv, int* R, int num_of_tops)
{
	int **E;
	E = new int* [num_of_tops];
	for(int r=0; r<num_of_tops; r++)
	{
		E[r] = new int [num_of_tops];
		for(int s=0; s<num_of_tops; s++) E[r][s]=0;
	}

	int num_relev_tops=0;
		
	for (int r=0; r<num_of_tops; r++)
	{
		if (rtv[r]==1) 
		{
			num_relev_tops++;
			for (int s=0; s<num_of_tops; s++)
			{
				if (rtv[s]==0)
				{
					if (R[r]>=R[s]) E[r][s]=1;
				}
			}
		}
	}

	//мощность набора ошибок 
	int ErrSize=0;
	float RankingLoss;
	for(int r=0; r<num_of_tops; r++)
	{
		for(int s=0; s<num_of_tops; s++) 
			if(E[r][s]==1) ErrSize++;
	}

	//нормализация
	if ((num_relev_tops!=0)&&(num_relev_tops!=num_of_tops))
	{
		RankingLoss=(float)ErrSize/(num_relev_tops*(num_of_tops-num_relev_tops));
	}
	else
	{
		RankingLoss=ErrSize;
	}

	for (int r=0; r<num_of_tops; r++)
	{
		delete [] E[r];
	}
	delete [] E;

	//log4cpp::Category::getInstance("classifier.MLClassification").infoStream() << "RankingLoss=" << RankingLoss << log4cpp::CategoryStream::ENDLINE;
	return RankingLoss;
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void MLClassification::decomposition(int num_of_tops, topics *tops, CModule::Measure *measure)
{
	int num_of_train_docs=measure->getPreloadedSetSize();
	model->put_number_of_topics(num_of_tops);
	model->put_multiclass_decomposition_type(decomposition_type);
	model->put_number_of_SV(num_of_train_docs);
	model->create_decomposition_matrix();
	model->save_model("model.txt");
	model->show();

	Classified_Document *docs;
	Document_Set *DocSet = new Document_Set;
	DocSet->put_number_of_documents(num_of_train_docs);
	docs=new Classified_Document [num_of_train_docs];

	for (int j=0; j<num_of_train_docs; j++) 
	{
		docs[j].put_number_of_topics(tops[j].number);
		docs[j].put_topics(tops[j].theames);
	}
	DocSet->put_documents(docs);

	Decomposition decomp;
	decomp.put_learning_docs(DocSet);
	decomp.put_number_of_topics(model->get_number_of_topics());
	decomp.put_number_of_binary_problems(model->get_number_of_binary_problems());
	decomp.put_multiclass_decomposition_type(model->get_multiclass_decomposition_type());
	decomp.put_decomposition_matrix(model->get_decomposition_matrix());
		
	//cout <<"Check decompose()"<<endl;
		
	decomp.decompose();
	decomp.save_decomposition();
	//decomp.show();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void MLClassification::calc_number_of_SV(int num_of_tops, topics *tops, CModule::Measure *measure)
{
	Decomposition decomp;
	
	int num_of_train_docs=measure->getPreloadedSetSize();
	//model->put_number_of_topics(num_of_tops);
	//model->put_multiclass_decomposition_type(decomposition_type);
	//model->put_number_of_SV(num_of_train_docs);
	//model->create_decomposition_matrix();
	//model->save_model("model.txt");
	//model->show();
	model->read_model("model.txt");

	Classified_Document *docs;
	Document_Set *DocSet = new Document_Set;
	DocSet->put_number_of_documents(num_of_train_docs);
	docs=new Classified_Document [num_of_train_docs];

	for (int j=0; j<num_of_train_docs; j++) 
	{
		docs[j].put_number_of_topics(tops[j].number);
		docs[j].put_topics(tops[j].theames);
	}
	DocSet->put_documents(docs);

	//Decomposition decomp;
	decomp.put_learning_docs(DocSet);

	decomp.put_number_of_topics(model->get_number_of_topics());
	decomp.put_number_of_binary_problems(model->get_number_of_binary_problems());
	decomp.put_multiclass_decomposition_type(model->get_multiclass_decomposition_type());
	decomp.put_decomposition_matrix(model->get_decomposition_matrix());
	decomp.read_decomposition();

	decomp.delete_zero();
	cout << "Number of SV="<<decomp.get_number_of_SV()<<endl;
	PyLogger::info("",LOG_PATH) << "Number of SV="<<decomp.get_number_of_SV();
	//log4cpp::Category::getInstance("classifier.MLClassification").infoStream() <<"Number of SV="<<decomp.get_number_of_SV()<<log4cpp::CategoryStream::ENDLINE;
	//cout <<"CHECK"<<endl;
	//decomp.save_decomposition();
	//model->put_number_of_SV(decomp.get_number_of_SV());
	//model->save_model("model.txt");
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MLClassification::train (int number_of_topics, 
				topics *tops, CModule::Measure *measure)
{
	
	decomposition(number_of_topics, tops, measure);

	model->read_model("model.txt");
	int num_of_tops = model->get_number_of_topics();
	int num_of_binary_problems = model->get_number_of_binary_problems();
	int number_of_SV=model->get_number_of_SV();

	int do_cash_measure =1;
	cash_measure =new float* [number_of_SV];
	for (int i=0; i<number_of_SV; i++)
			cash_measure[i] = new float [number_of_SV];
	//"кеширование" меры сходства
	if (do_cash_measure==1)
	{
		for (int i=0; i<number_of_SV;i++)
			for (int j=0; j<number_of_SV;j++)
				cash_measure[i][j]=measure->similarity(i,j);
	}

	float *c_reg;//параметры кросс-валидации
	c_reg = new float [num_of_binary_problems];
	float A, B;//параметры метода Платта 
	
	Binary_Model *b_model;		
	b_model = new Binary_Model;

	if (method==Kern_Perc) 
	{
		Kernel_Perceptron kern_perc;
		//цикл по бинарным подпроблемам 
		for (int k=0;k<num_of_binary_problems;k++)
		{
			//log4cpp::Category::getInstance("classifier.MLClassification").infoStream() <<"NUMBER_OF_BINARY_PROBLEM ="<<k<<log4cpp::CategoryStream::ENDLINE;
			std::cout <<"NUMBER_OF_BINARY_PROBLEM ="<<k<<endl;
			//чтение k-ой бинарной модели 
			read_b_model (b_model,k);
			int num_of_SV=b_model->get_number_of_SV();

			if (do_cross_validation==1)
			{
				float c, C1=C_start; //параметр регуляризации
				double err, error_min=1.1;
				int error;
				c=C_start;
				//кросс-валидация
				while (c<=C_finish)
				{
					//log4cpp::Category::getInstance("classifier.MLClassification").infoStream() <<"lambda= "<<c<<"******************" << log4cpp::CategoryStream::ENDLINE;
					/////////////////////////////////////
					//обучение на части документов 
					kern_perc.put_lambda(c);
					//if (c>=100) kern_perc.put_eta(1/(2*c));
					kern_perc.put_T(num_of_SV);
					kern_perc.put_num_of_train_docs(num_of_SV);
					kern_perc.put_binary_model(b_model);
					kern_perc.put_do_cash_measure(do_cash_measure);
					if (do_cash_measure==1)
					{
						kern_perc.put_cash_measure(cash_measure);
					}

					kern_perc.train(measure);

					if (do_cash_measure==1)


					///////////////////////////////////////////
					//оценка ошибки на другой части документов    
					b_model=kern_perc.get_binary_model();
					kern_perc.put_binary_model(b_model);
					kern_perc.put_num_of_test_docs(num_of_SV);				
					kern_perc.put_platt(1);
					kern_perc.predict_platt(measure);
					kern_perc.put_platt(0);

					error=0;
					for (int i=0;i<num_of_SV; i++)
					{
						//cout<<kern_perc.get_target_predict()[i]<<" "<<b_model->get_labels()[i]<<endl;
						if (kern_perc.get_target_predict()[i]!=b_model->get_labels()[i])
							error++;
					}
					err=(double) error/num_of_SV;
					cout <<"error = "<<err<<endl;

					/////////////////////////////////////////////////////
					//выбираем С при котором получается минимальная ошибка    
					if (err < error_min) 
					{
						error_min=err;
						C1=c;
					}
					c=step*c;
				}
				c_reg[k]=C1;
				//log4cpp::Category::getInstance("classifier.MLClassification").infoStream() <<"C_optimal= "<<c_reg[k]<<"***************"<<log4cpp::CategoryStream::ENDLINE;				
			}
			/////////////////////////////////////////////////
			//обучение модели с наилучшим С   
			if (do_cross_validation==1)
				kern_perc.put_lambda(c_reg[k]);
			else
				kern_perc.put_lambda(0.0);
			
			kern_perc.put_num_of_train_docs(num_of_SV);

			kern_perc.put_binary_model(b_model);
			kern_perc.put_T(num_of_SV);

			kern_perc.put_do_cash_measure(do_cash_measure);
			if (do_cash_measure==1)
			{
				kern_perc.put_cash_measure(cash_measure);
			}
			kern_perc.train(measure);

			//log4cpp::Category::getInstance("classifier.MLClassification").infoStream()<<"training end"<<log4cpp::CategoryStream::ENDLINE;

			Binary_Model *b_model1;
			b_model1=kern_perc.get_binary_model();

			if (do_Platt_probabilities==1)
			{
				//log4cpp::Category::getInstance("classifier.MLClassification").infoStream() <<"Platt probs estimation"<<log4cpp::CategoryStream::ENDLINE;
				//оценка параметров А и В для оценки вероятностей (метод Платта)
				kern_perc.put_num_of_test_docs(b_model1->get_number_of_SV());

				kern_perc.put_platt(1);
				kern_perc.predict_platt(measure);
				kern_perc.put_platt(0);

				//log4cpp::Category::getInstance("classifier.MLClassification").infoStream()<<"Sigmoid train"<<log4cpp::CategoryStream::ENDLINE;
				sigmoid_train(b_model1->get_number_of_SV(), kern_perc.get_f_predict(),
						b_model1->get_labels(), A, B);
				b_model1->put_A(A);
				b_model1->put_B(B);
				//log4cpp::Category::getInstance("classifier.MLClassification").infoStream() <<"A= "<<A<<" B= "<<B<<log4cpp::CategoryStream::ENDLINE;
			}
			//сохранение просчитанной бинарной модели 
			save_b_model (b_model1,k);
		}
	}
	if (method==SVM)
	{
		SMO smo;
		//цикл по бинарным подпроблемам
		for (int k=0;k<num_of_binary_problems;k++)
		{
			//log4cpp::Category::getInstance("classifier.MLClassification").infoStream() <<"NUMBER_OF_BINARY_PROBLEM ="<<k<<log4cpp::CategoryStream::ENDLINE;
			std::cout <<"NUMBER_OF_BINARY_PROBLEM ="<<k<<endl;
			//чтение k-ой бинарной модели
			read_b_model (b_model,k);
			int num_of_SV=b_model->get_number_of_SV();

			if (do_cross_validation==1)
			{
				float c, C1=C_start; //параметр регуляризации
				double err, error_min=1.1;
				int error;
				c=C_start;
				//кросс-валидация
				while (c<=C_finish)
				{
					//log4cpp::Category::getInstance("classifier.MLClassification").infoStream() <<"c= "<<c<<"******************" << log4cpp::CategoryStream::ENDLINE;
					/////////////////////////////////////
					//обучение на части документов  
					smo.put_C(c);
					smo.put_binary_model(b_model);
					smo.put_do_cash_measure(do_cash_measure);
					if (do_cash_measure==1)
					{
						smo.put_cash_measure(cash_measure);
					}
					smo.smo_train(measure);


					///////////////////////////////////////////
					//оценка ошибки на другой части документов  
					b_model=smo.get_binary_model();
					smo.put_binary_model(b_model);
					smo.put_number_of_test_docs(num_of_SV);
					smo.put_platt(1);
					smo.smo_predict_platt(measure);
					smo.put_platt(0);

					error=0;
					for (int i=0;i<num_of_SV; i++)
					{
						//cout<<smo.get_target_predict()[i]<<" "<<b_model->get_labels()[i]<<endl;
						if (smo.get_target_predict()[i]!=b_model->get_labels()[i])
							error++;
					}
					err= (double) error/num_of_SV;
					//log4cpp::Category::getInstance("classifier.MLClassification").infoStream() <<"error = "<<err<<log4cpp::CategoryStream::ENDLINE;

					/////////////////////////////////////////////////////
					//выбираем С при котором получается минимальная ошибка     
					if (err < error_min) 
					{
						error_min=err;
						C1=c;
					}
					//c=step+c;
					c=step*c;
				}
				c_reg[k]=C1;
				//log4cpp::Category::getInstance("classifier.MLClassification").infoStream() <<"C_optimal= "<<c_reg[k]<<"***************"<<log4cpp::CategoryStream::ENDLINE;
				
			}
			/////////////////////////////////////////////////
			//обучение модели с наилучшим С    
			if (do_cross_validation==1)
				smo.put_C(c_reg[k]);
			else
				smo.put_C(1.0);
			//reg_param = smo.get_C();

			smo.put_binary_model(b_model);
			smo.put_do_cash_measure(do_cash_measure);
			if (do_cash_measure==1)
			{
				smo.put_cash_measure(cash_measure);
			}
			smo.smo_train(measure);
			//log4cpp::Category::getInstance("classifier.MLClassification").infoStream()<<"training end"<<log4cpp::CategoryStream::ENDLINE;

			Binary_Model *b_model1;
			b_model1=smo.get_binary_model();

			if (do_Platt_probabilities==1)
			{
				//log4cpp::Category::getInstance("classifier.MLClassification").infoStream() <<"Platt probs estimation"<<log4cpp::CategoryStream::ENDLINE;
				//оценка параметров А и В для оценки вероятностей (метод Платта)
				smo.put_number_of_test_docs(b_model1->get_number_of_SV());
				
				smo.put_platt(1);
				smo.put_do_cash_measure(do_cash_measure);
				smo.smo_predict_platt(measure);

				smo.put_platt(0);
	
				//log4cpp::Category::getInstance("classifier.MLClassification").infoStream() <<"Sigmoid train"<<log4cpp::CategoryStream::ENDLINE;
				sigmoid_train(b_model1->get_number_of_SV(), smo.get_f_predict(),
						b_model1->get_labels(), A, B);
				b_model1->put_A(A);
				b_model1->put_B(B);
				//log4cpp::Category::getInstance("classifier.MLClassification").infoStream() <<"A= "<<A<<" B= "<<B<<log4cpp::CategoryStream::ENDLINE;
			}
			//сохранение просчитанной бинарной модели   
			save_b_model (b_model1,k);
		}
	}
	delete [] c_reg;


	std::cout<<"train_threshold start" <<std::endl;

//struct timespec startTime11; 
//clock_gettime(CLOCK_REALTIME, &startTime11);

	load_models();
	string s = Context::getThreadContext()(" ");
	int context_number=0;
	sscanf(&s.c_str()[7],"%d",&context_number);
	cout << "s = " <<s<<"context_number= "<<context_number<<endl;
	current_context_number = context_number;
	load_models_flag =1;

//struct timespec endTime11; 
//clock_gettime(CLOCK_REALTIME, &endTime11);
//std::cout<<"LoadBinaryModels=" << endTime11.tv_sec-startTime11.tv_sec<<" "<<endTime11.tv_nsec-startTime11.tv_nsec<<std::endl;

//struct timespec startTime1; 
//clock_gettime(CLOCK_REALTIME, &startTime1); 	
	calc_number_of_SV(number_of_topics, tops, measure);
//struct timespec endTime1; 
//clock_gettime(CLOCK_REALTIME, &endTime1);
//std::cout<<"calc_number_of_SVTime=" << endTime1.tv_sec-startTime1.tv_sec<<" "<<endTime1.tv_nsec-startTime1.tv_nsec<<std::endl;

//getchar();

//struct timespec startTime; 
//clock_gettime(CLOCK_REALTIME, &startTime); 
	if  (threshold_method==1)
		train_threshold (number_of_topics, number_of_SV, tops, measure);
//struct timespec endTime; 
//clock_gettime(CLOCK_REALTIME, &endTime);
//std::cout<<"TrainThreshodTime=" << endTime.tv_sec-startTime.tv_sec<<" "<<endTime.tv_nsec-startTime.tv_nsec<<std::endl;

	std::cout<<"train_threshold end" <<std::endl;


	delete b_model;

	for(int i=0; i<number_of_SV; i++)
		delete [] cash_measure[i];
	delete [] cash_measure;
}

void MLClassification::save_vector (float *v, int dim, char *file)
{
	FILE *f;

	Context context = Context::getThreadContext();
	f=fopen(context(file).c_str(),"w");
	if (!f)
	{
		//log4cpp::Category::getInstance("classifier.MLClassification").error("file not found");
		return;
	}
	
	fprintf(f, "\nCoefficients_For_Threshold_Function\n");
	for (int i=0; i<dim;i++) 
	{
		fprintf(f, "%.16g ",v[i]);
	}
	fprintf(f, "\n");

	fclose(f);
}

void MLClassification::read_vector (float *v, int dim, char *file)
{
	FILE *f;
	int intch;
	
	Context context = Context::getThreadContext();
	f=fopen(context(file).c_str(),"r");
	if (!f)
	{
		//log4cpp::Category::getInstance("classifier.MLClassification").error("file not found");
		return;
	}
	
	intch= getc(f);intch= getc(f);
	while ((intch!= -1)&&(intch!=' ')&&(intch!='\n')) intch= getc(f);

	for (int i=0; i<dim;i++) 
	{
		fscanf(f,"%f",&v[i]);
		//cout << v[i] <<" ";
	}
	//cout <<endl;
	
	fclose(f);
}

void MLClassification::train_threshold (int number_of_topics, int number_of_SV, topics *tops, CModule::Measure *measure)
{
	//log4cpp::Category::getInstance("classifier.MLClassification").info("training threshold ....");
	double *ALS1;

	//делать кеширование меры сходства
	int do_cash_measure =1;

	if (threshold_function==0)
	{
		ALS = new float [number_of_topics+1];
		ALS1 = new double [number_of_topics+1];
	}
	else
	{
		ALS = new float [number_of_SV+1];
		ALS1 = new double [number_of_SV+1];
	}

	double *TRSH = new double [number_of_SV];

	double** X =new double* [number_of_SV];
	for (int i=0; i<number_of_SV; i++)
		if (threshold_function==0)
			X[i] = new double [number_of_topics];
		else
			X[i] = new double [number_of_SV];
		
	double thresh=0.0;

	float *similarity = new float[number_of_SV];
	int *ranks = new int [number_of_topics];
	float *weights = new float [number_of_topics];
	topics *predicted_tops = new topics;
	float *rank = new float [number_of_topics];

	flag=0;

	for (int i=0; i<number_of_SV; i++)
	{
		//cout <<"number of document "<<i<<endl;
		//double lenght=0;

		for (int j=0; j<number_of_SV; j++)
		{
			if (do_cash_measure = 0)
				similarity[j]=measure->similarity(j,i);
			else
				similarity[j]=cash_measure[j][i];
			//lenght+=similarity[j]*similarity[j];
		}

if (threshold_function==0)
{
		threshold_method=0;

//struct timespec startTime1; 
//clock_gettime(CLOCK_REALTIME, &startTime1);

		predict(similarity, predicted_tops, ranks, weights);

//struct timespec endTime1; 
//clock_gettime(CLOCK_REALTIME, &endTime1);
//std::cout<<"PredictTimeTrainTresh=" << endTime1.tv_sec-startTime1.tv_sec<<" "<<endTime1.tv_nsec-startTime1.tv_nsec<<std::endl;
		
		threshold_method=1;
		
		float summa = 0;
		for (int j=0; j<number_of_topics; j++)
		{
			rank[j]=ranking[j];
			summa = summa + rank[j];
		}

		for (int j=0; j<number_of_topics; j++)
		{
			rank[j]=rank[j]/summa;
		}
}
		if (threshold_function==0)
			for (int j=0; j<number_of_topics; j++)
				X[i][j]=rank[j];
		else
			for (int j=0; j<number_of_SV; j++)
				X[i][j]=similarity[j];// /sqrt(lenght)
	
		//for (int j=0; j<number_of_topics; j++)
		//	cout <<ranks[j]<<" ";
		//cout<<endl;
		//for (int j=0; j<number_of_topics; j++)
		//	cout <<weights[j]<<" ";
		//cout<<endl;
		//for (int j=0; j<number_of_topics; j++)
		//{
		//	cout <<ranking[j]<<" "; 
		//}
		//cout<<endl;
		//int *target = new int[number_of_topics];
		//for (int j=0; j<number_of_topics; j++)
		//	target[j]=0;
		//for (int j=0; j<tops[i].number; j++)
		//	target[tops[i].theames[j]]=1;
		//for (int j=0; j<tops[i].number; j++)
		//	cout<<tops[i].theames[j]<<" ";
		//cout<<endl;
		//for (int j=0; j<number_of_topics; j++)
		//	cout <<target[j]<<" "; 
		//cout<<endl;


		//нахождение порогового значения
		//то есть нахождение t, минизирующего threshold_error
		double t, tmin;
		double threshold_err, threshold_error_tmin;
		tmin=(weights[0]+weights[1])/2;
		threshold_error_tmin=threshold_error(tmin,number_of_topics,tops[i],rank);
		//cout <<"t ="<<tmin<<" "<<threshold_error_tmin<<endl;
		for (int j=1; j<number_of_topics-1; j++)
		{
			t=(weights[j]+weights[j+1])/2;
			threshold_err=threshold_error(t,number_of_topics,tops[i],rank);
			//cout <<"t ="<<t<<" "<<threshold_err<<endl;
			if (threshold_err<threshold_error_tmin)//<=
			{
				tmin=t;
				threshold_error_tmin=threshold_err;
			}
		}
		thresh=thresh+tmin;
		TRSH[i]=tmin;
		//cout <<endl <<"tmin= "<<tmin<<endl;
	}
	threshold=(double) thresh/number_of_SV;
	cout <<endl <<"threshold= "<<threshold<<endl;

	if (threshold_function==0)
	{
		least_squares(X , TRSH/*, ALS1*/, number_of_topics);
//		for (int i=0; i<number_of_topics+1; i++) 
//			ALS[i] = ALS1[i];
		save_vector (ALS, number_of_topics+1, "LLS.txt");
	}
	else
	{
		least_squares(X , TRSH/*, ALS1*/, number_of_SV);
//		for (int i=0; i<number_of_SV+1; i++) 
//			ALS[i] = ALS1[i];
		save_vector (ALS, number_of_SV+1, "LLS.txt");
	}
	
	//read_vector (ALS, number_of_topics+1, "LLS.txt");
	//for (int i=0; i<number_of_topics+1; i++) 
	//{
	//	cout<< ALS[i] <<" ";
	//}
	//cout <<endl;

	
	//для тестирования пороговой функции на обучающих документах    
	/*double tmin;
	cout <<endl<<endl;
	for (int i=0; i<number_of_SV; i++)
	{
		tmin=0;
		for (int j=0; j<number_of_topics; j++)//number_of_topics
		{
			tmin = tmin + X[i][j]*ALS[j];
		}
		tmin = tmin+ALS[number_of_topics];
		cout <<i <<" "<<tmin<<endl;
	}*/
	

	delete [] similarity;
	delete [] ranks;
	delete [] weights;
	delete [] predicted_tops;
	delete [] rank;
	for (int i=0; i<number_of_SV; i++)
		delete [] X[i];
	delete [] X;
	delete TRSH;

}

int MLClassification::threshold_error(double t, int num_of_tops, topics tops, float *rank)
{
	int *target = new int [num_of_tops];
	for (int i=0; i<num_of_tops; i++)
		target[i]=0;
	for (int i=0; i<tops.number; i++)
		target[tops.theames[i]]=1;

	//for (int j=0; j<num_of_tops; j++)
	//	cout <<rank[j]<<" "; 
	//cout<<endl;
	//for (int j=0; j<num_of_tops; j++)
	//	cout <<target[j]<<" "; 
	//cout<<endl;

	int result=0;		
	for (int i=0; i<num_of_tops; i++)
	{
		if (target[i]==0)
		{
			if (rank[i]>t) result++;
		}
		if (target[i]==1) 
		{
			if (rank[i]<=t) result++;
		}
	}
	return result;
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------


void MLClassification::predict(float *similarity, topics *tops, int *ranks, float *weights)
{
	count_docs ++;
	//std::cout <<" count_docs = "<< count_docs << " "<<endl;

	num_of_test_docs=1;

	
	string s = Context::getThreadContext()(" ");
	int context_number=0;
	sscanf(&s.c_str()[7],"%d",&context_number);
	//cout << "current_context_number = "<<current_context_number<<endl;
	//cout << "s = " <<s<<"context_number= "<<context_number<<endl;
	//cout << "current_context_number = "<<current_context_number<<endl;
	
	if (current_context_number != context_number)
		load_models();

	Model *model;
	model = model_load;

	//model = new Model;
	//model->read_model("model.txt");

	int num_of_binary_problems = model->get_number_of_binary_problems();
	int num_of_tops = model->get_number_of_topics();
	int number_of_SV = model->get_number_of_SV();
	//PyLogger::info("",LOG_PATH) << "Number of topics "<<num_of_tops;

	MLC_result = new int* [num_of_binary_problems];
	MLC_ranking_f = new float* [num_of_binary_problems];
	MLC_ranking = new float* [num_of_binary_problems];
	num_of_SV_in_b_models = new int [num_of_binary_problems];

	for(int i=0; i<num_of_binary_problems; i++)
	{
		MLC_result[i] = new int [num_of_test_docs];
		MLC_ranking_f[i] = new float [num_of_test_docs];
		MLC_ranking[i] = new float [num_of_test_docs];
	}
	//PyLogger::info("",LOG_PATH) << "Number of topics2 "<<num_of_tops;

	MLC_ranks = new int* [num_of_tops];
	for(int i=0; i<num_of_tops; i++)
	{
		//PyLogger::info("",LOG_PATH) <<"similarity["<<i<<"]" << ranking[i] << " ";
		//cout <<"similarity["<<i<<"]" << ranking[i] << " "<<endl;
		MLC_ranks[i] = new int [num_of_test_docs];
	}

	//PyLogger::info("",LOG_PATH) << "Number of topics3 "<<num_of_tops;




	float *A,*B;//параметры метода Платта 
	A = new float [num_of_binary_problems];
	B = new float [num_of_binary_problems];
	
/*
struct timespec startTime11; 
clock_gettime(CLOCK_REALTIME, &startTime11);

	Binary_Model *b_models = new Binary_Model [num_of_binary_problems];

	for (int k=0;k<num_of_binary_problems;k++)
	{
			//чтение бинарной модели
			read_b_model (&b_models[k],k);
	}

struct timespec endTime11; 
clock_gettime(CLOCK_REALTIME, &endTime11);
//std::cout<<"LoadBinaryModels=" << endTime11.tv_sec-startTime11.tv_sec<<" "<<endTime11.tv_nsec-startTime11.tv_nsec<<std::endl;
*/
//struct timespec startTime10; 
//clock_gettime(CLOCK_REALTIME, &startTime10);

	if (method==Kern_Perc)
	{
		Kernel_Perceptron kern_perc;
		//цикл по бинарным подпроблемам   
		for (int k=0;k<num_of_binary_problems;k++)
		{
			/////////////////////////////////////////////
			//тестирование на новых тестовых документах     
			//cout <<endl<< "TESTING............."<<k<<endl;
			
			//cout <<"Kernel Perceptron PREDICT "<<endl;
			//PyLogger::info("",LOG_PATH) << "Kernel Perceptron PREDICT..."<<k;
			//Binary_Model *b_model;		
			//b_model = new Binary_Model;
			//чтение бинарной модели
			//read_b_model (b_model,k);
			num_of_SV_in_b_models[k]=(&b_models[k])->get_number_of_SV();
			kern_perc.put_binary_model(&b_models[k]);
			A[k]=(&b_models[k])->get_A();
			B[k]=(&b_models[k])->get_B();

			//тестирование
			kern_perc.put_T((&b_models[k])->get_number_of_SV());
			kern_perc.put_num_of_test_docs(num_of_test_docs);

			kern_perc.put_similarity(similarity);
			kern_perc.predict();

			//сохранение результатов
			for(int j=0; j<num_of_test_docs; j++)
			{
				MLC_result[k][j]=kern_perc.get_target_predict()[j];
				MLC_ranking_f[k][j]=kern_perc.get_f_predict()[j];
				if (do_Platt_probabilities==1)
					MLC_ranking[k][j]= sigmoid_predict
						(kern_perc.get_f_predict()[j],A[k],B[k]);
				else
					MLC_ranking[k][j]=MLC_ranking_f[k][j];
			}
			//delete b_model;
		}
	}

	if (method==SVM)
	{
		SMO smo;
		//цикл по бинарным подпроблемам  
		for (int k=0;k<num_of_binary_problems;k++)
		{
			/////////////////////////////////////////////
			//тестирование на новых тестовых документах   
			//cout <<endl<< "TESTING............."<<k<<endl;
			
			//cout <<"SVM PREDICT "<<endl;  
			//PyLogger::info("",LOG_PATH) << "SVM PREDICT ..."<<k;
			//Binary_Model *b_model;		
			//b_model = new Binary_Model;

			//чтение бинарной модели
			//read_b_model(b_model,k);
			num_of_SV_in_b_models[k]=(&b_models[k])->get_number_of_SV();
			smo.put_binary_model(&b_models[k]);
			A[k]=(&b_models[k])->get_A();
			B[k]=(&b_models[k])->get_B();
			
			//тестирование
			smo.put_number_of_test_docs(num_of_test_docs);

			smo.put_similarity(similarity);
//struct timespec startTime13; 
//clock_gettime(CLOCK_REALTIME, &startTime13); 	
			smo.smo_predict();
//struct timespec endTime13; 
//clock_gettime(CLOCK_REALTIME, &endTime13);
//std::cout<<"smo_predictTime=" << endTime13.tv_sec-startTime13.tv_sec<<" "<<endTime13.tv_nsec-startTime13.tv_nsec<<std::endl;


			//сохранение результатов
			for(int j=0; j<num_of_test_docs; j++)
			{
				MLC_result[k][j]=smo.get_target_predict()[j];
				MLC_ranking_f[k][j]=smo.get_f_predict()[j];
				if (do_Platt_probabilities==1)
				{
//struct timespec startTime12; 
//clock_gettime(CLOCK_REALTIME, &startTime12); 	
						MLC_ranking[k][j]= sigmoid_predict (smo.get_f_predict()[j],A[k],B[k]);
//struct timespec endTime12; 
//clock_gettime(CLOCK_REALTIME, &endTime12);
//std::cout<<"sigmoid_predictTime=" << endTime12.tv_sec-startTime12.tv_sec<<" "<<endTime12.tv_nsec-startTime12.tv_nsec<<std::endl;

				}
				else
					MLC_ranking[k][j]=MLC_ranking_f[k][j];
			}
			//delete b_model;
		}
	}

		
//struct timespec endTime10; 
//clock_gettime(CLOCK_REALTIME, &endTime10);
//std::cout<<"PredictTimeBinaryProblems=" << endTime10.tv_sec-startTime10.tv_sec<<" "<<endTime10.tv_nsec-startTime10.tv_nsec<<std::endl;	
	/////////////////////////////////////////////////////////
	int *predict;
	predict = new int [num_of_tops];
	ranking = new float [num_of_tops];

	int *R; 
	int *Rout;

	/*if  (threshold_method==1)
	{
		if (threshold_function==0)
		{
			ALS = new float [num_of_tops+1];
			read_vector (ALS, num_of_tops+1, "LLS.txt");
		}
		else
		{
			ALS = new float [number_of_SV+1];
			read_vector (ALS, number_of_SV+1, "LLS.txt");
		}
	}*/
	ALS = ALS_load;

	for (int j=0; j<num_of_test_docs; j++)
	{
		//cout <<endl<<"Num= "<<j+1<<endl;
		
		if (model->get_multiclass_decomposition_type()==ONE_ALL)
		{
			float summa = 0;
			//ранжирование
			for (int i=0; i<num_of_tops; i++)
			{
				predict[i]=MLC_result[i][j];
				ranking[i]=MLC_ranking[i][j];
				//PyLogger::info("",LOG_PATH) <<"ranking_["<<i<<"]" << ranking[i] << " ";
				//cout <<"ranking_["<<i<<"]" << ranking[i] << " "<<endl;
				summa = summa + ranking[i];
			}

			for (int i=0; i<num_of_tops; i++)
			{
				ranking[i]=ranking[i]/summa;
			}
			
			
			/*BTOneVsAllRankSolver strs;
			float* p = new float[num_of_tops];
			
			for(int i=0; i<num_of_tops; ++i)
				p[i] = MLC_ranking[i][j];
			
			float* r = strs.solveRanks(model->get_decomposition_matrix(), num_of_tops, num_of_tops, p, 0);
			
			for (int i=0; i<num_of_tops; i++)
			{
				predict[i] = MLC_result[i][j];
				ranking[i] = r[i];
			}
			
			delete [] p;
			delete [] r;*/

			if  (threshold_method==1)
			{
				//вычисление порога
				threshold=0;
				if (threshold_function==0)
				{
					for (int i=0; i<num_of_tops; i++)
						threshold = threshold + ranking[i]*ALS[i];
					threshold = threshold + ALS[num_of_tops];
				}
				else
				{
					for (int i=0; i<number_of_SV; i++)
						threshold = threshold + similarity[i]*ALS[i];
					threshold = threshold + ALS[number_of_SV];
				}
				//cout <<"threshold = "<<threshold<<endl;
				if ((flag==1)||(threshold<=0)||(threshold>1)) 
				{
					flag=1;
					threshold=0.5;
				}
			}

			//предсказание релевантных тем  
			for (int i=0; i<num_of_tops; i++)
			{
				//if (MLC_ranking_f[i][j]>=0) predict[i]=1; else predict[i]=-1;
				if (ranking[i]>=threshold) predict[i]=1; else predict[i]=-1;
				////cout <<MLC_ranking_f[i][j]<<endl;
			}
		}
		else if (model->get_multiclass_decomposition_type()==DENSE || model->get_multiclass_decomposition_type()==SPARSE)
		{
			/*DecompositionMatrix DM(model->get_decomposition_matrix(), num_of_binary_problems, num_of_tops);
			
			float* ranks;
			float* probs_b = new float[num_of_binary_problems];
			
			SimpleTeamRankSolver strs;
			
			for (int b=0; b < num_of_binary_problems; b++)
				probs_b[b] = MLC_ranking[b][j];
			
			ranks = strs.solveRanks(DM, probs_b, 0);*/
			
			//cout << "im in predict for sparse or dense" << endl;
			
			for (int c=0; c<num_of_tops; c++)
			{
				ranking[c] = 1/ float(num_of_tops);
			}
			
			/*delete [] ranks;
			delete [] probs_b;*/
			
			if  (threshold_method==1)
			{
				//вычисление порога//
				threshold=0;
				if (threshold_function==0)
				{
					for (int i=0; i<num_of_tops; i++)
						threshold = threshold + ranking[i]*ALS[i];
					threshold = threshold + ALS[num_of_tops];
				}
				else
				{
					for (int i=0; i<number_of_SV; i++)
						threshold = threshold + similarity[i]*ALS[i];
					threshold = threshold + ALS[number_of_SV];
				}
				if ((flag==1)||(threshold<0)||(threshold>1)) 
				{
					flag=1;
					threshold=0.5;
				}
			}

			//предсказание релевантных тем  
			for (int i=0; i<num_of_tops; i++)
			{
				if (ranking[i]>=threshold) predict[i]=1; else predict[i]=-1;
				MLC_ranking[i][j]=ranking[i];
				MLC_result[i][j]=predict[i];
			}
		}
		else
		{//ONE_ONE
			//вычисление ранжирования
			predict_probs(j,ranking,A,B,model);
			float summa = 0;
			for (int i=0; i<num_of_tops; i++)
			{
				summa = summa + ranking[i];
			}
			for (int i=0; i<num_of_tops; i++)
			{
				ranking[i]=ranking[i]/summa;
			}
			
			if  (threshold_method==1)
			{
				//вычисление порога//
				threshold=0;
				if (threshold_function==0)
				{
					for (int i=0; i<num_of_tops; i++)
						threshold = threshold + ranking[i]*ALS[i];
					threshold = threshold + ALS[num_of_tops];
				}
				else
				{
					for (int i=0; i<number_of_SV; i++)
						threshold = threshold + similarity[i]*ALS[i];
					threshold = threshold + ALS[number_of_SV];
				}
				if ((flag==1)||(threshold<0)||(threshold>1)) 
				{
					flag=1;
					threshold=0.5;
				}
			}

			
			//threshold=0.166312;//для каждый-против-каждого
			//threshold=0.16;

			//предсказание релевантных тем  
			for (int i=0; i<num_of_tops; i++)
			{
				if (ranking[i]>=threshold) predict[i]=1; else predict[i]=-1;
				MLC_ranking[i][j]=ranking[i];
				MLC_result[i][j]=predict[i];
			}
		}

		//вычисление "рангов тем" R=(rank(x,1),...,rank(x,k))
		R = new int [num_of_tops];
		int r;
		for (int i=0; i<num_of_tops; i++)
		{
			//PyLogger::info("",LOG_PATH) <<"ranking["<<i<<"]" << ranking[i] << " ";
			//cout <<"ranking["<<i<<"]" << ranking[i] << " "<<endl;
			r=0;
			for (int s=0; s<num_of_tops; s++)
			{
				if (ranking[s]>ranking[i]) r++;
				if ((ranking[s]==ranking[i])&&(s<i)) r++;
				//для одинаковых значений ранжирования   
			}
			R[i]=r;
			//PyLogger::info("",LOG_PATH) <<"R_["<<i<<"]" << R[i] << " ";
			//cout <<"R_["<<i<<"]" << R[i] << " "<<endl;
		}

		//вычисление ранжирования - порядка релевантности тем   
		Rout = new int [num_of_tops];
		for (int i=0; i<num_of_tops; i++)
		{
			r=R[i];
			Rout[r]=i;
			MLC_ranks[r][j]=i;
		}
		
		for (int i=0; i<num_of_tops; i++)
		{
			ranks[i]=MLC_ranks[i][j];
			//PyLogger::info("",LOG_PATH) <<"ranks["<<i<<"]" << ranks[i] << " ";
			//cout <<"ranks["<<i<<"]" << ranks[i] << " "<<endl;
			weights[i]=ranking[ranks[i]];
		}

		int count=0;
		for (int i=0; i<num_of_tops; i++)
		{
			if (predict[i]==1) count++;
			////cout <<ranking[i]<<endl;
			//cout <<MLC_ranking_f[i][j]<<endl;
		}

		//cout <<count<<endl;

		//tops->number=1;
		//tops->theames = new int [1];
		//tops->theames[0]=ranks[0];

		tops->number=count;
		if (count>0)
		{
			tops->theames = new int [count];
			for (int i=0; i<count; i++)
				tops->theames[i]=ranks[i];
		}
		else
		{
			if (heuristics==1)
			{
				if ((weights[0]/weights[1])>1.5)
				{
					tops->number=1;
					tops->theames = new int [1];
					tops->theames[0]=ranks[0];
				}
				else
				{
					if ((weights[1]/weights[2])>5.0)
					{
						tops->number=1;
						tops->theames = new int [tops->number];
						for (int i=0; i<tops->number; i++)
							tops->theames[i]=ranks[i];
					}
				}
			}
		}
	}

	delete [] predict;
	delete [] R;
	delete [] Rout;
	for (int k=0; k<num_of_binary_problems; k++)
	{
		delete [] MLC_ranking_f[k];
		delete [] MLC_result[k];
		delete [] MLC_ranking[k];
	}

	delete [] MLC_ranking_f;
	delete [] MLC_result;
	delete [] MLC_ranking;

	for(int i=0; i<num_of_tops; i++)
		delete [] MLC_ranks[i];
	delete [] MLC_ranks;

	delete [] num_of_SV_in_b_models;
	delete [] A;
	delete [] B;
	//cout << "classification process end"<< endl;
	//PyLogger::info("",LOG_PATH) << "Classification process end...";

	//delete model;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#include <map>
/**
 * предсказание вероятностей классов для случая каждый-против-каждого
 */
void MLClassification::predict_probs(int test_j, float* rank, float *A, float *B, Model *model)
{
	int nr_class = model->get_number_of_topics();
	int nr_binary = model->get_number_of_binary_problems();


	float * r = Malloc(float, nr_class*nr_class);//вероятности r_ji
	float * n = Malloc(float, nr_class*nr_class);
	float * rt = Malloc(float, nr_class*nr_class);
	//результирующие вероятности
	float * pt = Malloc(float, nr_class);
	float * pt0 = Malloc(float, nr_class);
		
	//calc r(i,j) and init pt
	for(int i=0;i<nr_class;i++)	{
		pt[i] = 0.5;
		pt0[i] = 0;
	}

	if ((model->get_multiclass_decomposition_type()== 0)) 
	{
		//декомпозиция ONE_ONE 
		int **I = model->get_decomposition_matrix();
		for(int j=0;j<nr_binary;j++) 
		{
			int i=0, k=0;
			for(i=0; I[j][i]==0; i++);
			for(k=i+1; I[j][k]==0; k++);
			int nn = num_of_SV_in_b_models[j];
			n[(I[j][i]>I[j][k])?(i*nr_class+k):(k*nr_class+i)] = num_of_SV_in_b_models[j];
			float rr = MLC_ranking[j][test_j];
			r[(I[j][i]>I[j][k])?(i*nr_class+k):(k*nr_class+i)] = rr;
		}
		for(int i=0;i<nr_class;i++) {
			for(int j=0;j<nr_class;j++) {
				if (i==j) continue;
				//mask[i*nr_class+j] = mask [j*nr_class+i] = ((r[j*nr_class+i]<0.5)&&(r[i*nr_class+j]<0.5))?0:1;
				if ((r[j*nr_class+i]<0.5)&&(r[i*nr_class+j]<0.5)) {
					r[j*nr_class+i]=r[i*nr_class+j] = 0.5;
				}
			}
		}
	}
	float* score = Malloc(float, nr_class);
	int sm = 0;
	for(int i=0;i<nr_class;i++) {
		score[i]=0;
		for(int j=0;j<nr_class;j++) {
			if ((i==j)) continue;
			if (r[i*nr_class+j]>=0.5) score[i]+=1; //считаем голоса кажого из классов
			sm++;
		}
	}
/*
	std::vector<float> scores;
	for(int i=0;i<nr_class;i++) 
		scores.push_back(score[i]);
	sort(scores.begin(), scores.end());

	//for(int i=0;i<nr_class;i++) 
		//cout << scores[i]<<" ";
	//cout << endl<<"nr_class = "<<nr_class<<endl;
	float score_treshold;

	std::vector<float>::iterator it1 = scores.begin();
	if (nr_class>=10) 
	{
		//it1 = it1+nr_class-10;
		it1 = it1+9;
		//it1 = it1+3;
		score_treshold=*it1;
	}
	else score_treshold = 0;

	//cout << endl<<"score_treshold = "<<score_treshold<<endl;
*/
	std::map<float, int> sscores;

	double avg_score = 0;

	for(int i=0;i<nr_class;i++) {
		score[i]/=sm;
		sscores[score[i]]=i;
		avg_score += score[i];//сумма счетов (где r>=0.5) по всем классам 
	}

	avg_score /= nr_class;

	std::map<float, int>::iterator it = sscores.begin();
	for (int j = 1; j<sscores.size()*0.85;j++){
		avg_score = it->first;
		it++;
	}

	//!!!
	avg_score = 10e+10;
	float dv;

	float C = 10;//(nr_class-1);
	int n_max = 1;
	for(int j=0;j<nr_binary;j++) 
		if (n_max<num_of_SV_in_b_models[j]) n_max=num_of_SV_in_b_models[j];

	//cutting=0;
	//C*=n_max;
	// начальные значения 
	float phi = 1;
	float Ck = 0;
	do {

		dv = 0;
		//calc rt(i,j)
		float S = 0;
		float tcnt = 0;
		Ck = 0;
		for(int i=0;i<nr_class;i++) {
			if (cutting==1)
				//if (score[i]<=score_treshold) continue;/***/
				if (score[i]<avg_score) continue;/***/
			for(int j=0;j<nr_class;j++) {
				if ((i==j)) continue;
				if (cutting==1)
					//if (score[j]<=score_treshold) continue;/***/
					if (score[j]<avg_score) continue;/***/
				Ck+= 2*r[i*nr_class+j]*pt[j]/(pt[i]+phi*pt[j]);
				if ((r[i*nr_class+j]>=0.5)&&(r[j*nr_class+i]>=0.5)) tcnt++;
			}
		}

		//calc next p	
		Ck/=tcnt;
		phi = 1/(2*Ck)+sqrt(1+1/(4*Ck*Ck));


		float C0 =0;
		for(int i=0;i<nr_class;i++) {
			if (cutting==1)
				//if (score[i]<=score_treshold) continue;/***/
				if (score[i]<avg_score) continue;/***/
			float S = 0;
			float Su =0;
			float Sd = 0;
			for(int j=0;j<nr_class;j++) {
				if (cutting==1)
					//if (score[j]<=score_treshold) continue;/***/
					if (score[j]<avg_score) continue;/***/
				if ((i==j)) continue;
				//My
				//S += rt[i*nr_class+j];
				//Rao
				Su+=r[i*nr_class+j];
				Sd+=r[i*nr_class+j]/(pt[i]+phi*pt[j])+phi*r[j*nr_class+i]/(phi*pt[i]+pt[j]);
				//BT
				//S+=n[i*nr_class+j]*(r[i*nr_class+j]*pt[j]-r[j*nr_class+i]*pt[i])/(pt[i]*(pt[i]+pt[j]));
				//KL
				//sm+=(r[i*nr_class+j]-r[j*nr_class+i])*pt[j]*n[i*nr_class+j]/(pt[i]+pt[j]);
				//S+=(r[i*nr_class+j]*pt[j]-r[j*nr_class+i]*pt[i])/(pt[i]*(pt[i]+pt[j]));
				//S+=(r[i*nr_class+j]-r[j*nr_class+i])*pt[j]/((pt[i]+pt[j]));
			}
			//S/=(nr_class-1);//*(nr_class);
			//float np = exp(S/C)/(1+exp(S/C));
			float np = Su/Sd;
			//float ent = log(pt[i]/(1-pt[i]));
			//float np = pt[i]+0.001*S;
			//C0+=ent/S;
			if (np>1) np = 1; 
			if (np<0) np = 0;
			dv+=fabs(np-pt[i]);
			pt0[i] = np;
			dv/=nr_class;//было закоммментировано
		}
		memcpy(pt,pt0,sizeof(float)*nr_class);	

	} while (dv>0.01);//while (dv>0.00001);

	//float tr = 0.5;

	for (int i=0; i<nr_class;i++) {
		//rank[i] = pt[i];
		rank[i] = pt[i]+score[i];
	}

	free(r);
	free(rt);
	free(n);
	free(pt);
	free(pt0);
	free(score);
}

/**
 * поиск минимума целевой функции одного аргумента
 * методом деления шага пополам
 */
double MLClassification::findMinDihotomy(double a,//левая граница 
		       double b,            //правая граница интервала
		       double sigma, double* T, int dim, double **X)        //погрешность

{
   double l = b-a;                                 //длина интервала
   double hm = (b+a)*0.5;                          //середина интервала
   double h1 = a+l*0.25, h2 = b-l*0.25;            //пробные точки
   double f_h1 = TF(h1,T,dim,X), f_h2 = TF(h2,T,dim,X),
	   f_hm = TF(hm,T,dim, X);//значения функции

   while(l>sigma)
   {
      if(f_h1<f_hm)
      {
		 //исключен правый подынтервал
		 b = hm; hm = h1;
		 f_hm = f_h1;
	  }
	  else
	  {
		 if(f_h2<f_hm)
		 {
			//исключен левый подынтервал
			a = hm; hm = h2;
			f_hm = f_h2;
		 }
		 else
		 {
			//исключены левый и правый подынтервалы
			a = h1; b = h2;
		 }
	  }
      l = b-a;
      h1 = a+l*0.25; h2 = b-l*0.25;
      f_h1 = TF(h1,T, dim, X); f_h2 = TF(h2,T,dim, X);
   }
   return f_hm;
};

/**
 * вычисление направление наискорейшего спуска
 */
void MLClassification::GRADIENT(double** X, double* T, int dim)
{
	double SUM;
	int number_of_SV=model->get_number_of_SV();

	for (int i=0; i< number_of_SV; i++)
	{
		//cout << "NUMBER OF CURRENT GRAD_DOCUMENT = " << i << endl;
		SUM=0;
		for (int j=0; j<dim; j++)
		{
			SUM=SUM+X[i][j]*ALS[j];
			//cout <<X[i][j]<<" "<<ALS[j]<<" ";
		}
		//cout << "SUM=" << SUM << endl;
		SUM=SUM+ALS[dim];
		//cout << "SUM=" << SUM << endl;
		SUM=2*(T[i]-SUM);
		//cout << "SUM=" << SUM << endl;
		//getchar();
		for (int j=0; j<dim; j++)
			GRAD[j]=GRAD[j]+SUM*(-X[i][j]);
		GRAD[dim]=GRAD[dim]-SUM;
	}

	double absgrad=0;
	SUM=0;
	for (int i=0; i<=dim; i++)
	{
		SUM=SUM+GRAD[i]*GRAD[i];
	}
	absgrad=sqrt(SUM);

	for (int i=0; i<=dim; i++)
	{
		if(absgrad!=0) GRAD[i]=GRAD[i]/absgrad;
	}
};

/**
 * вычисление целевой функции как функции одного аргумента h
 */
double MLClassification::TF (double h, double* T, int dim, double** X)
{
	int number_of_SV=model->get_number_of_SV();
	double TFUNC=0;
	double SUM;
	
	for ( int i=0; i<=dim; i++)
	{
		DA[i]=h*GRAD[i];
		ALS[i]=ALSN[i]+DA[i];
	}

	for (int i=0; i<number_of_SV; i++)
	{
		//cout << "NUMBER OF CURRENT TFUNC_DOCUMENT = " << i << endl;
		SUM=0;
		for (int j=0; j<dim; j++)
			SUM=SUM+X[i][j]*ALS[j];
		SUM=SUM+ALS[dim];
		SUM=(T[i]-SUM)*(T[i]-SUM);

		//cout << "SUMTF=" << SUM << endl;
		TFUNC=TFUNC+SUM;
	}

	//cout << "TFUNC=" << TFUNC << endl;
	return TFUNC;
};

/**
 * линейный метод наименьших квадратов
 */
void MLClassification::least_squares(double** X , double * T, /*, double* ALS,*/ int dim)
{
	ALSN = new double [dim+1];
	DA = new double [dim+1];
	GRAD = new double [dim+1];
	for (int i=0; i<=dim;i++)
	{
		ALS[i]=0; ALSN[i]=0; DA[i]; GRAD[i]=0;
	}
	NDA=1;
	//while (NDA>0.0001)
	while (NDA>0.0001)
	{
		for (int  i=0; i<=dim; i++)
		{
			ALSN[i]=ALS[i];
		}

		GRADIENT(X,T,dim);

		double f_hm;
		//f_hm=findMinDihotomy(-100,100,0.0001,T,dim,X);
		f_hm=findMinDihotomy(-100,100,0.0001,T,dim,X);
		//for (int  i=0; i<=dim; i++)
		//{
		//	cout << ALS[i] <<" ";
		//}
		NDA=0;
		for (int i=0; i<=dim; i++)
		{
			if (NDA < fabs(DA[i])) NDA=fabs(DA[i]) ;
		}
	}
	//for (int  i=0; i<=dim; i++)
	//{
	//	cout << ALS[i] <<" ";
	//}
	//getchar();
}


//################################
//---------------------------------------------------------------------------

void MLClassification::incrementive_learning (int number_of_topics, int number_of_docs,
				topics *tops, CModule::Measure *measure)
{
	decomposition_inc(number_of_topics, tops, measure);

	int do_Platt_probs;
	do_Platt_probs = do_Platt_probabilities;

	if (method==SVM) 
	{
		do_Platt_probs = 0; 
		//do_Platt_probabilities = 0;
	}

	std::cout<<"method" <<method<<std::endl;
	std::cout<<"type" <<model->get_multiclass_decomposition_type()<<"   "<< decomposition_type<<std::endl;
 
	model->read_model("model.txt");
	int num_of_tops = model->get_number_of_topics();
	int num_of_binary_problems = model->get_number_of_binary_problems();
	int number_of_SV=model->get_number_of_SV();

	int do_cash_measure =1;
	cash_measure =new float* [number_of_SV];
	for (int i=0; i<number_of_SV; i++)
			cash_measure[i] = new float [number_of_SV];
	//"кеширование" меры сходства
	if (do_cash_measure==1)
	{
		for (int i=0; i<number_of_SV;i++)
			for (int j=0; j<number_of_SV;j++)
				cash_measure[i][j]=measure->similarity(i,j);
	}



	float *c_reg;//параметры кросс-валидации
	c_reg = new float [num_of_binary_problems];
	float A, B;//параметры метода Платта
	
	Binary_Model *b_model;		
	b_model = new Binary_Model;

	method==Kern_Perc;

	Kernel_Perceptron kern_perc;
	//цикл по бинарным подпроблемам
	for (int k=0;k<num_of_binary_problems;k++)
	{
		std::cout <<"NUMBER_OF_BINARY_PROBLEM ="<<k<<endl;
		//чтение k-ой бинарной модели
		read_b_model (b_model,k);
		int num_of_SV=b_model->get_number_of_SV();

		if ((num_of_SV > 0)&&(b_model->get_state()!=0))
		{
			kern_perc.put_lambda(1/reg_param);
				
			kern_perc.put_num_of_train_docs(num_of_SV);

			kern_perc.put_binary_model(b_model);
			kern_perc.put_T(num_of_SV);

			kern_perc.put_do_cash_measure(do_cash_measure);
			if (do_cash_measure==1)
			{
				kern_perc.put_cash_measure(cash_measure);
			}
			kern_perc.incrementive_learning(measure, number_of_docs);
			std::cout<<"incrementive learning end"<<std::endl;

			Binary_Model *b_model1;
			b_model1=kern_perc.get_binary_model();

			if (do_Platt_probs==1)
			{
				cout <<"Platt probs estimation"<<endl;
				//оценка параметров А и В для оценки вероятностей (метод Платта)
				kern_perc.put_num_of_test_docs(b_model1->get_number_of_SV());

				kern_perc.put_platt(1);
				//kern_perc.put_do_cash_measure (0);
				kern_perc.predict_platt(measure);
				kern_perc.put_platt(0);

				cout<<"Sigmoid train"<<endl;
				sigmoid_train(b_model1->get_number_of_SV(), kern_perc.get_f_predict(),
						b_model1->get_labels(), A, B);
				b_model1->put_A(A);
				b_model1->put_B(B);
				cout <<endl<<"A= "<<A<<" B= "<<B<<endl;
				cout <<endl<<endl;
			}
			//сохранение просчитанной бинарной модели
			std::cout<<"save_b_model start " << k <<std::endl;
			save_b_model (b_model1,k);
			std::cout<<"save_b_model end " << k <<std::endl;
		}
	}

	std::cout<<"delete  c_reg" <<std::endl;
	delete [] c_reg;
	std::cout<<"delete  b_model" <<std::endl;
	delete b_model;

	calc_number_of_SV(number_of_topics, tops, measure);

//struct timespec startTime11; 
//clock_gettime(CLOCK_REALTIME, &startTime11);

	load_models();
	string s = Context::getThreadContext()(" ");
	int context_number=0;
	sscanf(&s.c_str()[7],"%d",&context_number);
	cout << "s = " <<s<<"context_number= "<<context_number<<endl;
	current_context_number = context_number;
	load_models_flag =1;

//struct timespec endTime11; 
//clock_gettime(CLOCK_REALTIME, &endTime11);
//std::cout<<"LoadBinaryModels=" << endTime11.tv_sec-startTime11.tv_sec<<" "<<endTime11.tv_nsec-startTime11.tv_nsec<<std::endl;


	std::cout<<"train_threshold start" <<std::endl;
	if  (threshold_method==1)
		train_threshold (number_of_topics, number_of_SV, tops, measure);
	std::cout<<"train_threshold end" <<std::endl;


	for(int i=0; i<number_of_SV; i++)
		delete [] cash_measure[i];
	delete [] cash_measure;
}
///////////////////////////////////////////////////////////////////////////
void MLClassification::delete_topic(int topic_number, int number_of_topics, 
				int number_of_docs, topics *tops, CModule::Measure *measure)
{
	Model model_old;
	model_old.read_model("model.txt");
	int num_of_train_docs=measure->getPreloadedSetSize();
	model->put_number_of_topics(number_of_topics);
	model->put_multiclass_decomposition_type(decomposition_type);
	model->put_number_of_SV(num_of_train_docs);
	model->create_decomposition_matrix();
	//this->put_model(model);
	model->save_model("model.txt");
	//model->show();

	int do_cash_measure =1;
	cash_measure =new float* [num_of_train_docs];
	for (int i=0; i<num_of_train_docs; i++)
			cash_measure[i] = new float [num_of_train_docs];
	//"кеширование" меры сходства
	if (do_cash_measure==1)
	{
		for (int i=0; i<num_of_train_docs;i++)
			for (int j=0; j<num_of_train_docs;j++)
				cash_measure[i][j]=measure->similarity(i,j);
	}

	int countOfDeletedBinaryModels = 0;
	for (int i=0; i<model_old.get_number_of_binary_problems(); i++)
	{
		if (((model->get_multiclass_decomposition_type()==0)&&(model_old.get_decomposition_matrix()[i][topic_number] == 0))||
			((model->get_multiclass_decomposition_type()==1)&&(model_old.get_decomposition_matrix()[i][topic_number] == -1)))
		{
			if (countOfDeletedBinaryModels > 0)
			{
				rename_b_model(i,i-countOfDeletedBinaryModels);
				//remove_b_model(i);
			}
		}
		else
		{
			remove_b_model(i);
		}
	}

	if  (threshold_method==1)
		train_threshold (number_of_topics, num_of_train_docs, tops, measure);

	for(int i=0; i<num_of_train_docs; i++)
		delete [] cash_measure[i];
	delete [] cash_measure;
}

///////////////////////////////////////////////////////////////////////////////////
void MLClassification::decomposition_inc(int num_of_tops, topics *tops, CModule::Measure *measure)
{
	Model model_old;
	model_old.read_model("model.txt");
	int num_of_train_docs=measure->getPreloadedSetSize();
	model->put_number_of_topics(num_of_tops);
	model->put_multiclass_decomposition_type(decomposition_type);
	model->put_number_of_SV(num_of_train_docs);
	model->create_decomposition_matrix();
	///model->show();
	//this->put_model(model);
	model->save_model("model.txt");
	//model.show();

	Classified_Document *docs;
	Document_Set *DocSet = new Document_Set;
	DocSet->put_number_of_documents(num_of_train_docs);
	docs=new Classified_Document [num_of_train_docs];

	for (int j=0; j<num_of_train_docs; j++) 
	{
		docs[j].put_number_of_topics(tops[j].number);
		docs[j].put_topics(tops[j].theames);
	}
	DocSet->put_documents(docs);

	Decomposition decomp;

	decomp.put_learning_docs(DocSet);
	decomp.put_number_of_topics(model->get_number_of_topics());
	decomp.put_number_of_binary_problems(model->get_number_of_binary_problems());
	decomp.put_multiclass_decomposition_type(model->get_multiclass_decomposition_type());
	decomp.put_decomposition_matrix(model->get_decomposition_matrix());
		
	cout <<"Check decompose()"<<endl;
		
	decomp.decompose();
	//decomp.save_decomposition();
	//getchar();
	//decomp.show();

	cout <<"Check decompose1()"<<endl;

	Binary_Model *b_model;		
	b_model = new Binary_Model;
	int countOfBinaryProblems = 0;

	Binary_Model *binary_models_inc = new Binary_Model [model->get_number_of_binary_problems()];
	if (model_old.get_number_of_topics() < model->get_number_of_topics())
	{
		for(int i=0; i<model->get_number_of_binary_problems();i++)
		{
			//дообучение с добавлением новой темы
			if (((model->get_multiclass_decomposition_type()==0)&&(model->get_decomposition_matrix()[i][num_of_tops-1]==0))||
				((model->get_multiclass_decomposition_type()==1)&&(model->get_decomposition_matrix()[i][num_of_tops-1]==-1)))
			{
				//сохраняем данные от предыдущих обучений
				read_b_model (b_model,countOfBinaryProblems);
				binary_models_inc[i].put_number_of_SV(decomp.get_binary_models()[i].get_number_of_SV());
				binary_models_inc[i].put_labels(decomp.get_binary_models()[i].get_labels());
				binary_models_inc[i].put_numbers(decomp.get_binary_models()[i].get_numbers());
				binary_models_inc[i].put_state(decomp.get_binary_models()[i].get_number_of_SV()-b_model->get_number_of_SV());
				binary_models_inc[i].put_b(b_model->get_b());
				binary_models_inc[i].put_A(b_model->get_A());
				binary_models_inc[i].put_B(b_model->get_B());
				float *coefficients = new float [decomp.get_binary_models()[i].get_number_of_SV()];
				for (int j=0; j<b_model->get_number_of_SV(); j++)
					coefficients[j]=b_model->get_coefficients()[j];
				for(int j=b_model->get_number_of_SV(); j<decomp.get_binary_models()[i].get_number_of_SV();j++)
					coefficients[j]=-1;
				binary_models_inc[i].put_coefficients(coefficients);

				countOfBinaryProblems++;
			}
			else
			{
				//инициализация новых бинарных моделей для новой темы
				binary_models_inc[i].put_number_of_SV(decomp.get_binary_models()[i].get_number_of_SV());
				binary_models_inc[i].put_labels(decomp.get_binary_models()[i].get_labels());
				binary_models_inc[i].put_numbers(decomp.get_binary_models()[i].get_numbers());
				binary_models_inc[i].put_state(decomp.get_binary_models()[i].get_number_of_SV());
				binary_models_inc[i].put_b(0);
				binary_models_inc[i].put_A(0);
				binary_models_inc[i].put_B(0);
				float *coefficients = new float [decomp.get_binary_models()[i].get_number_of_SV()];
				for(int j=0; j<decomp.get_binary_models()[i].get_number_of_SV();j++)
					coefficients[j]=-1;
				binary_models_inc[i].put_coefficients(coefficients);
			}
		}
	}
	else
	{
		//дообучение без добавления новых тем
		for(int i=0; i<model->get_number_of_binary_problems();i++)
		{
			//сохраняем данные от предыдущих обучений
			read_b_model (b_model,i);
			binary_models_inc[i].put_number_of_SV(decomp.get_binary_models()[i].get_number_of_SV());
			binary_models_inc[i].put_labels(decomp.get_binary_models()[i].get_labels());
			binary_models_inc[i].put_numbers(decomp.get_binary_models()[i].get_numbers());
			binary_models_inc[i].put_state(decomp.get_binary_models()[i].get_number_of_SV()-b_model->get_number_of_SV());
			binary_models_inc[i].put_b(b_model->get_b());
			binary_models_inc[i].put_A(b_model->get_A());
			binary_models_inc[i].put_B(b_model->get_B());
			float *coefficients = new float [decomp.get_binary_models()[i].get_number_of_SV()];
			for (int j=0; j<b_model->get_number_of_SV(); j++)
				coefficients[j]=b_model->get_coefficients()[j];
			for(int j=b_model->get_number_of_SV(); j<decomp.get_binary_models()[i].get_number_of_SV();j++)
				coefficients[j]=-1;
			binary_models_inc[i].put_coefficients(coefficients);
		}
	}

	decomp.put_binary_models(binary_models_inc);
	decomp.save_decomposition();

	//delete [] binary_models_inc;
	//getchar();
	//decomp.show();
}

void MLClassification::remove_b_model(int k)
{
	char ch,num[10];
	char *f_model;
	
	sprintf(num,"%5.5d",k);
	string s,s1;
	s1.erase();
	for (int i=0; i<5; i++)
	{
		ch=num[i];
		s1=s1+ch;
	}
	s="bmodel"+s1+".txt";
	f_model=(char *) s.c_str();

	remove(f_model);
}

void MLClassification::rename_b_model(int k, int l)
{
	Binary_Model *b_model;
	b_model = new Binary_Model;

	char ch,num[10];
	char *f_model_old;
	char *f_model_new;
	
	sprintf(num,"%5.5d",k);
	string s,s1;
	s1.erase();
	for (int i=0; i<5; i++)
	{
		ch=num[i];
		s1=s1+ch;
	}
	s="bmodel"+s1+".txt";
	f_model_old=(char *) s.c_str();
	b_model->read(f_model_old);

	sprintf(num,"%5.5d",l);
	s1.erase();
	for (int i=0; i<5; i++)
	{
		ch=num[i];
		s1=s1+ch;
	}
	s="bmodel"+s1+".txt";
	f_model_new=(char *) s.c_str();
	b_model->save(f_model_new);

	delete b_model;

	remove(f_model_old);

	//rename(f_model_old, f_model_new);
}

