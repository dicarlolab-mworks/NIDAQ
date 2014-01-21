//
//  NIDAQPlugin.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/23/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQDevice.h"


BEGIN_NAMESPACE_MW


class NIDAQPlugin : public Plugin {
    void registerComponents(boost::shared_ptr<ComponentRegistry> registry) MW_OVERRIDE {
        registry->registerFactory<StandardComponentFactory, NIDAQDevice>();
        registry->registerFactory<StandardComponentFactory, NIDAQAnalogInputVoltageChannel>();
        registry->registerFactory<StandardComponentFactory, NIDAQAnalogOutputVoltageWaveformChannel>();
        registry->registerFactory<StandardComponentFactory, NIDAQDigitalInputChannel>();
        registry->registerFactory<StandardComponentFactory, NIDAQDigitalOutputChannel>();
        registry->registerFactory<StandardComponentFactory, NIDAQCounterInputCountEdgesChannel>();
    }
};


extern "C" Plugin* getPlugin() {
    return new NIDAQPlugin();
}


END_NAMESPACE_MW
