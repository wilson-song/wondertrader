/*!
 * \file WtRiskMonFact.h
 *
 * \author Wesley
 * \date 2020/03/30
 *
 * 
 */
#pragma once
#include "../Includes/RiskMonDefs.h"

USING_NS_WTP;

class WtRiskMonFact : public IRiskMonitorFactory
{
public:
	WtRiskMonFact();
	~WtRiskMonFact() override;

public:
	const char* getName() override;
	void enumRiskMonitors(FuncEnumRiskMonCallback cb) override;

	WtRiskMonitor* createRiskMonitor(const char* name) override;

	bool deleteRiskMonitor(WtRiskMonitor* unit) override;

};

