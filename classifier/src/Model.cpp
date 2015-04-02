#include "Model.h"
#include <Context.h>
#include <Logger.h>
//#include <log4cpp/Category.hh>
#include "decompositionMatrix/DecompositionMatrixGenerator.h"

#define LOG_PATH "Classifier.Model"
/**
 * Инициализация модели
 */
Model::Model()
{
	decomposition_matrix=NULL;
	number_of_binary_problems=0;
	multiclass_decomposition_type=0;
	supp_vectors=NULL;
	set_of_binary_models=NULL;
};

Model::~Model()
{
	for (int i=0; i<number_of_binary_problems; i++)
		delete decomposition_matrix[i];
	delete [] decomposition_matrix;
	//cout <<"~Model()"<<endl;
};

void Model::create_decomposition_matrix ()
{
	int** I = NULL;
	if(multiclass_decomposition_type == ONE_ONE || number_of_topics <= 2)
	{
		number_of_binary_problems = number_of_topics*(number_of_topics-1);

		I = new int* [number_of_binary_problems];
		int r,i,j=0,k=j+1;
		for (i=0; i<number_of_binary_problems/2;i++)
		{
			I[i] = new int [number_of_topics];
			I[number_of_binary_problems-i-1]=new int [number_of_topics];
			for(r=0; r<number_of_topics;r++)
			{
				I[i][r]=0;
				I[number_of_binary_problems-i-1][r]=0;
			}
			I[i][j]= +1;
			I[i][k]= -1;
			I[number_of_binary_problems-i-1][j]=-1;
			I[number_of_binary_problems-i-1][k]=+1;
			if (k==number_of_topics-1) 
			{
				j++; k=j+1;
			}
			else
				k++;
		}
	}
	else if(multiclass_decomposition_type == ONE_ALL)
	{
		int i,j;
		number_of_binary_problems = number_of_topics;
		I = new int* [number_of_binary_problems];
		for(i=0; i<number_of_binary_problems; i++)
		{
			I[i] = new int [number_of_topics];
			for(j=0; j<number_of_topics; j++)
				I[i][j] = -1;
			I[i][i] = 1;
		}
	}
	else if (multiclass_decomposition_type == DENSE)
	{
		//cout << "im in reading matrix for dense" << endl;
		
		DecompositionMatrix* D = new DecompositionMatrix(DecompositionMatrixGenerator::generateDenseMatrix(number_of_topics));
		I = *D;
		number_of_binary_problems = D->getBinaryCount();
	}
	else if (multiclass_decomposition_type == SPARSE)
	{
		//cout << "im in reading matrix for sparce" << endl;
		
		DecompositionMatrix* D = new DecompositionMatrix(DecompositionMatrixGenerator::generateSparseMatrix(number_of_topics,0.8));
		I = *D;
		number_of_binary_problems = D->getBinaryCount();		
	}
	
	decomposition_matrix=I;
}

void Model::show()
{ 
	//log4cpp::Category::getInstance("classifier.Model").infoStream() << "Number Of Topics =" << number_of_topics <<log4cpp::CategoryStream::ENDLINE;
	PyLogger::info("",LOG_PATH) << "Number Of Topics =" << number_of_topics;
	//log4cpp::Category::getInstance("classifier.Model").infoStream() << "Number Of Binary Problems =" << number_of_binary_problems <<log4cpp::CategoryStream::ENDLINE;
	PyLogger::info("",LOG_PATH) << "Number Of Binary Problems =" << number_of_binary_problems; 
	//log4cpp::Category::getInstance("classifier.Model").infoStream() << "Multiclass decomposition type =";
	{
		PyLogger logger = PyLogger::info("",LOG_PATH);
		logger << "Multiclass decomposition type =";
		if (multiclass_decomposition_type==0) logger <<"ONE_VS_ONE";
		else if (multiclass_decomposition_type==1) logger <<"ONE_VS_OTHERS";
	}
	PyLogger::info("",LOG_PATH) << "Decomposition Matrix";
	for (int i=0; i<number_of_binary_problems;i++) 
	{
		PyLogger logger = PyLogger::info();
		for (int j=0; j<number_of_topics;j++)
			logger <<decomposition_matrix[i][j]<<" ";
	}

	cout <<endl;
	//support_vectors->show();
	cout <<endl;

	if (set_of_binary_models!=NULL)
	{
		for (int j=0; j<number_of_binary_problems; j++) 
		{
			set_of_binary_models[j].show();
		}
	}
}

void Model::read_model(char *f_model)
{
	FILE *f;
	int intch;
	
	Context context = Context::getThreadContext();
	f=fopen(context(f_model).c_str(),"r");
	if (!f)
	{
		//log4cpp::Category::getInstance("classifier.Model").error("file not found");
		PyLogger::error("file not found",LOG_PATH);
		return;
	}

	intch= getc(f);
	while ((intch!= -1)&&(intch!=' ')&&(intch!='\n')) intch= getc(f);
	fscanf(f,"%d",&number_of_topics);
	//cout << number_of_topics <<endl;
	
	intch= getc(f);intch= getc(f);
	while ((intch!= -1)&&(intch!=' ')&&(intch!='\n')) intch= getc(f);
	fscanf(f,"%d",&number_of_SV);
	//cout << number_of_SV <<endl;

	intch= getc(f);intch= getc(f);
	while ((intch!= -1)&&(intch!=' ')&&(intch!='\n')) intch= getc(f);
	fscanf(f,"%d",&number_of_binary_problems);
	//cout << number_of_binary_problems <<endl;

	intch= getc(f);intch= getc(f);
	while ((intch!= -1)&&(intch!=' ')&&(intch!='\n')) intch= getc(f);
	fscanf(f,"%d",&multiclass_decomposition_type);
	//cout << multiclass_decomposition_type <<endl;
	
	intch= getc(f);intch= getc(f);
	while ((intch!= -1)&&(intch!=' ')&&(intch!='\n')) intch= getc(f);
	decomposition_matrix = new int* [number_of_binary_problems];
	for (int i=0; i<number_of_binary_problems;i++) 
	{
		decomposition_matrix[i] = new int [number_of_topics];
		for (int j=0; j<number_of_topics;j++) 
		{
			fscanf(f,"%d",&decomposition_matrix[i][j]);
			//cout << decomposition_matrix[i][j] <<" ";
		}
		intch= getc(f);
		//cout <<endl;
	}
	fclose(f);
}

void Model::save_model(char *f_mod /*, char *f_SVs*/)
{ 
	ofstream f_model;
	Context context = Context::getThreadContext();
	f_model.open(context(f_mod).c_str(),ios::out|ios::trunc);
	if (!f_model)
	{
		//log4cpp::Category::getInstance("classifier.Model").error("file not found");
		PyLogger::error("file not found",LOG_PATH);
		return;
	}
	f_model.close();

	f_model.open(context(f_mod).c_str(),ios::out|ios::app);
	if (!f_model)
	{
		//log4cpp::Category::getInstance("classifier.Model").error("file not found");
		PyLogger::error("file not found",LOG_PATH);
		return;
	}
	
	f_model << "Number_Of_Topics= " << number_of_topics <<endl;
	f_model << "Number_Of_Support_Vectors= " << number_of_SV <<endl;
	f_model << "Number_Of_Binary_Problems= " << number_of_binary_problems <<endl; 
	f_model << "Multiclass_decomposition_type= ";
	if (multiclass_decomposition_type==0) f_model <<0;//"ONE_VS_ONE";
	else if (multiclass_decomposition_type==1) f_model <<1;//"ONE_VS_OTHERS";
	f_model <<endl;
	f_model << "Decomposition_Matrix" <<endl;
	for (int i=0; i<number_of_binary_problems;i++) 
	{
		for (int j=0; j<number_of_topics;j++) 
			f_model <<decomposition_matrix[i][j]<<" ";
		f_model <<endl;
	}
	f_model.close();
}

void Model::delete_zero()
{
	int m=0, flag=0;
	int number_SVs;
	number_SVs=number_of_SV;

	int *Del;
	Del= new int [number_of_SV];
	for (int l=0; l<number_of_SV; l++) Del[l]=1;
	//Del[l]:=0, если опорный вектор с номером l надо удалить из общей модели

	int *Recount;
	Recount= new int [number_of_SV];

	//цикл по опорным векторам в общей модели
	for (int i=0; i<number_of_SV;i++) 
	{
		flag=0;
		// цикл по бинарным моделям
		for (int j=0; j<number_of_binary_problems;j++) 
		{
			//цикл по опорным векторам бинарных моделей
			for (int k=0; k<set_of_binary_models[j].get_number_of_SV(); k++)
			{
				if ((set_of_binary_models[j].get_numbers()[k]==i)
					&&(set_of_binary_models[j].get_coefficients()[k]>0))
				{
					flag=1; break;
				}
			}
			if (flag==1) break;
		}
		if (flag==0) Del[i]=0;
	}

	//формирование массива пересчёта (по массиву Del)
	int count=0;
	for (int l=0; l<number_of_SV; l++) 
	{
		if (Del[l]==0) 
		{
			Recount[l]=-1;
			count++;
		}
		else Recount[l]=l-count;
	}

	//пересчёт всех numbers во всех бинарных моделях 
	//(на основе созданного массива пересчёта)	
	int *numbers;
	// цикл по бинарным моделям
	for (int j=0; j<number_of_binary_problems;j++) 
	{
		numbers=set_of_binary_models[j].get_numbers();
		//цикл по опорным векторам бинарных моделей
		for (int k=0; k<set_of_binary_models[j].get_number_of_SV(); k++)
		{
			numbers[k]=Recount[numbers[k]];
		}
		set_of_binary_models[j].put_numbers(numbers);
	}

	//удаление "ненужных" опорных векторов из общей модели
	if (number_SVs>0)
	{
		while ((Del[m]==0)&&(number_SVs>0))
		{
			number_SVs--;
			if (number_SVs>0) supp_vectors++;
			m++;
		}
	}
	if (number_SVs>0)
	{
		while (m<number_of_SV-1)
		{
			m++;
			if (Del[m]==0)
			{
				for (int l=m;l<number_SVs-1; l++)
					supp_vectors[l]=supp_vectors[l+1];
				number_SVs--;
			}
		}
	}
	
	
	number_of_SV=number_SVs;
	//create_support_vectors_set();

	for (int j=0; j<number_of_binary_problems;j++) 
	{
		set_of_binary_models[j].delete_zero();
	}

	delete [] Del;
	delete [] Recount;
}
