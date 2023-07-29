#pragma once
#include "../WtCore/SelStraBaseCtx.h"

USING_NS_WTP;

class ExpSelContext : public SelStraBaseCtx
{
public:
	ExpSelContext(WtSelEngine* env, const char* name, int32_t slippage);
	~ExpSelContext() override;

public:
	void on_init() override;

	void on_session_begin(uint32_t uDate) override;

	void on_session_end(uint32_t uDate) override;

	void on_strategy_schedule(uint32_t curDate, uint32_t curTime) override;

	void on_bar_close(const char* stdCode, const char* period, WTSBarStruct* newBar) override;

	void on_tick_updated(const char* stdCode, WTSTickData* newTick) override;

};

