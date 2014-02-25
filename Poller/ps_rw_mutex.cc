/*
 * Copyright (c) 2008,2009, University of California, Los Angeles and 
 * Colorado State University All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of NLnetLabs nor the names of its
 *       contributors may be used to endorse or promote products derived from this
 *       software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include "config.h"
#include "ps_logger.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "ps_rw_mutex.h"
#include "ps_defs.h"

PsRwMutex::PsRwMutex()
{
  int iErr = pthread_rwlock_init(&m_tRwMutex, NULL);
  if (0 != iErr)
  {
    ps_elog(PSL_CRITICAL, "Unable to init rw lock: %s\n", strerror(iErr));
  }
}

PsRwMutex::~PsRwMutex()
{
  pthread_rwlock_destroy(&m_tRwMutex);
}

bool PsRwMutex::readLock()
{
  bool bRet = false;

  ps_elog(PSL_DEBUG, "Creating READ lock now...\n");
  int iErr = pthread_rwlock_rdlock(&m_tRwMutex);
  if (0 != iErr)
  {
    ps_elog(PSL_CRITICAL, "Unable to read lock rw mutex: %s\n", strerror(iErr));
  }
  else
  {
    ps_elog(PSL_DEBUG, "Obtained READ lock.\n");
    bRet = true;
  }

  return bRet;
}

bool PsRwMutex::writeLock()
{
  bool bRet = false;

  ps_elog(PSL_DEBUG, "Creating WRITE lock now...\n");
  int iErr = pthread_rwlock_wrlock(&m_tRwMutex);
  if (0 != iErr)
  {
    ps_elog(PSL_CRITICAL, "Unable to write lock rw mutex: %s\n", strerror(iErr));
  }
  else
  {
    ps_elog(PSL_DEBUG, "Obtained WRITE lock.\n");
    bRet = true;
  }

  return bRet;
}

bool PsRwMutex::unlock()
{
  bool bRet = false;

  int iErr = pthread_rwlock_unlock(&m_tRwMutex);
  if (0 != iErr)
  {
    ps_elog(PSL_CRITICAL, "Unable to unlock rw mutex: %s\n", strerror(iErr));
  }
  else
  {
    bRet = true;
  }

  return bRet;
}

