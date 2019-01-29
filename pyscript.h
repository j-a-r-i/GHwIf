#pragma once
#include "common.h"
#include "Python.h"

class PyScript :
	public IPluginScript
{
public:
	PyScript(IRuntime *rt);
	virtual ~PyScript();

	void add(const char* name, foreign_func func) override;
	pointer add(const char* name, double value) override;

	void eval(std::string& line);

	void exec(const char* func);

	void load(const char* filename);

	void version();

private:
	PyObject *module;
};


typedef PyScript Script;

