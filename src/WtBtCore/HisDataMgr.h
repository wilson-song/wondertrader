#pragma once
#include <functional>
#include "../Includes/IBtDtReader.h"

typedef std::function<void(std::string&)> FuncLoadDataCallback;

NS_WTP_BEGIN
class WTSVariant;
NS_WTP_END

USING_NS_WTP;

class HisDataMgr : public IBtDtReaderSink
{
public:
	HisDataMgr() :_reader(nullptr) {}
	~HisDataMgr()= default;

public:
	void reader_log(WTSLogLevel ll, const char* message) override;

public:
	bool	init(WTSVariant* cfg);

	bool	load_raw_bars(const char* exchg, const char* code, WTSKlinePeriod period, const FuncLoadDataCallback& cb);

	bool	load_raw_ticks(const char* exchg, const char* code, uint32_t uDate, const FuncLoadDataCallback& cb);

private:
	IBtDtReader*	_reader;
};

