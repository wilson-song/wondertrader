/*!
 * \file ExecMocker.h
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 * 
 * \brief 
 */
#pragma once
#include <sstream>
#include "HisDataReplayer.h"

#include "../Includes/ExecuteDefs.h"
#include "../Share/StdUtils.hpp"
#include "../Share/DLLHelper.hpp"
#include "MatchEngine.h"

USING_NS_WTP;

class ExecMocker : public ExecuteContext, public IDataSink, public IMatchSink
{
public:
	explicit ExecMocker(HisDataReplayer* replayer);
	~ExecMocker() override;

public:
	//////////////////////////////////////////////////////////////////////////
	//IMatchSink
	void handle_trade(uint32_t localid, const char* stdCode, bool isBuy, double vol, double fireprice, double price, uint64_t ordTime) override;
	void handle_order(uint32_t localid, const char* stdCode, bool isBuy, double leftover, double price, bool isCanceled, uint64_t ordTime) override;
	void handle_entrust(uint32_t localid, const char* stdCode, bool bSuccess, const char* message, uint64_t ordTime) override;

	//////////////////////////////////////////////////////////////////////////
	//IDataSink
	void handle_tick(const char* stdCode, WTSTickData* curTick, uint32_t pxType) override;
	void handle_schedule(uint32_t uDate, uint32_t uTime) override;
	void handle_init() override;

	void handle_bar_close(const char* stdCode, const char* period, uint32_t times, WTSBarStruct* newBar) override;

	void handle_session_begin(uint32_t curTDate) override;

	void handle_session_end(uint32_t curTDate) override;

	void handle_replay_done() override;

	//////////////////////////////////////////////////////////////////////////
	//ExecuteContext
	WTSTickSlice* getTicks(const char* stdCode, uint32_t count, uint64_t etime = 0) override;

	WTSTickData* grabLastTick(const char* stdCode) override;

	double getPosition(const char* stdCode, bool validOnly, int32_t flag) override;

	OrderMap* getOrders(const char* stdCode) override;

	double getUndoneQty(const char* stdCode) override;

	OrderIDs buy(const char* stdCode, double price, double qty, bool bForceClose = false) override;

	OrderIDs sell(const char* stdCode, double price, double qty, bool bForceClose = false) override;

	bool cancel(uint32_t localid) override;

	OrderIDs cancel(const char* stdCode, bool isBuy, double qty = 0) override;

	void writeLog(const char* message) override;

	WTSCommodityInfo* getCommodityInfo(const char* stdCode) override;
	WTSSessionInfo* getSessionInfo(const char* stdCode) override;

	uint64_t getCurTime() override;

public:
	bool	init(WTSVariant* cfg);

private:
	HisDataReplayer*	_replayer;

	typedef struct ExecFactInfo
	{
		std::string		_module_path;
		DllHandle		_module_inst;
		IExecuterFactory*	_fact;
		FuncCreateExeFactory	_creator{};
		FuncDeleteExeFactory	_remover{};

		ExecFactInfo()
		{
			_module_inst = nullptr;
			_fact = nullptr;
		}

		~ExecFactInfo()
		{
			if (_fact)
				_remover(_fact);
		}
	} ExecFactInfo;
	ExecFactInfo	_factory;

	ExecuteUnit*	_exec_unit{};
	std::string		_code;
	std::string		_period;
	double			_volunit{};
	int32_t			_volmode{};

	double			_target{};

	double			_position;
	double			_undone;
	WTSTickData*	_last_tick;
	double			_sig_px;
	uint64_t		_sig_time{};

	std::stringstream	_trade_logs;
	uint32_t	_ord_cnt;
	double		_ord_qty;
	uint32_t	_cacl_cnt;
	double		_cacl_qty;
	uint32_t	_sig_cnt;

	std::string	_id;

	MatchEngine	_matcher;
};

