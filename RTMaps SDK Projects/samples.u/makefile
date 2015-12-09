# RTMaps Makefile
# Copyright 2002-2012 (C) Intempora S.A.

ROOTDIR  := $(realpath $(CURDIR)/..)
ifeq ($(ROOTDIR),)
$(error Please use make at least version 3.81)
endif

sources = maps_CANReader.cpp \
          maps_CANWriter.cpp \
          maps_ComponentWithActions.cpp \
          maps_counter.cpp \
          maps_customclock.cpp \
          maps_DynamicInputs.cpp \
          maps_enum_props.cpp \
          maps_FusionSampleWithMultipleStartReading.cpp \
          maps_FusionSampleWithSamplingReaders.cpp \
          maps_ImageGenerator.cpp \
          maps_multiplier_1.cpp \
          maps_multiplier_2.cpp \
          maps_multiplier_3.cpp \
          maps_multiplier_4.cpp \
          maps_MultiThread.cpp \
          maps_MultiThreadAndSynchronisation.cpp \
          maps_RRMUserStructure.cpp \
          maps_SetProperty_Overload.cpp \
          maps_simple_imageprocessing.cpp \
          maps_stream_decoder_sample.cpp \
          maps_synchronizercomponent.cpp \
          maps_Threshold.cpp \
          maps_UserStructureInput.cpp \
          maps_UserStructureOutput.cpp \
          maps_vectorizer_sample_0_ERROR_TO_BANISH.cpp \
          maps_vectorizer_sample_1_triggered.cpp \
          maps_vectorizer_sample_2_reactive.cpp \
          maps_vectorizer_sample_3_resampling.cpp \
          maps_vectorizer_sample_4_resynchronizer.cpp \
          maps_vectorizer_sample_5_generic.cpp

PCK = rtmaps_samples.pck

all: pck

RTMAPS_SDKDIR ?=/opt/rtmaps
include $(RTMAPS_SDKDIR)/templates.u/makefile.inc
