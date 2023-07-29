#pragma once
#include "../Includes/IParserApi.h"

USING_NS_WTP;

class ExpParser : public IParserApi
{
public:
	explicit ExpParser(const char* id):_id(id){}
	~ExpParser() override= default;

public:
	bool init(WTSVariant* config) override;


	void release() override;


	bool connect() override;


	bool disconnect() override;


	bool isConnected() override { return true; }


	void subscribe(const CodeSet& setCodes) override;


	void unsubscribe(const CodeSet& setCodes) override;


	void registerSpi(IParserSpi* listener) override;

private:
	std::string			_id;
	IParserSpi*			m_sink{};
	IBaseDataMgr*		m_pBaseDataMgr{};
};

