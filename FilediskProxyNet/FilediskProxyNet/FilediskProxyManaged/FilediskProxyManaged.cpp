#pragma once
#include "pch.h"
#include <stdint.h>
#include "FilediskProxyManaged.h"

using namespace System;
using namespace std;

namespace FilediskProxyManaged {

	FilediskProxyManaged::FilediskProxyManaged()
	{
		this->_nativePtr = new FilediskProxyNative::FilediskProxyNative();
	}

	BOOL FilediskProxyManaged::init_ctx(UCHAR DriveLetter, size_t filesize, OUT int64_t% ctxOut)
	{
		// Initialize Software AES Framework for the CBC mode
		int64_t ctx = 0;
		BOOL result = this->_nativePtr->init_ctx(DriveLetter, filesize, ctx);
		ctxOut = ctx;
		return result;
	}

    BOOL FilediskProxyManaged::delete_ctx(int64_t ctxref)
    {
        return this->_nativePtr->delete_ctx(ctxref);
    }

    // set/reset Event
    void FilediskProxyManaged::SetEventDriverRequestDataSet(int64_t ctxref, BOOL set)
    {
        this->_nativePtr->SetEventDriverRequestDataSet(ctxref, set);
    }

    // set/reset Event
    void FilediskProxyManaged::SetEventProxyIdle(int64_t ctxref, BOOL set)
    {
        this->_nativePtr->SetEventProxyIdle(ctxref, set);
    }

    // set/reset Event
    void FilediskProxyManaged::SetEventRequestComplete(int64_t ctxref, BOOL set)
    {
        this->_nativePtr->SetEventRequestComplete(ctxref, set);
    }

    // notifies the windows about drive add or remove
    void FilediskProxyManaged::NotifyWindows(int64_t ctxref, BOOL DriveAdded)
    {
        this->_nativePtr->NotifyWindows(ctxref, DriveAdded);
    }

    uint32_t FilediskProxyManaged::WaitEventDriverRequestDataSet(int64_t ctxref, DWORD miliSeconds)
    {
        return this->_nativePtr->WaitEventDriverRequestDataSet(ctxref, miliSeconds);

    }
    uint32_t FilediskProxyManaged::WaitEventProxyIdle(int64_t ctxref, DWORD miliSeconds)
    {
        return this->_nativePtr->WaitEventProxyIdle(ctxref, miliSeconds);

    }
    uint32_t FilediskProxyManaged::WaitEventRequestComplete(int64_t ctxref, DWORD miliSeconds)
    {
        return this->_nativePtr->WaitEventRequestComplete(ctxref, miliSeconds);

    }

    void FilediskProxyManaged::GetSHMHeader(int64_t ctxref, OUT int64_t% byteOffset, OUT DWORD% length, OUT UCHAR% function, OUT DWORD% totalBytesReadWrite)
    {
        int64_t byteOffsettmp = 0;
        DWORD lengthtmp = 0;
        UCHAR functiontmp = 0;
        DWORD totalbytesreadwritetmp = 0;
        this->_nativePtr->GetSHMHeader(ctxref, byteOffsettmp, lengthtmp, functiontmp, totalbytesreadwritetmp);
        byteOffset = byteOffsettmp;
        length = lengthtmp;
        function = functiontmp;
        totalBytesReadWrite = totalbytesreadwritetmp;
    }

    void FilediskProxyManaged::SetSHMHeader(int64_t ctxref, int64_t byteOffset, DWORD length, UCHAR function, DWORD totalBytesReadWrite)
    {
        this->_nativePtr->SetSHMHeader(ctxref, byteOffset, length, function, totalBytesReadWrite);
    }

    void FilediskProxyManaged::GetSHMBuffer(int64_t ctxref, int64_t byteOffset, DWORD length, int64_t outputBuffer)
    {
        this->_nativePtr->GetSHMBuffer(ctxref, byteOffset, length, (void*)outputBuffer);
    }

    void FilediskProxyManaged::SetSHMBuffer(int64_t ctxref, int64_t byteOffset, DWORD length, int64_t inputBuffer)
    {
        this->_nativePtr->SetSHMBuffer(ctxref, byteOffset, length, (void*)inputBuffer);
    }

	FilediskProxyManaged::~FilediskProxyManaged() { delete _nativePtr; }
}