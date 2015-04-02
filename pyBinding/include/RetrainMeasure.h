#ifndef RETRAINMEASURE_H_
#define RETRAINMEASURE_H_

class RetrainMeasure : public CModule::Measure
{
public:
	virtual void retrain(std::vector<std::vector<CModule::id> > &preloadedSet)
	{
		throw std::exception();
	};
};

#endif
