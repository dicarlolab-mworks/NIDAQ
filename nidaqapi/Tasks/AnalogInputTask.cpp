//
//  AnalogInputTask.cpp
//  NIDAQ
//
//  Created by Christopher Stawarz on 8/22/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#include "AnalogInputTask.h"

#include "NIDAQmxBaseAPI.h"
#include "Error.h"


BEGIN_NAMESPACE_NIDAQ


AnalogInputTask::AnalogInputTask(const Device &device) :
    Task(device, "AnalogInput")
{ }


void AnalogInputTask::addVoltageChannel(unsigned int channelNumber,
                                        double minVal,
                                        double maxVal,
                                        TerminalConfig termConfig)
{
    std::string physicalChannel = getChannelName("ai", channelNumber);
    
    int32_t error = DAQmxBaseCreateAIVoltageChan(getHandle(),
                                                 physicalChannel.c_str(),
                                                 NULL,
                                                 getTerminalConfigValue(termConfig),
                                                 minVal,
                                                 maxVal,
                                                 DAQmx_Val_Volts,
                                                 NULL);
    Error::throwIfFailed(error);
    
    Task::addChannel();
}


size_t AnalogInputTask::getNumSamplesAvailable() const {
    nidaqmxbase::uint32_t sampsPerChanAvail;
    Error::throwIfFailed(  DAQmxBaseGetReadAttribute(getHandle(), DAQmx_Read_AvailSampPerChan, &sampsPerChanAvail)  );
    return size_t(sampsPerChanAvail) * getNumChannels();
}


size_t AnalogInputTask::read(double &firstSample,
                             size_t numSamples,
                             double timeout,
                             bool interleaved)
{
    int32_t numSampsPerChan = getNumSamplesPerChannel(numSamples);
    nidaqmxbase::int32_t sampsPerChanRead = 0;
    
    int32_t error = DAQmxBaseReadAnalogF64(getHandle(),
                                           numSampsPerChan,
                                           timeout,
                                           (interleaved ? DAQmx_Val_GroupByScanNumber : DAQmx_Val_GroupByChannel),
                                           &firstSample,
                                           numSamples,
                                           &sampsPerChanRead,
                                           NULL);
    
    // Don't throw if we did a partial read (not sure this ever happens, but just to be safe ...)
    if ((error != DAQmxErrorSamplesNotYetAvailable) || (sampsPerChanRead == 0)) {
        Error::throwIfFailed(error);
    }
    
    return size_t(sampsPerChanRead) * getNumChannels();
}


int32_t AnalogInputTask::getTerminalConfigValue(TerminalConfig termConfig) {
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


END_NAMESPACE_NIDAQ


























