/*!
 * /file WtRunner.cpp
 * /project	WonderTrader
 *
 * /author Wesley
 * /date 2020/03/30
 * 
 * /brief 
 */
#include "WtRunner.h"

#include "../WtCore/WtHelper.h"
#include "../WtCore/CtaStraContext.h"
#include "../WtCore/HftStraContext.h"
#include "../WtCore/WtDiffExecuter.h"

#include "../Includes/WTSVariant.hpp"
#include "../WTSTools/WTSLogger.h"
#include "../WTSUtils/WTSCfgLoader.h"
#include "../Share/StrUtil.hpp"


const char* getBinDir()
{
	static std::string basePath;
	if (basePath.empty())
	{
		basePath = boost::filesystem::initial_path<boost::filesystem::path>().string();

		basePath = StrUtil::standardisePath(basePath);
	}

	return basePath.c_str();
}



WtRunner::WtRunner()
	: _data_store(nullptr)
	, _is_hft(false)
	, _is_sel(false)
{
#if _WIN32
#pragma message("Signal hooks disabled in WIN32")
#else
#pragma message("Signal hooks enabled in UNIX")
	install_signal_hooks([](const char* message) {
		WTSLogger::error(message);
	});
#endif
}


WtRunner::~WtRunner() = default;

bool WtRunner::init()
{
	std::string path = "logcfg.json";
	if(!StdFile::exists(path.c_str()))
		path = "logcfg.yaml";
	WTSLogger::init(path.c_str());

	WtHelper::setInstDir(getBinDir());

	return true;
}

bool WtRunner::config()
{
	std::string cfgFile = "config.json";
	if (!StdFile::exists(cfgFile.c_str()))
		cfgFile = "config.yaml";

	_config = WTSCfgLoader::load_from_file(cfgFile);
	if(_config == nullptr)
	{
		WTSLogger::error("Loading config file {} failed", cfgFile);
		return false;
	}

	//基础数据文件
	WTSVariant* cfgBF = _config->get("basefiles");
	if (cfgBF->get("session"))
		_bd_mgr.loadSessions(cfgBF->getCString("session"));

	WTSVariant* cfgItem = cfgBF->get("commodity");
	if (cfgItem)
	{
		if (cfgItem->type() == WTSVariant::VT_String)
		{
			_bd_mgr.loadCommodities(cfgItem->asCString());
		}
		else if (cfgItem->type() == WTSVariant::VT_Array)
		{
			for (uint32_t i = 0; i < cfgItem->size(); i++)
			{
				_bd_mgr.loadCommodities(cfgItem->get(i)->asCString());
			}
		}
	}

	cfgItem = cfgBF->get("contract");
	if (cfgItem)
	{
		if (cfgItem->type() == WTSVariant::VT_String)
		{
			_bd_mgr.loadContracts(cfgItem->asCString());
		}
		else if (cfgItem->type() == WTSVariant::VT_Array)
		{
			for (uint32_t i = 0; i < cfgItem->size(); i++)
			{
				_bd_mgr.loadContracts(cfgItem->get(i)->asCString());
			}
		}
	}

	if (cfgBF->get("holiday"))
		_bd_mgr.loadHolidays(cfgBF->getCString("holiday"));

	if (cfgBF->get("hot"))
		_hot_mgr.loadHots(cfgBF->getCString("hot"));

	if (cfgBF->get("second"))
		_hot_mgr.loadSeconds(cfgBF->getCString("second"));

	if (cfgBF->has("rules"))
	{
		auto cfgRules = cfgBF->get("rules");
		auto tags = cfgRules->memberNames();
		for (const std::string& ruleTag : tags)
		{
			_hot_mgr.loadCustomRules(ruleTag.c_str(), cfgRules->getCString(ruleTag.c_str()));
			WTSLogger::info("{} rules loaded from {}", ruleTag, cfgRules->getCString(ruleTag.c_str()));
		}
	}

	//初始化运行环境
	initEngine();

	//初始化数据管理
	initDataMgr();

	if (!initActionPolicy())
		return false;

	//初始化行情通道
	WTSVariant* cfgParser = _config->get("parsers");
	if (cfgParser)
	{
		if (cfgParser->type() == WTSVariant::VT_String)
		{
			const char* filename = cfgParser->asCString();
			if (StdFile::exists(filename))
			{
				WTSLogger::info("Reading parser config from {}...", filename);
				WTSVariant* var = WTSCfgLoader::load_from_file(filename);
				if(var)
				{
					if (!initParsers(var->get("parsers")))
						WTSLogger::error("Loading parsers failed");
					var->release();
				}
				else
				{
					WTSLogger::error("Loading parser config {} failed", filename);
				}
			}
			else
			{
				WTSLogger::error("Parser configuration {} not exists", filename);
			}
		}
		else if (cfgParser->type() == WTSVariant::VT_Array)
		{
			initParsers(cfgParser);
		}
	}

	//初始化交易通道
	WTSVariant* cfgTraders = _config->get("traders");
	if (cfgTraders)
	{
		if (cfgTraders->type() == WTSVariant::VT_String)
		{
			const char* filename = cfgTraders->asCString();
			if (StdFile::exists(filename))
			{
				WTSLogger::info("Reading trader config from {}...", filename);
				WTSVariant* var = WTSCfgLoader::load_from_file(filename);
				if (var)
				{
					if (!initTraders(var->get("traders")))
						WTSLogger::error("Loading traders failed");
					var->release();
				}
				else
				{
					WTSLogger::error("Loading trader config {} failed", filename);
				}
			}
			else
			{
				WTSLogger::error("Trader configuration {} not exists", filename);
			}
		}
		else if (cfgTraders->type() == WTSVariant::VT_Array)
		{
			initTraders(cfgTraders);
		}
	}

	initEvtNotifier();

	//如果不是高频引擎,则需要配置执行模块
	if (!_is_hft)
	{
		WTSVariant* cfgExec = _config->get("executers");
		if (cfgExec != nullptr)
		{
			if (cfgExec->type() == WTSVariant::VT_String)
			{
				const char* filename = cfgExec->asCString();
				if (StdFile::exists(filename))
				{
					WTSLogger::info("Reading executor config from {}...", filename);
					WTSVariant* var = WTSCfgLoader::load_from_file(filename);
					if (var)
					{
						if (!initExecutors(var->get("executers")))
							WTSLogger::error("Loading executors failed");

						WTSVariant* c = var->get("routers");
						if (c != nullptr)
							_cta_engine.loadRouterRules(c);

						var->release();
					}
					else
					{
						WTSLogger::error("Loading executor config {} failed", filename);
					}
				}
				else
				{
					WTSLogger::error("Trader configuration {} not exists", filename);
				}
			}
			else if (cfgExec->type() == WTSVariant::VT_Array)
			{
                initExecutors(cfgExec);
			}
		}

		WTSVariant* cfgRouter = _config->get("routers");
		if (cfgRouter != nullptr)
			_cta_engine.loadRouterRules(cfgRouter);
	}

	if (!_is_hft)
		initCtaStrategies();
	else
		initHftStrategies();
	
	return true;
}

bool WtRunner::initCtaStrategies()
{
	WTSVariant* cfg = _config->get("strategies");
	if (cfg == nullptr || cfg->type() != WTSVariant::VT_Object)
		return false;

	cfg = cfg->get("cta");
	if (cfg == nullptr || cfg->type() != WTSVariant::VT_Array)
		return false;

	std::string path = WtHelper::getCWD() + "cta/";
	_cta_strategy_mgr.loadFactories(path.c_str());

	for (uint32_t idx = 0; idx < cfg->size(); idx++)
	{
		WTSVariant* cfgItem = cfg->get(idx);
		if (!cfgItem->getBoolean("active"))
			continue;

		const char* id = cfgItem->getCString("id");
		const char* name = cfgItem->getCString("name");
		int32_t slippage = cfgItem->getInt32("slippage");
		CtaStrategyPtr stra = _cta_strategy_mgr.createStrategy(name, id);
		stra->self()->init(cfgItem->get("params"));
		auto* ctx = new CtaStraContext(&_cta_engine, id, slippage);
		ctx->set_strategy(stra->self());
		_cta_engine.addContext(CtaContextPtr(ctx));
	}

	return true;
}

bool WtRunner::initHftStrategies()
{
	WTSVariant* cfg = _config->get("strategies");
	if (cfg == nullptr || cfg->type() != WTSVariant::VT_Object)
		return false;

	cfg = cfg->get("hft");
	if (cfg == nullptr || cfg->type() != WTSVariant::VT_Array)
		return false;

	std::string path = WtHelper::getCWD() + "hft/";
	_hft_strategy_mgr.loadFactories(path.c_str());

	for (uint32_t idx = 0; idx < cfg->size(); idx++)
	{
		WTSVariant* cfgItem = cfg->get(idx);
		if (!cfgItem->getBoolean("active"))
			continue;

		const char* id = cfgItem->getCString("id");
		const char* name = cfgItem->getCString("name");
		bool agent = cfgItem->getBoolean("agent");
		int32_t slippage = cfgItem->getInt32("slippage");
		HftStrategyPtr strategy = _hft_strategy_mgr.createStrategy(name, id);
		if (strategy == nullptr)
			continue;

		strategy->self()->init(cfgItem->get("params"));
		auto* ctx = new HftStraContext(&_hft_engine, id, agent,slippage);
		ctx->set_strategy(strategy->self());

		const char* traderID = cfgItem->getCString("trader");
		TraderAdapterPtr trader = _traders.getAdapter(traderID);
		if(trader)
		{
			ctx->setTrader(trader.get());
			trader->addSink(ctx);
		}
		else
		{
			WTSLogger::error("Trader {} not exists, binding trader to HFT strategy failed", traderID);
		}

		_hft_engine.addContext(HftContextPtr(ctx));
	}

	return true;
}


bool WtRunner::initEngine()
{
	WTSVariant* cfg = _config->get("env");
	if (cfg == nullptr)
		return false;

	const char* name = cfg->getCString("name");
	
	if (strlen(name) == 0 || wt_stricmp(name, "cta") == 0)
	{
		_is_hft = false;
		_is_sel = false;
	}
	else if (wt_stricmp(name, "sel") == 0)
	{
		_is_sel = true;
	}
	else //if (wt_stricmp(name, "hft") == 0)
	{
		_is_hft = true;
	}

	if (_is_hft)
	{
		WTSLogger::info("Trading environment initialized with engine: HFT");
		_hft_engine.init(cfg, &_bd_mgr, &_data_mgr, &_hot_mgr, &_notifier);
		_engine = &_hft_engine;
	}
	else if (_is_sel)
	{
		WTSLogger::info("Trading environment initialized with engine: SEL");
		_sel_engine.init(cfg, &_bd_mgr, &_data_mgr, &_hot_mgr, &_notifier);
		_engine = &_sel_engine;
	}
	else
	{
		WTSLogger::info("Trading environment initialized with engine: CTA");
		_cta_engine.init(cfg, &_bd_mgr, &_data_mgr, &_hot_mgr, &_notifier);
		_engine = &_cta_engine;
	}

	_engine->set_adapter_mgr(&_traders);

	return true;
}

bool WtRunner::initActionPolicy()
{
	return _act_policy.init(_config->getCString("bspolicy"));
}

bool WtRunner::initDataMgr()
{
	WTSVariant*cfg = _config->get("data");
	if (cfg == nullptr)
		return false;

	_data_mgr.init(cfg, _engine);
	WTSLogger::info("Data manager initialized");

	return true;
}

bool WtRunner::initParsers(WTSVariant* cfgParser)
{
	if (cfgParser == nullptr)
		return false;

	uint32_t count = 0;
	for (uint32_t idx = 0; idx < cfgParser->size(); idx++)
	{
		WTSVariant* cfgItem = cfgParser->get(idx);
		if(!cfgItem->getBoolean("active"))
			continue;

		const char* id = cfgItem->getCString("id");
		// By Wesley @ 2021.12.14
		// 如果id为空，则生成自动id
		std::string realID = id;
		if (realID.empty())
		{
			static uint32_t auto_parser_id = 1000;
            realID = StrUtil::printf("auto_parser_%u", auto_parser_id++);
		}

		ParserAdapterPtr adapter(new ParserAdapter);
		adapter->init(realID.c_str(), cfgItem, _engine, &_bd_mgr, &_hot_mgr);
		_parsers.addAdapter(realID.c_str(), adapter);

		count++;
	}

	WTSLogger::info("{} parsers loaded", count);
	return true;
}

bool WtRunner::initExecutors(WTSVariant* cfgExecutor)
{
	if (cfgExecutor == nullptr || cfgExecutor->type() != WTSVariant::VT_Array)
		return false;

	std::string path = WtHelper::getCWD() + "executer/";
	_exe_factory.loadFactories(path.c_str());

	uint32_t count = 0;
	for (uint32_t idx = 0; idx < cfgExecutor->size(); idx++)
	{
		WTSVariant* cfgItem = cfgExecutor->get(idx);
		if (!cfgItem->getBoolean("active"))
			continue;

		const char* id = cfgItem->getCString("id");
		std::string name = cfgItem->getCString("name");	//local,diff,dist
		if (name.empty())
			name = "local";

		if (name == "local")
		{
			auto* executor = new WtLocalExecuter(&_exe_factory, id, &_data_mgr);
			if (!executor->init(cfgItem))
				return false;

			const char* tid = cfgItem->getCString("trader");
			if (strlen(tid) == 0)
			{
				WTSLogger::error("No Trader configured for Executor {}", id);
			}
			else
			{
				TraderAdapterPtr trader = _traders.getAdapter(tid);
				if (trader)
				{
					executor->setTrader(trader.get());
					trader->addSink(executor);
				}
				else
				{
					WTSLogger::error("Trader {} not exists, cannot configured for executor %s", tid, id);
				}
			}

            _cta_engine.addExecutor(ExecCmdPtr(executor));
		}
		else if (name == "diff")
		{
			auto* executor = new WtDiffExecuter(&_exe_factory, id, &_data_mgr, &_bd_mgr);
			if (!executor->init(cfgItem))
				return false;

			const char* tid = cfgItem->getCString("trader");
			if (strlen(tid) == 0)
			{
				WTSLogger::error("No Trader configured for Executor {}", id);
			}
			else
			{
				TraderAdapterPtr trader = _traders.getAdapter(tid);
				if (trader)
				{
					executor->setTrader(trader.get());
					trader->addSink(executor);
				}
				else
				{
					WTSLogger::error("Trader {} not exists, cannot configured for executor %s", tid, id);
				}
			}

            _cta_engine.addExecutor(ExecCmdPtr(executor));
		}
		else
		{
			auto* executor = new WtDistExecuter(id);
			if (!executor->init(cfgItem))
				return false;

            _cta_engine.addExecutor(ExecCmdPtr(executor));
		}
		count++;
	}

	WTSLogger::info("{} executors loaded", count);

	return true;
}

bool WtRunner::initTraders(WTSVariant* cfgTrader)
{
	if (cfgTrader == nullptr || cfgTrader->type() != WTSVariant::VT_Array)
		return false;
	
	uint32_t count = 0;
	for (uint32_t idx = 0; idx < cfgTrader->size(); idx++)
	{
		WTSVariant* cfgItem = cfgTrader->get(idx);
		if (!cfgItem->getBoolean("active"))
			continue;

		const char* id = cfgItem->getCString("id");

		TraderAdapterPtr adapter(new TraderAdapter(&_notifier));
		adapter->init(id, cfgItem, &_bd_mgr, &_act_policy);

		_traders.addAdapter(id, adapter);

		count++;
	}

	WTSLogger::info("{} traders loaded", count);

	return true;
}

void WtRunner::run(bool bAsync /* = false */)
{
	try
	{
		_parsers.run();
		_traders.run();

		_engine->run(bAsync);
	}
	catch (...)
	{
		//print_stack_trace([](const char* message) {
		//	WTSLogger::error(message);
		//});
	}
}

const char* LOG_TAGS[] = {
	"all",
	"debug",
	"info",
	"warn",
	"error",
	"fatal",
	"none"
};

void WtRunner::handleLogAppend(WTSLogLevel ll, const char* msg)
{
	_notifier.notify_log(LOG_TAGS[ll - 100], msg);
}

bool WtRunner::initEvtNotifier()
{
	WTSVariant* cfg = _config->get("notifier");
	if (cfg == nullptr || cfg->type() != WTSVariant::VT_Object)
		return false;

	_notifier.init(cfg);

	return true;
}