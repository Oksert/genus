#ifndef FACTORY_H_
#define FACTORY_H_

namespace CModule
{
	
void initParserParams(object &parserParams, object &measureParams, HTMLParserLibrary::Parameters &pars)
{
	dict &d = (dict&)parserParams;
	if(d["processing"] == "STEMMING")
		pars.procType = HTMLParserLibrary::PROCESSING_STEMMING;
	else if(d["processing"] == "GRAMM")
	{
		pars.grammNumber = extract<int>(d["grammNumber"]);
		pars.procType = HTMLParserLibrary::PROCESSING_GRAMM;
	}
	else
		pars.procType = HTMLParserLibrary::PROCESSING_NONE;

	if(measureParams["measure"] == "EPISODES")
		pars.extractPunct = true;
	else
		pars.extractPunct = false;
} 

// parser
Parser *createTrainParser(object &parserParams, object &measureParams)
{
	HTMLParserLibrary::Parameters pars;
	initParserParams(parserParams,measureParams,pars);
	pars.train = true;
	
	ParserImpl *res = new ParserImpl(pars);
	return res;
}

Parser *createPredictParser(object &parserParams, object &measureParams)
{
	HTMLParserLibrary::Parameters pars;
	initParserParams(parserParams,measureParams,pars);
	pars.train = false;
	return new ParserImpl(pars);
}

// package parser
PackageParser *createTrainPackageParser(object &parserParams, object &measureParams, std::string encoding)
{
	HTMLParserLibrary::Parameters pars;
	initParserParams(parserParams,measureParams,pars);
	pars.train = true;
	return new PackageParser(pars,encoding);
}

PackageParser *createPredictPackageParser(object &parserParams, object &measureParams, std::string encoding)
{
	HTMLParserLibrary::Parameters pars;
	initParserParams(parserParams,measureParams,pars);
	pars.train = false;
	return new PackageParser(pars,encoding);
}

RetrainMeasure *createRetrainMeasure(object &parserParams, object &measureParams)
{
	PyLogger::debug("Creating measure","pyBinding.Measure");
	if(measureParams["measure"] == "FREQUENCY")
	{
		return new RetrainMeasureImpl(new Similarity::RetrainFrequencyMeasure());
	}
	else if(measureParams["measure"] == "EPISODES")
	{
		return new FPTreeMeasureImpl(Context::getThreadContext()("episodes_in.txt"),Context::getThreadContext()("episodes_out.txt"),10,2);
	}
	else
		return NULL;
}
// measure
Measure *createMeasure(object &parserParams, object &measureParams)
{
	PyLogger::debug("Creating measure","pyBinding.Measure");
	if(measureParams["measure"] == "FREQUENCY")
	{
		return new RetrainMeasureImpl(new Similarity::RetrainFrequencyMeasure());
	}
	else if(measureParams["measure"] == "K_SPECTRUM")
	{
		return new MeasureImpl(new Similarity::KSpectrum(extract<int>(measureParams.attr("kernel_number")),NULL,0));
	}
	else if(measureParams["measure"] == "EPISODES")
	{
		if(parserParams["processing"] == "GRAMM")
			return new FPTreeMeasureImpl(Context::getThreadContext()("episodes_in.txt"),Context::getThreadContext()("episodes_out.txt"),10,2);
			//return new AprMeasure(Context::getThreadContext()("episodes_in.txt"),Context::getThreadContext()("episodes_out.txt"),10,2);
		else if(parserParams["processing"] == "STEMMING")
			return new FPTreeMeasureImpl(Context::getThreadContext()("episodes_in.txt"),Context::getThreadContext()("episodes_out.txt"),10,2);
			//return new AprMeasure(Context::getThreadContext()("episodes_in.txt"),Context::getThreadContext()("episodes_out.txt"),10,2);
	}
	else
		return NULL;
}

// classifier
Classifier *createClassifier(object &params)
{
	Classifier *classObj = new Classifier();

	if (params["decompType"] == "oneVsOne") classObj->put_decomposition_type(0);
	else if (params["decompType"] == "oneVsAll") classObj->put_decomposition_type(1);

	if (params["classType"] == "SVM") classObj->put_method(0);
	else if (params["classType"] == "KP") classObj->put_method(1);

	return classObj;
}

}

#endif /*FACTORY_H_*/
