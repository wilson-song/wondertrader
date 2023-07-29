#pragma once
#include "../WtCore/IExecCommand.h"

USING_NS_WTP;

class ExpExecuter : public IExecCommand
{
public:
	explicit ExpExecuter(const char* name):IExecCommand(name){}

	void	init();


	void set_position(const faster_hashmap<LongKey, double>& targets) override;


	void on_position_changed(const char* stdCode, double targetPos) override;

};

