/*!
 * \file WtDataManager.h
 * \project	WonderTrader
 *
 * \author Wesley
 * \date 2020/03/30
 * 
 * \brief 
 */
#pragma once
#include <vector>
#include "../Includes/IDataReader.h"
#include "../Includes/IDataManager.h"

#include "../Includes/FasterDefs.h"
#include "../Includes/WTSCollection.hpp"

NS_WTP_BEGIN
class WTSVariant;
class WTSTickData;
class WTSKlineSlice;
class WTSTickSlice;
class IBaseDataMgr;
class IBaseDataMgr;
class WtEngine;

class WtDtMgr : public IDataReaderSink, public IDataManager
{
public:
	WtDtMgr();
	~WtDtMgr();

private:
	bool	initStore(WTSVariant* cfg);

public:
	bool	init(WTSVariant* cfg, WtEngine* engine);

	void	register_loader(IHisDataLoader* loader) { _loader = loader; }

	void	handle_push_quote(const char* stdCode, WTSTickData* newTick);

	//////////////////////////////////////////////////////////////////////////
	//IDataManager 接口
	WTSTickSlice* get_tick_slice(const char* stdCode, uint32_t count, uint64_t etime) override;
	WTSOrdQueSlice* get_order_queue_slice(const char* stdCode, uint32_t count, uint64_t etime) override;
	WTSOrdDtlSlice* get_order_detail_slice(const char* stdCode, uint32_t count, uint64_t etime) override;
	WTSTransSlice* get_transaction_slice(const char* stdCode, uint32_t count, uint64_t etime) override;
	WTSKlineSlice* get_kline_slice(const char* stdCode, WTSKlinePeriod period, uint32_t times, uint32_t count, uint64_t etime) override;
	WTSTickData* grab_last_tick(const char* stdCode) override;
	double get_adjusting_factor(const char* stdCode, uint32_t uDate) override;

	uint32_t get_adjusting_flag() override;

	//////////////////////////////////////////////////////////////////////////
	//IDataReaderSink
	void	on_bar(const char* code, WTSKlinePeriod period, WTSBarStruct* newBar) override;
	void	on_all_bar_updated(uint32_t updateTime) override;

	IBaseDataMgr*	get_basedata_mgr() override;
	IHotMgr*		get_hot_mgr() override;
	uint32_t	get_date() override;
	uint32_t	get_min_time()override;
	uint32_t	get_secs() override;

	void		reader_log(WTSLogLevel ll, const char* message) override;

	inline IDataReader*	reader() { return _reader; }
	inline IHisDataLoader*	loader() { return _loader; }

private:
	IDataReader*	_reader;
	IHisDataLoader*	_loader;
	WtEngine*		_engine;

	faster_hashset<LongKey> _subed_basic_bars;
	typedef WTSHashMap<LongKey> DataCacheMap;
	DataCacheMap*	_bars_cache;	//K线缓存
	DataCacheMap*	_rt_tick_map;	//实时tick缓存
	//By Wesley @ 2022.02.11
	//这个只有后复权tick数据
	//因为前复权和不复权，都不需要缓存
	DataCacheMap*	_ticks_adjusted;	//复权tick缓存

	typedef struct _NotifyItem
	{
		char		_code[MAX_INSTRUMENT_LENGTH];
		char		_period[2] = { 0 };
		uint32_t	_times;
		WTSBarStruct* _newBar;

		_NotifyItem(const char* code, char period, uint32_t times, WTSBarStruct* newBar)
			: _times(times), _newBar(newBar)
		{
			wt_strcpy(_code, code);
			_period[0] = period;
		}
	} NotifyItem;

	std::vector<NotifyItem> _bar_notifies;
};

NS_WTP_END