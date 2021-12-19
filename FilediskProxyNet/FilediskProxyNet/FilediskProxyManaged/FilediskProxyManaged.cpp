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

    BOOL FilediskProxyManaged::init_ctx(UCHAR DriveLetter, size_t filesize, BOOL usePipe, BOOL useShm, BOOL useSocket, BOOL readOnlyDisk, ULONG port, OUT int64_t% ctxOut)
    {
        // Initialize Software AES Framework for the CBC mode
        int64_t ctx = 0;
        BOOL result = this->_nativePtr->init_ctx(DriveLetter, filesize, usePipe, useShm, useSocket, readOnlyDisk, port, ctx);
        ctxOut = ctx;
        return result;
    }

    BOOL FilediskProxyManaged::delete_ctx(int64_t ctxref)
    {
        return this->_nativePtr->delete_ctx(ctxref);
    }

    // set/reset write access to the virtual disk drive
    int FilediskProxyManaged::setWriteAccess(int64_t ctxref, BOOL set)
    {
        return this->_nativePtr->setWriteAccess(ctxref, set);
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

    // set/reset Event
    void FilediskProxyManaged::SetEventShutdown(int64_t ctxref, BOOL set)
    {
        this->_nativePtr->SetEventShutdown(ctxref, set);
    }

    void FilediskProxyManaged::SetEventShutdownComplete(int64_t ctxref, BOOL set)
    {
        this->_nativePtr->SetEventShutdownComplete(ctxref, set);
    }

    void FilediskProxyManaged::PulseEventDriverRequestDataSet(int64_t ctxref)
    {
        this->_nativePtr->PulseEventDriverRequestDataSet(ctxref);

    }
    void FilediskProxyManaged::PulseEventProxyIdle(int64_t ctxref)
    {
        this->_nativePtr->PulseEventProxyIdle(ctxref);
    }

    void FilediskProxyManaged::PulseEventRequestComplete(int64_t ctxref)
    {
        this->_nativePtr->PulseEventRequestComplete(ctxref);
    }

    void FilediskProxyManaged::PulseEventShutdown(int64_t ctxref)
    {
        this->_nativePtr->PulseEventShutdown(ctxref);
    }
    void FilediskProxyManaged::PulseEventShutdownComplete(int64_t ctxref)
    {
        this->_nativePtr->PulseEventShutdownComplete(ctxref);
    }

    // notifies the windows about drive add or remove
    void FilediskProxyManaged::NotifyWindows(int64_t ctxref, BOOL DriveAdded)
    {
        this->_nativePtr->NotifyWindows(ctxref, DriveAdded);
    }

    void FilediskProxyManaged::NotifyWindowsAtributesChanged(int64_t ctxref)
    {
        this->_nativePtr->NotifyWindowsAtributesChanged(ctxref);
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

    DWORD FilediskProxyManaged::WaitEventShutdownComplete(int64_t ctxref, DWORD miliSeconds)
    {
        return this->_nativePtr->WaitEventShutdownComplete(ctxref, miliSeconds);
    }
    int FilediskProxyManaged::isEventSignalled(HANDLE hEvent)
    {
        return this->_nativePtr->isEventSignalled(hEvent);
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

    int FilediskProxyManaged::ConnectPipe(int64_t ctxref)
    {
        return this->_nativePtr->ConnectPipe(ctxref);
    }
    
    int FilediskProxyManaged::DisconnectPipe(int64_t ctxref)
    {
        return this->_nativePtr->DisconnectPipe(ctxref);
    }

    int FilediskProxyManaged::deregister_file(int64_t ctxref)
    {
        return this->_nativePtr->deregister_file(ctxref);
    }

    void FilediskProxyManaged::delete_objects(int64_t ctxref)
    {
        return this->_nativePtr->delete_objects(ctxref);
    }

    BOOL FilediskProxyManaged::Step1PipeGetRequest(int64_t ctxref, OUT uint64_t% byteOffset, OUT DWORD% length, OUT UCHAR% function, OUT DWORD% totalBytesReadWrite)
    {
        uint64_t byteOffsettmp = 0;
        DWORD lengthtmp = 0;
        UCHAR functiontmp = 0;
        DWORD totalbytesreadwritetmp = 0;
        BOOL result = this->_nativePtr->Step1PipeGetRequest(ctxref, byteOffsettmp, lengthtmp, functiontmp, totalbytesreadwritetmp);
        byteOffset = byteOffsettmp;
        length = lengthtmp;
        function = functiontmp;
        totalBytesReadWrite = totalbytesreadwritetmp;
        return result;
    }

    void FilediskProxyManaged::ReadPipe(int64_t ctxref, int64_t outputBuffer, int64_t length)
    {
        this->_nativePtr->ReadPipe(ctxref, (void*)outputBuffer, length);
    }
    void FilediskProxyManaged::WritePipe(int64_t ctxref, int64_t inputBuffer, int64_t length)
    {
        this->_nativePtr->WritePipe(ctxref, (void*)inputBuffer, length);
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

    // sockets
    BOOL FilediskProxyManaged::Step1SocketGetRequest(int64_t ctxref, OUT uint64_t% byteOffset, OUT DWORD% length, OUT UCHAR% function, OUT DWORD% totalBytesReadWrite)
    {
        uint64_t byteOffsettmp = 0;
        DWORD lengthtmp = 0;
        UCHAR functiontmp = 0;
        DWORD totalbytesreadwritetmp = 0;
        BOOL result = this->_nativePtr->Step1SocketGetRequest(ctxref, byteOffsettmp, lengthtmp, functiontmp, totalbytesreadwritetmp);
        byteOffset = byteOffsettmp;
        length = lengthtmp;
        function = functiontmp;
        totalBytesReadWrite = totalbytesreadwritetmp;
        return result;
    }

    void FilediskProxyManaged::ReadSocket(int64_t ctxref, int64_t outputBuffer, size_t length)
    {
        this->_nativePtr->ReadSocket(ctxref, (void*)outputBuffer, length);
    }

    void FilediskProxyManaged::WriteSocket(int64_t ctxref, int64_t inputBuffer, size_t length)
    {
        this->_nativePtr->WriteSocket(ctxref, (void*)inputBuffer, length);
    }

    void FilediskProxyManaged::SocketAcceptClient(int64_t ctxref)
    {
        this->_nativePtr->SocketAcceptClient(ctxref);
    }

    void FilediskProxyManaged::CloseClientSocket(int64_t ctxref)
    {
        this->_nativePtr->CloseClientSocket(ctxref);
    }

	FilediskProxyManaged::~FilediskProxyManaged() { delete _nativePtr; }
}