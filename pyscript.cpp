#include "pyscript.h"


PyScript::PyScript(IRuntime *rt) :
	module{NULL}
{
	//PySys_SetPath("Z:\\work\\GHwIf\\Debug\\Lib");
	Py_VerboseFlag++;
	Py_NoSiteFlag++;

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
	PyRun_SimpleStringFlags(line.c_str(), NULL);
}

void PyScript::exec(const char * func)
{
	PyObject *fn = PyObject_GetAttrString(module, func);

	if (fn && PyCallable_Check(fn)) {
		PyObject *value = PyObject_CallObject(fn, NULL);
		if (value != NULL) {
			Log::msg("pycall", "return value");
			Py_DECREF(value);
		}
		Py_DECREF(fn);
	}
	else {
		if (PyErr_Occurred())
			PyErr_Print();
		Log::err("can not find python function", func);
	}
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
