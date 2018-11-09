/*
 * Copyright © 2017 VMware, Inc.  All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the “License”); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an “AS IS” BASIS, without
 * warranties or conditions of any kind, EITHER EXPRESS OR IMPLIED.  See the
 * License for the specific language governing permissions and limitations
 */

typedef enum
{
    VDIR_EVENT_MOD = 0,
    VDIR_EVENT_ADD,
    VDIR_EVENT_DEL
}VDIR_EVENT_TYPE;

typedef struct _VDIR_EVENT_DATA
{
    BOOL            bIsGroupUpdate;
    DWORD           revision;
    PVDIR_ENTRY     pCurEntry;
    PVDIR_ENTRY     pPrevEntry;
    VDIR_EVENT_TYPE eventType;
}VDIR_EVENT_DATA, *PVDIR_EVENT_DATA;

typedef struct _VDIR_EVENT
{
    DWORD                       refCount;
    PVMDIR_MUTEX                pMutex;
    PVMDIR_COND                 pCond;
    PVDIR_EVENT_DATA            pEventData;
    BOOL                        bIsEventReady;
    BOOL                        bIsSuccessful;
    PVDIR_LINKED_LIST_NODE      pListNode;
    struct _VDIR_EVENT_NODE*    pNext;
}VDIR_EVENT, *PVDIR_EVENT;

typedef struct _VDIR_EVENT_REPO
{
    PVDIR_LINKED_LIST   pReadyEventList;
    PVDIR_QUEUE         pPendingQueue;
}VDIR_EVENT_REPO, *PVDIR_EVENT_REPO;

typedef PVDIR_EVENT PVDIR_EVENT_REPO_COOKIE;