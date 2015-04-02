#include "SVM_SMO.h"
//#include <log4cpp/Category.hh>
#include <cstdlib>

#define LOG_PATH "Classifier.SVM_SMO"

//-------------------------------------------------------------------
SMO::SMO()
{
	C=1.0f;
	tolerance=0.1;//0.3f;//0.001;
	eps=0.001;//0.1f;//0.001;
	platt=0;
}

SMO::~SMO()
{
	delete [] target_predict;
	delete [] f_predict;
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
/**
 * целевая функция f(x)=сумма(alpha[i]*Y[i]*k(i,x))-b
 */
float SMO::learned_func_nonlinear(int k, CModule::Measure *measure)
{
	float sum=0;
	int kk=binary_model->get_numbers()[k];
	if (do_cash_measure ==0)
		for(int i=0;i<number_of_SV;i++)
		{
			if(alpha[i]>0)
			{
				int ii =binary_model->get_numbers()[i];
				if (dataset==1)
					sum+=alpha[i]*target[i]*exp(7*log(measure->similarity(ii,kk)+1));
				else
					sum+=alpha[i]*target[i]*measure->similarity(ii,kk); 
			}
		}
	else
		for(int i=0;i<number_of_SV;i++)
		{
			if(alpha[i]>0)
			{
				int ii =binary_model->get_numbers()[i];
				if (dataset==1)
					sum+=alpha[i]*target[i]*exp(7*log(cash_measure[ii][kk]+1));
				else
					sum+=alpha[i]*target[i]*cash_measure[ii][kk]; 
			}
		}
	sum-=b;
	return sum;
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
/**
 * результирующая целевая функция f(x)=сумма(alpha[i]*k(i,x))-b
 */
float SMO::f()
{
//struct timespec startTime13; 
//clock_gettime(CLOCK_REALTIME, &startTime13);	
	float sum=0;
	for(int i=0;i<number_of_SV;i++)
	{
		int ii=binary_model->get_numbers()[i];
		if (dataset==1)
			sum+=alpha[i]*exp(7*log(similarity[ii]+1));
		else
			sum+=alpha[i]*similarity[ii];
	}
	sum-=b;
//struct timespec endTime13; 
//clock_gettime(CLOCK_REALTIME, &endTime13);
//std::cout<<"smo_predictTime2=" << endTime13.tv_sec-startTime13.tv_sec<<" "<<endTime13.tv_nsec-startTime13.tv_nsec<<std::endl;


	return sum;
}

float SMO::f_platt(int j, CModule::Measure *measure)
{
	float sum=0;
	int jj=binary_model->get_numbers()[j];
	if (do_cash_measure == 0)
		for(int i=0;i<number_of_SV;i++)
		{
			int ii=binary_model->get_numbers()[i];
			if (dataset==1)
				sum+=alpha[i]*exp(7*log(measure->similarity(ii,jj)+1));
			else
				sum+=alpha[i]*measure->similarity(ii,jj);
		}
	else
		for(int i=0;i<number_of_SV;i++)
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

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------

void SMO::write_alph()
{
	//for(int i=0;i<number_of_SV;i++)
		////PyLogger::info("",LOG_PATH)<<"alpha["<<i<<"]= "<<alpha[i];//log4cpp::Category::getInstance("classifier.SMO").infoStream()<<"alpha["<<i<<"]= "<<alpha[i]<<log4cpp::CategoryStream::ENDLINE;
	//log4cpp::Category::getInstance("classifier.SMO").infoStream() <<"b= "<<b<<log4cpp::CategoryStream::ENDLINE;
	////PyLogger::info("",LOG_PATH)<<"b= "<<b;
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
/**
 * совместная оптимизация множителей Лагранжа i1 и i2
 * возвращает 1, если совместная оптимизация i1 и i2 успешна,
 *           0 - иначе
 */
int SMO::takeStep(int i1, int i2, CModule::Measure *measure) 
{ 
	int y1, y2, s;
	float delta_b;
	float alph1, alph2;     // старые значения alpha_1, alpha_2 
	float a1, a2;           // новые значения alpha_1, alpha_2
	float E1, E2, L, H, k11, k22, k12, eta, Lobj, Hobj;
	if (i1 == i2) return 0;
	
	/******************************************************/
	//y,E
	alph1 = alpha[i1];
	y1 = target[i1];
	if (alph1 > 0 && alph1 < C)
	{
		E1 = error_cache[i1];
	}
	else 
	{
		E1 = learned_func_nonlinear(i1,measure) - y1;
	}
	alph2 = alpha[i2];
	y2 = target[i2];
	if (alph2 > 0 && alph2 < C)
	{
		E2 = error_cache[i2];
	}
	else 
	{
		E2 = learned_func_nonlinear(i2,measure) - y2;
	}
	s = y1 * y2;
	
	/*******************************************************/
	if (y1 == y2)
	{
		float gamma = alph1 + alph2;
		if (gamma > C) 
		{
		    L = gamma-C;
			H = C;
		}
		else 
		{
			L = 0;
			H = gamma;
		}
	}
	else
	{
		float gamma = alph1 - alph2;
		if (gamma > 0) 
		{
			L = 0;
			H = C - gamma;
		}
		else
		{
			L = -gamma;
			H = C;
		}
	}
	if(L==H) return 0;
	  
	/********************************************************/
	//eta=2K12-K11-K22
	int ii1=binary_model->get_numbers()[i1];
	int ii2=binary_model->get_numbers()[i2];
	if (do_cash_measure == 0)
	{
		k11 = measure->similarity(ii1,ii1); if (dataset==1) k11=exp(7*log(k11+1));
		k22 = measure->similarity(ii2,ii2); if (dataset==1) k22=exp(7*log(k22+1));
		k12 = measure->similarity(ii1,ii2); if (dataset==1) k12=exp(7*log(k12+1));
	}
	else
	{
		k11 = cash_measure[ii1][ii1]; if (dataset==1) k11=exp(7*log(k11+1));
		k22 = cash_measure[ii2][ii2]; if (dataset==1) k22=exp(7*log(k22+1));
		k12 = cash_measure[ii1][ii2]; if (dataset==1) k12=exp(7*log(k12+1));
	}
	eta = 2 * k12 - k11 - k22;
	/********************************************************/
	//H, L
	if (eta < 0) 
	{
		a2 = alph2 + y2 * (E2 - E1) / eta;
		if (a2 < L)
		{
			a2 = L;
		}
		else
		{
			if (a2 > H)
			a2 = H;
		}
	}
	else 
	{
		float c1 = eta/2;
		float c2 = y2 * (E1-E2)- eta * alph2;
		Lobj = c1 * L * L + c2 * L;
		Hobj = c1 * H * H + c2 * H;
	    if (Lobj > Hobj+eps)
			a2 = L;
	    else if (Lobj < Hobj-eps)
				a2 = H;
			 else
				a2 = alph2;
	}
	/********************************************************/
	//alph1
	if (fabs(a2-alph2) < eps*(a2+alph2+eps)) return 0;
	a1 = alph1 - s * (a2 - alph2);
	if (a1 < 0) 
	{
		a2 += s * a1;
		a1 = 0;
	}
	else if (a1 > C) 
	{
		float t = a1-C;
		a2 += s * t;
		a1 = C;
	}
 
	/**********************************************************/
	//обновляем порог b, учитывая изменённые множители Лагранжа   
	float b1, b2, bnew;
	if (a1 > 0 && a1 < C)
	{
		bnew = b + E1 + y1 * (a1 - alph1) * k11 + y2 * (a2 - alph2) * k12;
	}
	else 
	{
		if (a2 > 0 && a2 < C)
			bnew = b + E2 + y1 * (a1 - alph1) * k12 + y2 * (a2 - alph2) * k22;
		else 
		{
			b1 = b + E1 + y1 * (a1 - alph1) * k11 + y2 * (a2 - alph2) * k12;
			b2 = b + E2 + y1 * (a1 - alph1) * k12 + y2 * (a2 - alph2) * k22;
			bnew = (b1 + b2) / 2;
		}
	}
	delta_b = bnew - b;
	b = bnew;
	///************************************************************/
	//error_cache
	float t1 = y1 * (a1-alph1);
	float t2 = y2 * (a2-alph2);
	float k_m_i1_i, k_m_i2_i;
  
	for (int i=0; i<number_of_SV; i++)
	{
		if (0 < alpha[i] && alpha[i] < C)
		{
			int ii=binary_model->get_numbers()[i];
			int ii1=binary_model->get_numbers()[i1];
			int ii2=binary_model->get_numbers()[i2];
			if (do_cash_measure == 0)
			{
				k_m_i1_i = measure->similarity(ii1,ii);if (dataset==1) k_m_i1_i=exp(7*log(k_m_i1_i+1));
				k_m_i2_i = measure->similarity(ii2,ii);if (dataset==1) k_m_i2_i=exp(7*log(k_m_i2_i+1));
			}
			else
			{
				k_m_i1_i = cash_measure[ii1][ii];if (dataset==1) k_m_i1_i=exp(7*log(k_m_i1_i+1));
				k_m_i2_i = cash_measure[ii2][ii];if (dataset==1) k_m_i2_i=exp(7*log(k_m_i2_i+1));
			}

			error_cache[i]+=t1 * k_m_i1_i + t2 * k_m_i2_i-delta_b;
		}
	}
	error_cache[i1] = 0.;
	error_cache[i2] = 0.;
	 
	/***********************************************************/
	//обновляем вектор множителей Лагранжа
	alpha[i1] = a1;   // сохраняем a1 в массиве alpha 
	alpha[i2] = a2;   // сохраняем a1 в массиве alpha 
	return 1;
}

//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
/**
 * задавая начальное alpha_i (c индексом i1), examineExample() сначала проверяет,
 * нарушает ли оно KKT-условие больше, чем на tolerance;
 * если да - то ищет второе alpha_i (c индексом i2) и выполняет совместную 
 * оптимизацию двух alpha_i, вызывая takeStep(i1,i2)
 * возвращает 1, если оптимизация выполнилась; 0 - иначе
 */
int SMO::examineExample(int i1, CModule::Measure *measure)
{
	float y1, alph1, E1, r1;//значения целевой функции 
	y1 = (float)target[i1];//классификация точки i1
	alph1 = alpha[i1];//множитель Лагранжа для точки i1
	if (alph1 > 0 && alph1 < C)
		E1 = error_cache[i1];
	else 
		E1 = learned_func_nonlinear(i1, measure) - y1;//вычисление ошибки предсказания  
	r1 = y1 * E1;
	
	if ((r1 < -tolerance && alph1 < C)||(r1 > tolerance && alph1 > 0))
	{ 
		//Е1 нарушает ККТ условие
		//////////////////////////
		//пытаемся найти подходящий пример i2
		//1. Находим неграничный пример i2, который максимизирует |E1-E2|
		if (examineFirstChoice(i1,E1, measure))     //1
		{
			//cout<<"examineFirstChoice"<<endl;			
			return 1;
		}
		//2. Находим произвольный неграничный пример, с которого начинаем
		if (examineNonBound(i1, measure))           //2
		{
			//cout<<"examineNonBound"<<endl;
			return 1;
		}
		//3. Находим произвольный граничный пример, с которого начинаем
		if (examineBound(i1, measure))              //3
		{
			//cout<<"examineNonBound"<<endl;
			return 1;
		}
	}
	return 0;
}

/**
 * находим неграничный пример, который максимизирует |E1-E2|
 */
int SMO::examineFirstChoice(int i1, float E1, CModule::Measure *measure)
{
	int k,i2;
	float tmax;  
	for(i2=-1,tmax=0.0,k=0;k<number_of_SV;k++)
	{
		if(alpha[k]>0&&alpha[k]<C)
		{
			float E2,temp;
			E2=error_cache[k];
			temp=fabs(E1-E2);
			if(temp>tmax)
			{
				tmax=temp;
				i2=k;
			}
		}
	}
    if(i2>=0)
    {
		if(takeStep(i1,i2, measure))
		{
			return 1;
		}
	}
	return 0;
}

/**
 * находим произвольный неграничный пример, с которого начинаем процесс
 */
int SMO::examineNonBound(int i1, CModule::Measure *measure)
{
	int k,k0 = rand()%number_of_SV;
    int i2;
	//цикл по всем i2, соответствующим альфа, не равным 0 не равным С
	//начинаем с произвольной точки  
    for (k = 0; k < number_of_SV; k++)
    {
		i2 = (k + k0) % number_of_SV;//номер текущего альфа       
		if (alpha[i2] > 0.0 && alpha[i2] < C)
		{
			if (takeStep(i1, i2, measure)) 
			{
				return 1;
			}
		}
	}
    return 0;
}

/**
 * находим произвольный граничный пример, с которого начинаем  
 */
int SMO::examineBound(int i1, CModule::Measure *measure)
{
	int k,k0 = rand()%number_of_SV;
    int i2;
	//цикл по всем оставшимся i2
	//начинаем с произвольной точки
	for (k = 0; k < number_of_SV; k++)
	{
		i2 = (k + k0) % number_of_SV;//номер текущего альфа   
		if (takeStep(i1, i2, measure)) 
		{
			return 1;
		}
    }
    return 0;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void SMO::smo_train(CModule::Measure *measure)
{
	//log4cpp::Category::getInstance("classifier.SMO").info("Traning process...");
	////PyLogger::info("Training process...",LOG_PATH);

	number_of_SV=binary_model->get_number_of_SV();
	//бинарные метки обучающих документов 
	target=binary_model->get_labels();
	
	//инициализация коэффициентов нулями
	alpha = new float [number_of_SV];
	for (int i=0; i<number_of_SV; i++) alpha[i]=0;
	b=0;
	error_cache = new float [number_of_SV];

	numChanged = 0;
	examineAll = 1;
	while (numChanged > 0 || examineAll) 
	{
		//cout<<"train";
		numChanged = 0;
		if (examineAll) 
		{ 
			for (int k = 0; k < number_of_SV; k++)
				numChanged += examineExample (k,measure);
		}
		else 
		{ 
			for (int k = 0; k < number_of_SV; k++)
			if (alpha[k] != 0 && alpha[k] != C)
				numChanged += examineExample (k,measure);
        }
		if (examineAll == 1)
			examineAll = 0;
		else 
			if (numChanged == 0)
				examineAll = 1;
	}
	
	for(int i=0;i<number_of_SV;i++)
	{
		alpha[i]=alpha[i]*target[i];
	}

	///write_alph(); 

	//сохранение модели - коэффициентов опорных векторов и состояние модели    
	binary_model->put_coefficients(alpha);
	binary_model->put_b(b);
	binary_model->put_state(2);//просчитанная
	
	delete [] error_cache;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void SMO::smo_predict()
{
//struct timespec startTime13; 
//clock_gettime(CLOCK_REALTIME, &startTime13); 
	//PyLogger::info("Classification process...",LOG_PATH);

	number_of_SV=binary_model->get_number_of_SV();
	alpha = binary_model->get_coefficients();
	b=binary_model->get_b();
//struct timespec endTime13; 
//clock_gettime(CLOCK_REALTIME, &endTime13);
//std::cout<<"smo_predictTime1=" << endTime13.tv_sec-startTime13.tv_sec<<" "<<endTime13.tv_nsec-startTime13.tv_nsec<<std::endl;


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
void SMO::smo_predict_platt(CModule::Measure *measure)
{
	//log4cpp::Category::getInstance("classifier.SMO").info("Classification process...");
	////PyLogger::info("Classification process...",LOG_PATH);
	number_of_SV=binary_model->get_number_of_SV();
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
