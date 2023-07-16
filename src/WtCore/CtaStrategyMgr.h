/*!
 * \file CtaStrategyMgr.h
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 * 
 * \brief 
 */
#pragma once
#include <memory>
#include <boost/core/noncopyable.hpp>

#include "../Includes/FasterDefs.h"
#include "../Includes/CtaStrategyDefs.h"

#include "../Share/DLLHelper.hpp"

class CtaStrategyWrapper
{
public:
	CtaStrategyWrapper(CtaStrategy* strategy, ICtaStrategyFactory* fact) : _strategy(strategy), _factory(fact){}
	~CtaStrategyWrapper()
	{
		if (_strategy)
		{
			_factory->deleteStrategy(_strategy);
		}
	}

	CtaStrategy* self(){ return _strategy; }


private:
	CtaStrategy*		_strategy;
	ICtaStrategyFactory*	_factory;
};
typedef std::shared_ptr<CtaStrategyWrapper>	CtaStrategyPtr;


class CtaStrategyMgr : private boost::noncopyable
{
public:
	CtaStrategyMgr();
	~CtaStrategyMgr();

public:
	bool loadFactories(const char* path);

	CtaStrategyPtr createStrategy(const char* name, const char* id);
	CtaStrategyPtr createStrategy(const char* factname, const char* unitname, const char* id);

	CtaStrategyPtr getStrategy(const char* id);
private:
	typedef struct _StraFactInfo
	{
		std::string		_module_path;
		DllHandle		_module_inst;
		ICtaStrategyFactory*	_fact;
		FuncCreateStrategyFact	_creator;
		FuncDeleteStrategyFact	_remover;
	} StraFactInfo;
	typedef faster_hashmap<LongKey, StraFactInfo> StraFactMap;

	StraFactMap	_factories;

	typedef faster_hashmap<LongKey, CtaStrategyPtr> StrategyMap;
	StrategyMap	_strategies;
};

