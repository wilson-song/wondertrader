/*!
 * \file WtRiskMonFact.cpp
 *
 * \author Wesley
 * \date 2020/03/30
 *
 * 
 */
#include "WtRiskMonFact.h"
#include "WtSimpleRiskMon.h"

const char* FACT_NAME = "WtRiskMonFact";

extern "C"
{
	EXPORT_FLAG IRiskMonitorFactory* createRiskMonFact()
	{
		IRiskMonitorFactory* fact = new WtRiskMonFact();
		return fact;
	}

	EXPORT_FLAG void deleteRiskMonFact(IRiskMonitorFactory* fact)
	{
        delete fact;
	}
}


WtRiskMonFact::WtRiskMonFact() = default;


WtRiskMonFact::~WtRiskMonFact() = default;

const char* WtRiskMonFact::getName()
{
	return FACT_NAME;
}

void WtRiskMonFact::enumRiskMonitors(FuncEnumRiskMonCallback cb)
{
	//cb(FACT_NAME, "WtSimpExeUnit", false);
	cb(FACT_NAME, "SimpleRiskMon", true);
}

WtRiskMonitor* WtRiskMonFact::createRiskMonitor(const char* name)
{
	if (strcmp(name, "SimpleRiskMon") == 0)
		return new WtSimpleRiskMon();
	return nullptr;
}

bool WtRiskMonFact::deleteRiskMonitor(WtRiskMonitor* unit)
{
	if (unit == nullptr)
		return true;

	if (strcmp(unit->getFactName(), FACT_NAME) != 0)
		return false;

	delete unit;
	return true;
}
