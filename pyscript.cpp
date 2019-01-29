#include "pyscript.h"


PyScript::PyScript(IRuntime *rt) :
	module{NULL}
{
	//PySys_SetPath("Z:\\work\\GHwIf\\Debug\\Lib");

	Log::msg("PyPath  ", Py_GetPath());
	Log::msg("PyScript", "initialize");
	Py_Initialize();

}


PyScript::~PyScript()
{
	Py_Finalize();
}

void PyScript::add(const char * name, foreign_func func)
{
}

pointer PyScript::add(const char * name, double value)
{
	return pointer();
}

void PyScript::eval(std::string & line)
{
}

void PyScript::exec(const char * func)
{
}

void PyScript::load(const char * filename)
{
	PyObject *name;

	name = PyString_FromString(filename);
	if (name == NULL) {
		Log::err("PyString_FromString", "");
		PyErr_Print();
		return;
	}

	module = PyImport_Import(name);
	if (module == NULL) {
	    Log::err("init not loaded", filename);
	    PyErr_Print();
	}
	Py_DECREF(name);
}

void PyScript::version()
{
    Log::msg("version python", PY_VERSION);
}
