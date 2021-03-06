/******************************************************************************
 * Copyright (c) 2015, Georges Thill
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include "decoder_ym.h"

//
// Decoder class
//
DecoderYm::DecoderYm(const QString &path)
        : Decoder()
{
    m_path = path;
    m_bitrate = 0;
    m_totalTime = 0;
    m_freq = 0;
    pMusic = NULL;
}

DecoderYm::~DecoderYm()
{
    deinit();
}

bool DecoderYm::initialize()
{
    m_totalTime = 0.0;
    m_freq = 0;
    m_bitrate = 0;

    pMusic = new CYmMusic;
    if (pMusic == NULL)
    {
        qWarning("DecoderYm: failed to create CYmMusic");
        return false;
    }

	if (pMusic->load(m_path.toLocal8Bit()))
	{
  	    pMusic->getMusicInfo(&info);

     	pMusic->setLoopMode(YMFALSE);
	
        m_totalTime = info.musicTimeInMs;
        m_freq = 44100;
        m_bitrate = ((QFileInfo(m_path).size () * 8.0) / m_totalTime) + 0.5;
    }
    else
    {
        if (pMusic)
   	        delete pMusic;
        pMusic = NULL;
        qWarning("DecoderYm: failed to open: %s", qPrintable(m_path));
        return false;
    }

    configure(m_freq, 1, Qmmp::PCM_S16LE);
    
    qDebug("DecoderYm: detected format: \"%s\"", info.pSongType);
    qDebug("DecoderYm: initialize success");
    return true;
}

void DecoderYm::deinit()
{
    m_totalTime = 0;
    m_bitrate = 0;
    m_freq = 0;
    if (pMusic)
   	    delete pMusic;
    pMusic = NULL;
}

qint64 DecoderYm::totalTime()
{
    return m_totalTime;
}

int DecoderYm::bitrate()
{
    return m_bitrate ? m_bitrate : 1;
}

qint64 DecoderYm::read(char *audio, qint64 maxSize)
{
    if (pMusic->update((ymsample *)audio, maxSize / sizeof(ymsample)))
       return maxSize;
       
    return 0;
}

void DecoderYm::seek(qint64 pos)
{
	pMusic->setMusicTime((ymu32)pos);
}

