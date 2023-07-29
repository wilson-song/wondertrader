/*!
 * \file HftStraContext.h
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 * 
 * \brief 
 */
#pragma once
#include "HftStraBaseCtx.h"


USING_NS_WTP;

class HftStrategy;

class HftStraContext : public HftStraBaseCtx
{
public:
	HftStraContext(WtHftEngine* engine, const char* name, bool bAgent, int32_t slippage);
	~HftStraContext() override;

	void set_strategy(HftStrategy* stra){ _strategy = stra; }
	HftStrategy* get_stragety() { return _strategy; }

public:
	void on_init() override;

	void on_session_begin(uint32_t uTDate) override;

	void on_session_end(uint32_t uTDate) override;

	void on_tick(const char* code, WTSTickData* newTick) override;

	void on_order_queue(const char* stdCode, WTSOrdQueData* newOrdQue) override;

	void on_order_detail(const char* stdCode, WTSOrdDtlData* newOrdDtl) override;

	void on_transaction(const char* stdCode, WTSTransData* newTrans) override;

	void on_bar(const char* code, const char* period, uint32_t times, WTSBarStruct* newBar) override;

	void on_trade(uint32_t localid, const char* stdCode, bool isBuy, double vol, double price) override;

	void on_order(uint32_t localid, const char* stdCode, bool isBuy, double totalQty, double leftQty, double price, bool isCanceled = false) override;

	void on_channel_ready() override;

	void on_channel_lost() override;

	void on_entrust(uint32_t localid, const char* stdCode, bool bSuccess, const char* message) override;

	void on_position(const char* stdCode, bool isLong, double prevol, double preavail, double newvol, double newavail, uint32_t tradingday) override;


private:
	HftStrategy*		_strategy;
};

