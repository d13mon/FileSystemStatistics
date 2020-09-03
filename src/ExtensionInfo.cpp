#include "ExtensionInfo.h"


QString ExtensionInfo::parseExtension(const QFileInfo& file)
{
	if (file.isDir())
		return QString{};
	
	return file.suffix().toLower();
}