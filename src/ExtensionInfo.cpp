#include "ExtensionInfo.h"


const QString ExtensionInfo::TOTAL_KEY = "<<TOTAL>>";


QString ExtensionInfo::parseExtension(const QFileInfo& file)
{
	if (file.isDir())
		return QString{};
	
	return file.suffix().toLower();
}