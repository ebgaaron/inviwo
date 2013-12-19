/**********************************************************************
 * Copyright (C) 2012-2013 Scientific Visualization Group - Link�ping University
 * All Rights Reserved.
 * 
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * No part of this software may be reproduced or transmitted in any
 * form or by any means including photocopying or recording without
 * written permission of the copyright owner.
 *
 * Primary author : Erik Sund�n
 *
 **********************************************************************/

#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/processors/processorfactory.h>
#include <inviwo/core/processors/processorwidgetfactory.h>
#include <inviwo/core/properties/optionproperties.h>
#include <inviwo/core/metadata/metadatafactory.h>
#include <inviwo/core/datastructures/representationconverterfactory.h>
#include <inviwo/core/io/datareaderfactory.h>
#include <inviwo/core/io/datawriterfactory.h>

//#define MEMLEAKCHECK

#ifdef MEMLEAKCHECK
#ifdef WIN32
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif
#endif
#endif


namespace inviwo {

// TODO: are the first two constructors needed? Otherwise remove.
InviwoApplication::InviwoApplication()
    : displayName_("Inviwo"), basePath_("")
{
    init(this);
}

InviwoApplication::InviwoApplication(std::string displayName, std::string basePath)
                                     : displayName_(displayName), basePath_(basePath)
{
    init(this);
}

InviwoApplication::InviwoApplication(int argc, char** argv, std::string displayName, std::string basePath)
                                    : displayName_(displayName), basePath_(basePath)
{
    commandLineParser_ = new CommandLineParser(argc, argv);
	commandLineParser_->initialize();
	commandLineParser_->parse();
    init(this);
}

InviwoApplication::~InviwoApplication() {
#ifdef MEMLEAKCHECK
#ifdef WIN32
#ifdef _DEBUG
    _CrtDumpMemoryLeaks();
#endif
#endif
#endif
}

void InviwoApplication::initialize(registerModuleFuncPtr regModuleFunc) {
    settings_ = new Settings();
    settings_->initialize();

    printApplicationInfo();

    registerModule(new InviwoCore());
    (*regModuleFunc)(this);
    for (size_t i=0; i<modules_.size(); i++){
        modules_[i]->initialize();
        modules_[i]->setGlobalSettings(settings_);
    }

    // initialize singleton factories
    ProcessorFactory::init();
    MetaDataFactory::init();
    ProcessorWidgetFactory::init();
    RepresentationConverterFactory::init();
    DataReaderFactory::init();
    DataWriterFactory::init();

    initialized_ = true;
}

void InviwoApplication::deinitialize() {
    for (size_t i=0; i<modules_.size(); i++)
        modules_[i]->deinitialize();
    initialized_ = false;
}

std::string InviwoApplication::getPath(PathType pathType, const std::string& suffix) {
    std::string result = getBasePath();
    if (pathType == InviwoApplication::PATH_PROJECT)
        result += "data/workspaces/";
    else if (pathType == InviwoApplication::PATH_DATA)
        result += "data/";
    else if (pathType == InviwoApplication::PATH_VOLUMES)
        result += "data/volumes/";
    else if (pathType == InviwoApplication::PATH_IMAGES)
        result += "data/images/";
    else if (pathType == InviwoApplication::PATH_MODULES)
        result += "modules/";
    result += suffix;
    return result;
}

void InviwoApplication::printApplicationInfo(){
    LogInfoCustom("InviwoInfo", "Inviwo Version: " << IVW_VERSION);
    std::string config = "";
#ifdef CMAKE_GENERATOR
    config += std::string(CMAKE_GENERATOR);
#endif
#if defined(CMAKE_BUILD_TYPE)
    config += " [" + std::string(CMAKE_BUILD_TYPE) + "]";
#elif defined(CMAKE_INTDIR)
    config += " [" + std::string(CMAKE_INTDIR) + "]";
#endif
    if (config != "")
        LogInfoCustom("InviwoInfo", "Config: " << config);
}

void InviwoApplication::setPropertyVisibilityMode(PropertyVisibilityMode viewMode){
    dynamic_cast<OptionPropertyInt*>(getSettings()->getPropertyByIdentifier("viewMode"))->set(static_cast<int>(viewMode));
}

PropertyVisibilityMode InviwoApplication::getPropertyVisibilityMode(){
    return static_cast<PropertyVisibilityMode>(dynamic_cast<OptionPropertyInt*>(getSettings()->getPropertyByIdentifier("viewMode"))->get());
}

void InviwoApplication::addCallbackAction(ModuleCallbackAction* callbackAction) {
    moudleCallbackActions_.push_back(callbackAction);
}

std::vector<ModuleCallbackAction*> InviwoApplication::getCallbackActions() {
    return moudleCallbackActions_;
}

} // namespace
