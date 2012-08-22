//
//  NIDAQAnalogInputTask.cpp
//  nidaqtest
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
    int32_t terminalConfig;
    
    switch (termConfig) {
        case TerminalConfigRSE:
            terminalConfig = DAQmx_Val_RSE;
            break;

        case TerminalConfigNRSE:
            terminalConfig = DAQmx_Val_NRSE;
            break;
            
        case TerminalConfigDifferential:
            terminalConfig = DAQmx_Val_Diff;
            break;
            
        default:
            terminalConfig = DAQmx_Val_Cfg_Default;
            break;
    }
    
    int32_t error = DAQmxBaseCreateAIVoltageChan(getHandle(),
                                                 physicalChannel.c_str(),
                                                 NULL,
                                                 terminalConfig,
                                                 minVal,
                                                 maxVal,
                                                 DAQmx_Val_Volts,
                                                 NULL);
    NIDAQError::throwOnFailure(error);
    
    addChannel();
}


int32_t NIDAQAnalogInputTask::read(double timeout,
                                   std::vector<double> &samples,
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
    NIDAQError::throwOnFailure(error);
    
    return sampsPerChanRead;
}


























