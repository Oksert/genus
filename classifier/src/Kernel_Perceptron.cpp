#include "Kernel_Perceptron.h"

#define LOG_PATH "Classifier.KernelPerceptron"

//---------------------------------------------------------------------------------
Kernel_Perceptron::Kernel_Perceptron(void)
{
	lambda=0;
	eta=0.01;//0.01;
	ro=0.3;//5;//3;//0.1;
	platt=0;
	do_cash_measure = 0;
}

Kernel_Perceptron::~Kernel_Perceptron(void)
{
	//std::cout << "Kernel_Perceptron::~Kernel_Perceptron1" << std::endl;
	//delete [] target_predict;
	//delete [] f_predict;
	//std::cout << "Kernel_Perceptron::~Kernel_Perceptron2" << std::endl;
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
/**
 * результирующа€ целева€ функци€ f(x)=сумма(alpha[i]*k(i,x))-b
 */
float Kernel_Perceptron::f()
{
	float sum=0;
	for(int i=0;i<T;i++)
	{
		int ii=binary_model->get_numbers()[i];
		if (dataset==1)
			sum+=alpha[i]*exp(7*log(similarity[ii]+1));
		else
			sum+=alpha[i]*similarity[ii];
	}
	sum-=b;
	return sum;
}

/**
 * вычисление значений целевой функции дл€ обучающих документов 
 */
float Kernel_Perceptron::f_platt(int j, CModule::Measure *measure)
{
	float sum=0;
	int jj=binary_model->get_numbers()[j];
	if (do_cash_measure == 0)
		for(int i=0;i<T;i++)
		{
			int ii=binary_model->get_numbers()[i];
			if (dataset==1)
				sum+=alpha[i]*exp(7*log(measure->similarity(ii,jj)+1));
			else
				sum+=alpha[i]*measure->similarity(ii,jj);
		}
	else
		for(int i=0;i<T;i++)
		{
			int ii=binary_model->get_numbers()[i];
			if (dataset==1)
				sum+=alpha[i]*exp(7*log(cash_measure[ii][jj]+1));
			else
				sum+=alpha[i]*cash_measure[ii][jj];
		}
	sum-=b;
	return sum;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void Kernel_Perceptron::train(CModule::Measure *measure)
{
	//log4cpp::Category::getInstance("classifier.KernelPerceptron").infoStream()<<"Traning process..."<<log4cpp::CategoryStream::ENDLINE;
	///PyLogger::info("",LOG_PATH) << "Traning process...";
	num_of_train_docs=binary_model->get_number_of_SV();
	//бинарные метки обучающих документов   
	int *target = binary_model->get_labels();
	
	//инициализаци€
	alpha = new float [num_of_train_docs];
	//alpha = binary_model->get_coefficients();
	//b=binary_model->get_b();
	b=0;

	float f_t;//значение целевой функции на шаге t
	int sigma_t;//индикатор margin error на шаге t
	float *beta;//вспомогательный массив длины T - дл€ ускорени€
	beta = new float [T];

	for(int i=0;i<T;i++)
	{
		beta[i]=exp(i*log(1-lambda*eta));
	}

	//обучение
	for(int t=1;t<=T;t++)
	{
		//cout<<"train"<<t<<endl;
		f_t=0;
		int tt=binary_model->get_numbers()[t-1];
		if (do_cash_measure == 0)
			for (int i=1;i<t;i++)
			{
				int ii=binary_model->get_numbers()[i-1];
				if (dataset==1)
					f_t+=alpha[i-1]*beta[t-i-1]*exp(7*log(measure->similarity(ii,tt)+1));
				else
					f_t+=alpha[i-1]*beta[t-i-1]*measure->similarity(ii,tt);
			}
		else
			for (int i=1;i<t;i++)
			{
				int ii=binary_model->get_numbers()[i-1];
				if (dataset==1)
					f_t+=alpha[i-1]*beta[t-i-1]*exp(7*log(cash_measure[ii][tt]+1));
				else
					f_t+=alpha[i-1]*beta[t-i-1]*cash_measure[ii][tt];
			}
		f_t+=b;
		if (target[t-1]*f_t<=ro) sigma_t=1; else sigma_t=0;
		alpha[t-1]=eta*sigma_t*target[t-1];
		b+=eta*sigma_t*target[t-1];
	}
	
	for(int i=1;i<=T;i++)
	{
		alpha[i-1]=alpha[i-1]*beta[T-i];
	}
	b=(-1)*b;

	///write_alph(); 

	//сохранение модели - коэффициентов опорных векторов 
	//и состо€ние модели
	binary_model->put_coefficients(alpha);
	binary_model->put_b(b);
	binary_model->put_state(2);//просчитанна€

	delete [] beta;
}

///////////////////////////////////////////////////////////////////////
void Kernel_Perceptron::predict()
{
	//cout <<endl<<"Classification process..."<<endl;
	//PyLogger::info("",LOG_PATH) << "Classification process...";

	alpha = binary_model->get_coefficients();
	b=binary_model->get_b();

	num_of_test_docs=1;

	target_predict = new int [num_of_test_docs];
	f_predict=new float [num_of_test_docs];
	for (int j=0; j<num_of_test_docs; j++) 
	{
		f_predict[j]=f();
		if (f_predict[j]>0) target_predict[j]=1; else target_predict[j]=-1;
	}
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void Kernel_Perceptron::predict_platt(CModule::Measure *measure)
{
	//log4cpp::Category::getInstance("classifier.KernelPerceptron").infoStream()<<"Classification process..."<<log4cpp::CategoryStream::ENDLINE;
	//PyLogger::info("Classification process...",LOG_PATH);
	alpha = binary_model->get_coefficients();
	b=binary_model->get_b();

	target_predict = new int [num_of_test_docs];
	f_predict=new float [num_of_test_docs];
	for (int j=0; j<num_of_test_docs; j++) 
	{
		f_predict[j]=f_platt(j,measure);
		if (f_predict[j]>0) target_predict[j]=1; else target_predict[j]=-1;
	}
}
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
/**
 * вывод значений коэффициентов 
 */
void Kernel_Perceptron::write_alph()
{
	//for(int i=0;i<T;i++)
		////PyLogger::info("",LOG_PATH) << "alpha[" << i << "]= " << alpha[i];//log4cpp::Category::getInstance("classifier.KernelPerceptron").infoStream()<<"alpha["<<i<<"]= "<<alpha[i]<<log4cpp::CategoryStream::ENDLINE;
	//log4cpp::Category::getInstance("classifier.KernelPerceptron").infoStream() <<"b= "<<b<<log4cpp::CategoryStream::ENDLINE;
	////PyLogger::info("",LOG_PATH) << "b= " << b;
}

//######################################################

void Kernel_Perceptron::incrementive_learning(CModule::Measure *measure_new, int number_of_docs)
{
	cout <<endl<<"Incrementive learning process..."<<endl;

	num_of_train_docs=binary_model->get_number_of_SV();
	//бинарные метки обучающих документов
	int *target = binary_model->get_labels();
	
	//инициализаци€
	alpha = binary_model->get_coefficients();
	b=(-1)*binary_model->get_b();

	// вычисление ширины граничной полосы на основе результатов основного обучени€
	// ro=1/(||w||), w - ширина раздел€ющей граничной полосы
	// ||w||=sqrt[summa(i){summa(j)[alpha_i*alpha_j*k(x_i,x_j)]}]
	int number_of_old_SV = T-binary_model->get_state();
	cout<<"number_of_old_SV = "<<number_of_old_SV<<endl;
	float summa=0, w;
	if (do_cash_measure == 0)
		for (int i=0; i<number_of_old_SV; i++)
			for (int j=0; j<number_of_old_SV; j++)
				summa+=alpha[i]*alpha[j]*measure_new->similarity(i,j);
	else
		for (int i=0; i<number_of_old_SV; i++)
			for (int j=0; j<number_of_old_SV; j++)
				summa+=alpha[i]*alpha[j]*cash_measure[i][j];
	w=sqrt(summa);
	if (w!=0)
		ro= 1/w;
	else
		ro = 0.01;
	cout<<"ro = "<<ro<<endl;


	//количество новых документов дл€ дообучени€
	//int count=0;//, number_of_docs;
	//for (int i=T-1; i>=0; i--)
	//	if (alpha[i]==1) 
	//	{
	//		count++;///!!!!
	//	}
	//	else number_of_docs=count;
	// cout<<"number_of_IL_docs = "<<binary_model->get_state()<<endl;

	float f_t;//значение целевой функции на шаге t
	int sigma_t;//индикатор margin error на шаге t
	float *beta;//вспомогательный массив длины T - дл€ ускорени€
	beta = new float [T];

	for(int i=0;i<T;i++)
	{
		beta[i]=exp(i*log(1-lambda*eta));
	}

	//дообучение
	for(int t=T-binary_model->get_state()+1;t<=T;t++)
	{
		//cout<<"train"<<t<<endl;
		f_t=0;
		int tt=binary_model->get_numbers()[t-1];
		if (do_cash_measure == 0)
			for (int i=1;i<t;i++)
			{
				int ii=binary_model->get_numbers()[i-1];
				if (dataset==1)
					f_t+=alpha[i-1]*beta[t-i-1]*exp(7*log(measure_new->similarity(ii,tt)+1));
				else
					f_t+=alpha[i-1]*beta[t-i-1]*measure_new->similarity(ii,tt);
			}
		else
			for (int i=1;i<t;i++)
			{
				int ii=binary_model->get_numbers()[i-1];
				if (dataset==1)
					f_t+=alpha[i-1]*beta[t-i-1]*exp(7*log(cash_measure[ii][tt]+1));
				else
					f_t+=alpha[i-1]*beta[t-i-1]*cash_measure[ii][tt];
			}
		f_t+=b;
		if (target[t-1]*f_t<=ro) sigma_t=1; else sigma_t=0;
		alpha[t-1]=eta*sigma_t*target[t-1];
		b+=eta*sigma_t*target[t-1];
	}
	
	for(int i=T-binary_model->get_state()+1;i<=T;i++)
	{
		alpha[i-1]=alpha[i-1]*beta[T-i];
	}

	b=(-1)*b;///в конце!!   

	///write_alph(); 

	//сохранение модели - коэффициентов опорных векторов 
	//и состо€ние модели
	binary_model->put_coefficients(alpha);
	binary_model->put_b(b);
	binary_model->put_state(-2);//просчитанна€

	delete [] beta;
}
