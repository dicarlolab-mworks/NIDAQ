//
//  NIDAQAnalogInputTask.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/22/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "NIDAQAnalogInputTask.h"

#include <boost/format.hpp>

#include "NIDAQmxBaseAPI.h"
#include "NIDAQError.h"


NIDAQAnalogInputTask::NIDAQAnalogInputTask(const NIDAQDevice &device) :
    NIDAQTask(device, "AnalogInputTask")
{ }


void NIDAQAnalogInputTask::addVoltageChannel(unsigned int channelNumber,
                                             double minVal,
                                             double maxVal,
                                             TerminalConfig termConfig)
{
    std::string physicalChannel = (boost::format("%s/ai%u") % getDeviceName() % channelNumber).str();
    
    int32_t error = DAQmxBaseCreateAIVoltageChan(getHandle(),
                                                 physicalChannel.c_str(),
                                                 NULL,
                                                 getTerminalConfigValue(termConfig),
                                                 minVal,
                                                 maxVal,
                                                 DAQmx_Val_Volts,
                                                 NULL);
    NIDAQError::throwIfFailed(error);
    
    addChannel(physicalChannel);
}


int32_t NIDAQAnalogInputTask::read(std::vector<double> &samples,
                                   double timeout,
                                   bool interleaved)
{
    int32_t numSampsPerChan = getNumSamplesPerChannel(samples);
    nidaqmxbase::int32_t sampsPerChanRead;
    
    int32_t error = DAQmxBaseReadAnalogF64(getHandle(),
                                           numSampsPerChan,
                                           timeout,
                                           (interleaved ? DAQmx_Val_GroupByScanNumber : DAQmx_Val_GroupByChannel),
                                           &(samples.front()),
                                           samples.size(),
                                           &sampsPerChanRead,
                                           NULL);
    NIDAQError::throwIfFailed(error);
    
    return sampsPerChanRead;
}


int32_t NIDAQAnalogInputTask::getTerminalConfigValue(TerminalConfig termConfig) {
    switch (termConfig) {
        case TerminalConfigRSE:
            return DAQmx_Val_RSE;
            
        case TerminalConfigNRSE:
            return DAQmx_Val_NRSE;
            
        case TerminalConfigDifferential:
            return DAQmx_Val_Diff;
            
        default:
            return DAQmx_Val_Cfg_Default;
    }
}


























