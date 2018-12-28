#include <tidy/tidy.h>
#include <string.h>
#include "htmlparse.h"
#include "common.h"

void HtmlParse::begin()
{
    tidyBufInit(&tbuffer);
}

void HtmlParse::parse(const char* str)
{
    tidyBufAppend(&tbuffer, (void*)str, strlen(str));
}

void HtmlParse::end()
{
    TidyDoc doc;
    TidyBuffer errBuf;
    int err;

    tidyBufInit(&errBuf);

    doc = tidyCreate();
    tidyOptSetBool    (doc, TidyForceOutput, yes);
    tidyOptSetInt     (doc, TidyWrapLen,      4096);
    tidySetErrorBuffer(doc, &errBuf);

    try {
	err = tidyParseBuffer(doc, &tbuffer);
	if (err < 0) {
	    throw TheException(TheException::ETidyParseBuffer);
	}
	
	err = tidyCleanAndRepair(doc);
	if (err < 0) {
	    throw TheException(TheException::ETidyCleanNRepair);
	}

	err = tidyRunDiagnostics(doc);
	if (err < 0) {
	    throw TheException(TheException::ETidyRunDiagnostics);
	}

	printNode(tidyGetRoot(doc), 0);
    }
    catch (TheException &e) {
	Log::err(__FUNCTION__, e.what());
    }

    tidyBufFree(&errBuf);
    tidyBufFree(&tbuffer);
    tidyRelease(doc);
}

void HtmlParse::printNode(TidyNode node, int level)
{
    for (TidyNode child = tidyGetChild(node);
	 child;
	 child = tidyGetNext(child))
    {
	ctmbstr name;
	switch (tidyNodeGetType(child)) {
	case TidyNode_Root:       name = "Root";                    break;
	case TidyNode_DocType:    name = "DOCTYPE";                 break;
	case TidyNode_Comment:    name = "Comment";                 break;
	case TidyNode_ProcIns:    name = "Processing Instruction";  break;
	case TidyNode_Text:       name = "Text";                    break;
	case TidyNode_CDATA:      name = "CDATA";                   break;
	case TidyNode_Section:    name = "XML Section";             break;
	case TidyNode_Asp:        name = "ASP";                     break;
	case TidyNode_Jste:       name = "JSTE";                    break;
	case TidyNode_Php:        name = "PHP";                     break;
	case TidyNode_XmlDecl:    name = "XML Declaration";         break;

	case TidyNode_Start:
	case TidyNode_End:
	case TidyNode_StartEnd:
	default:
	    name = tidyNodeGetName( child );
	    if (strcmp(name, "a") == 0) {
		for (TidyAttr attr = tidyAttrFirst(child);
		     attr;
		     attr = tidyAttrNext(attr))
		{
		    ctmbstr aname = tidyAttrName(attr);

		    if (strcmp(aname, "href") == 0) {
			printf("%s = %s\n",
			       tidyAttrName(attr),
			       tidyAttrValue(attr));
		    }
		}
	    }
	    break;
	}
	assert( name != NULL );

	printNode(child, level+1);	
    }
}

const char* HtmlParse::result()
{
}
