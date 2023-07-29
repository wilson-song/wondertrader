#include "WtExecuterFactory.h"

#include "../Share/StdUtils.hpp"
#include "../Share/StrUtil.hpp"
#include "../WTSTools/WTSLogger.h"

#include <boost/filesystem.hpp>
#include <memory>


USING_NS_WTP;

//////////////////////////////////////////////////////////////////////////
//WtExecuterFactory
bool WtExecuterFactory::loadFactories(const char* path)
{
	if (!StdFile::exists(path))
	{
		WTSLogger::error("Directory {} of executer factory not exists", path);
		return false;
	}

	boost::filesystem::path myPath(path);
	boost::filesystem::directory_iterator endIter;
	for (boost::filesystem::directory_iterator iter(myPath); iter != endIter; iter++)
	{
		if (boost::filesystem::is_directory(iter->path()))
			continue;

#ifdef _WIN32
		if (iter->path().extension() != ".dll")
			continue;
#else //_UNIX
		if (iter->path().extension() != ".so")
			continue;
#endif

		const std::string& path = iter->path().string();

		DllHandle hInst = DLLHelper::load_library(path.c_str());
		if (hInst == nullptr)
		{
			continue;
		}

		auto creator = (FuncCreateExeFactory)DLLHelper::get_symbol(hInst, "createExecFact");
		if (creator == nullptr)
		{
			DLLHelper::free_library(hInst);
			continue;
		}

		ExeFactInfo fInfo;
		fInfo._module_inst = hInst;
		fInfo._module_path = iter->path().string();
		fInfo._creator = creator;
		fInfo._remover = (FuncDeleteExeFactory)DLLHelper::get_symbol(hInst, "deleteExecFact");
		fInfo._fact = fInfo._creator();

		_factories[fInfo._fact->getName()] = fInfo;

		WTSLogger::info("Executer factory {} loaded", fInfo._fact->getName());
	}

	return true;
}

ExecuteUnitPtr WtExecuterFactory::createExeUnit(const char* factname, const char* unitname)
{
	auto it = _factories.find(factname);
	if (it == _factories.end())
		return {};

	auto& fInfo = (ExeFactInfo&)it->second;
	ExecuteUnit* unit = fInfo._fact->createExeUnit(unitname);
	if (unit == nullptr)
	{
		WTSLogger::error("Createing execution unit failed: {}.{}", factname, unitname);
		return {};
	}
	return std::make_shared<ExeUnitWrapper>(unit, fInfo._fact);
}

ExecuteUnitPtr WtExecuterFactory::createDiffExeUnit(const char* factname, const char* unitname)
{
	auto it = _factories.find(factname);
	if (it == _factories.end())
		return {};

	auto& fInfo = (ExeFactInfo&)it->second;
	ExecuteUnit* unit = fInfo._fact->createDiffExeUnit(unitname);
	if (unit == nullptr)
	{
		WTSLogger::error("Createing execution unit failed: {}.{}", factname, unitname);
		return {};
	}
	return std::make_shared<ExeUnitWrapper>(unit, fInfo._fact);
}

ExecuteUnitPtr WtExecuterFactory::createExeUnit(const char* name)
{
	StringVector ay = StrUtil::split(name, ".");
	if (ay.size() < 2)
		return {};

	const char* factname = ay[0].c_str();
	const char* unitname = ay[1].c_str();

	auto it = _factories.find(factname);
	if (it == _factories.end())
		return {};

	auto& fInfo = (ExeFactInfo&)it->second;
	ExecuteUnit* unit = fInfo._fact->createExeUnit(unitname);
	if (unit == nullptr)
	{
		WTSLogger::error("Createing execution unit failed: {}", name);
		return {};
	}
	return std::make_shared<ExeUnitWrapper>(unit, fInfo._fact);
}

ExecuteUnitPtr WtExecuterFactory::createDiffExeUnit(const char* name)
{
	StringVector ay = StrUtil::split(name, ".");
	if (ay.size() < 2)
		return {};

	const char* factname = ay[0].c_str();
	const char* unitname = ay[1].c_str();

	auto it = _factories.find(factname);
	if (it == _factories.end())
		return {};

	auto& fInfo = (ExeFactInfo&)it->second;
	ExecuteUnit* unit = fInfo._fact->createDiffExeUnit(unitname);
	if (unit == nullptr)
	{
		WTSLogger::error("Createing execution unit failed: {}", name);
		return {};
	}
	return std::make_shared<ExeUnitWrapper>(unit, fInfo._fact);
}
