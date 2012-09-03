/*
 * Copyright (c) 2010 Inside Secure, All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.opennfc.service;

/**
 * Task done in separate thread
 * 
 * @param <PARAMETER> Parameter type
 */
public interface ThreadedTask<PARAMETER> {
    /**
     * Call when task turn arrive to execute it
     * 
     * @param taskID The task ID
     * @param parameters The parameters given to the task
     */
    public void excuteTask(int taskID, PARAMETER... parameters);

    /** Call when task is cancel (Typically on timeout) */
    public void cancel();
}
