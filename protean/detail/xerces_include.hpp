#ifndef PROTEAN_DETAIL_XERCES_INCLUDE_HPP
#define PROTEAN_DETAIL_XERCES_INCLUDE_HPP

#ifndef PROTEAN_DISABLE_XML

#include <protean/config.hpp>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax/InputSource.hpp>
#include <xercesc/util/BinInputStream.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/framework/psvi/PSVIAttribute.hpp>
#include <xercesc/framework/psvi/PSVIAttributeList.hpp>
#include <xercesc/framework/psvi/PSVIElement.hpp>
#include <xercesc/framework/psvi/PSVIHandler.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/parsers/SAX2XMLReaderImpl.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/Base64.hpp>

#endif // #ifndef PROTEAN_DISABLE_XML

#endif // PROTEAN_DETAIL_XERCES_INCLUDE_HPP
