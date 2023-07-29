/*!
 * \file CtaStraContext.h
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 * 
 * \brief 
 */
#pragma once
#include "CtaStraBaseCtx.h"

#include "../Includes/WTSDataDef.hpp"

NS_WTP_BEGIN
class WtCtaEngine;
NS_WTP_END

USING_NS_WTP;

class CtaStrategy;

class CtaStraContext : public CtaStraBaseCtx
{
public:
	CtaStraContext(WtCtaEngine* engine, const char* name, int32_t slippage);
	~CtaStraContext() override;

	void set_strategy(CtaStrategy* stra){ _strategy = stra; }
	CtaStrategy* get_stragety() { return _strategy; }

public:
	//»Øµ÷º¯Êý
	void on_init() override;
	void on_session_begin(uint32_t uTDate) override;
	void on_session_end(uint32_t uTDate) override;
	void on_tick_updated(const char* stdCode, WTSTickData* newTick) override;
	void on_bar_close(const char* stdCode, const char* period, WTSBarStruct* newBar) override;
	void on_calculate(uint32_t curDate, uint32_t curTime) override;
	void on_condition_triggered(const char* stdCode, double target, double price, const char* usertag) override;

private:
	CtaStrategy*		_strategy{};
};


