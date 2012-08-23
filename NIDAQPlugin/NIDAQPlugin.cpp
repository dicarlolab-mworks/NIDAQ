//
//  NIDAQPlugin.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/23/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include <MWorksCore/Plugin.h>
#include <MWorksCore/StandardComponentFactory.h>

using namespace mw;


class NIDAQPlugin : public Plugin {
    void registerComponents(shared_ptr<ComponentRegistry> registry) MW_OVERRIDE {
        //registry->registerFactory<StandardComponentFactory, NIDAQIODevice>();
    }
};


extern "C" Plugin* getPlugin() {
    return new NIDAQPlugin();
}
