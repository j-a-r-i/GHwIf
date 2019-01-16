#include "common.h"
#include "scmscript.h"

InfoItemReal::InfoItemReal(IPluginScript& scm, const char* n) :
	InfoItem(n)
{
	scmValue = scm.add(n, 0.0);
}
