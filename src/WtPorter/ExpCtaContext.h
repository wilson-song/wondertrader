/*!
 * \file PyCtaContext.h
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 * 
 * \brief 
 */
#pragma once
#include "../WtCore/CtaStraBaseCtx.h"

USING_NS_WTP;

class ExpCtaContext : public CtaStraBaseCtx
{
public:
	ExpCtaContext(WtCtaEngine* env, const char* name, int32_t slippage);
	~ExpCtaContext() override;

public:
	void on_init() override;

	void on_session_begin(uint32_t uDate) override;

	void on_session_end(uint32_t uDate) override;

	void on_tick_updated(const char* stdCode, WTSTickData* newTick) override;

	void on_bar_close(const char* stdCode, const char* period, WTSBarStruct* newBar) override;

	void on_calculate(uint32_t curDate, uint32_t curTime) override;

	void on_condition_triggered(const char* stdCode, double target, double price, const char* usertag) override;
};

