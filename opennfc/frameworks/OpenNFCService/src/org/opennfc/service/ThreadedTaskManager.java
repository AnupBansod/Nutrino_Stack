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

import android.util.Log;
import android.util.SparseArray;

import java.util.concurrent.Future;
import java.util.concurrent.ScheduledThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.ReentrantLock;

/**
 * Manage threads
 */
public final class ThreadedTaskManager extends ReentrantLock {
    /**
     * Encapsulate a task and its parameters
     * 
     * @param <PARAMETER> Task parameter type
     */
    class TaskThreaded<PARAMETER> implements Runnable {
        /** Indicates if task is finished */
        boolean finished;
        /** Task internal ID */
        private final int mTaskInternalID;
        /** Task to do in separate thread */
        private final ThreadedTask<PARAMETER> mThreadedTask;
        /** Task parameters */
        private final PARAMETER[] parameters;
        /** Indicates if task is repeated */
        private final boolean repeatMode;
        /** Indicates if task is started */
        boolean started;

        /**
         * Create the encapsulator
         * 
         * @param repeatMode Indicates if task is repeated
         * @param taskInternalID Thread internal ID
         * @param threadedTask Task to play in separate thread
         * @param parameters Parameters to give to the task
         */
        public TaskThreaded(final boolean repeatMode, final int taskInternalID,
                final ThreadedTask<PARAMETER> threadedTask,
                final PARAMETER[] parameters) {
            this.repeatMode = repeatMode;
            this.mTaskInternalID = taskInternalID;
            this.mThreadedTask = threadedTask;
            this.parameters = parameters;
            this.started = false;
            this.finished = false;
        }

        /**
         * Do the task
         */
        @Override
        public void run() {
            synchronized (this) {
                this.notifyAll();
            }

            this.started = true;

            this.mThreadedTask.excuteTask(this.mTaskInternalID, this.parameters);

            if (this.repeatMode == false) {
                ThreadedTaskManager.this.finish(this.mTaskInternalID);

                this.finished = true;
            }

            synchronized (this) {
                this.notifyAll();
            }
        }
    }

    /** Enable/disable debug debug */
    private static final boolean DEBUG = true;

    /** The thread task manager singleton instance */
    public static final ThreadedTaskManager MANAGER = new ThreadedTaskManager();
    /** Number maximum possible of thread run in same time */
    private static final int NUMBER_OF_THREAD = 4;

    /** Start size of task list */
    private static final int SPARSE_ARRAY_SIZE = ThreadedTaskManager.NUMBER_OF_THREAD << 1;
    /** TAG to use on debug */
    private static final String TAG = ThreadedTaskManager.class.getSimpleName();
    /** Timeout (in millisecond) before decide that shutdown failed */
    private static final long TIME_WAIT = 1024;
    /** Next task ID */
    private int mNextTaskID;
    /** Pool of thread execution */
    private final ScheduledThreadPoolExecutor mScheduledThreadPoolExecutor;
    /** Tasks list */
    private final SparseArray<Future<?>> mTaskThreadedList;

    /**
     * Create the thread task manager
     */
    private ThreadedTaskManager() {
        this.mScheduledThreadPoolExecutor = new ScheduledThreadPoolExecutor(
                ThreadedTaskManager.NUMBER_OF_THREAD);
        this.mTaskThreadedList = new SparseArray<Future<?>>(ThreadedTaskManager.SPARSE_ARRAY_SIZE);
        this.mNextTaskID = 0;
    }

    /**
     * Cancel a thread
     * 
     * @param threadID Thread ID
     */
    public void cancel(final int threadID) {
        final Future<?> future = this.mTaskThreadedList.get(threadID);
        if (future != null && future.isCancelled() == false && future.isDone() == false) {
            future.cancel(true);
        }

        this.finish(threadID);
    }

    /**
     * Destroy the manager
     */
    public void destroy() {
        this.mScheduledThreadPoolExecutor.purge();
        this.mScheduledThreadPoolExecutor.shutdown();

        try {
            if (this.mScheduledThreadPoolExecutor
                    .awaitTermination(ThreadedTaskManager.TIME_WAIT, TimeUnit.MILLISECONDS) == false) {
                this.mScheduledThreadPoolExecutor.shutdownNow();
                if (this.mScheduledThreadPoolExecutor
                        .awaitTermination(ThreadedTaskManager.TIME_WAIT, TimeUnit.MILLISECONDS) == false) {
                    if (ThreadedTaskManager.DEBUG) {
                        Log.v(ThreadedTaskManager.TAG, "Can't destroy properly !");
                    }
                }
            }
        } catch (final InterruptedException exception) {
            if (ThreadedTaskManager.DEBUG) {
                Log.d(ThreadedTaskManager.TAG, "Failed to terminate", exception);
            }
        }

        this.mTaskThreadedList.clear();

        System.gc();
    }

    /**
     * Say a task to play in separate thread
     * 
     * @param <PARAMETER> Task parameters type
     * @param threadedTask Task to do
     * @param parameters Task parameters
     * @return Thread ID associated to the task (Use for cancel it in
     *         {@link #cancel(int)} or know its status with {@link #isDone(int)}
     */
    public <PARAMETER> int doInSeparateThread(final ThreadedTask<PARAMETER> threadedTask,
            final PARAMETER... parameters) {
        int id = 0;

        this.lock();
        try {
            id = this.mNextTaskID;

            this.mNextTaskID++;
        } finally {
            this.unlock();
        }

        final Future<?> future = this.mScheduledThreadPoolExecutor
                    .schedule(new TaskThreaded<PARAMETER>(false, id, threadedTask, parameters), 16,
                            TimeUnit.MILLISECONDS);
        this.mTaskThreadedList.append(id, future);

        return id;
    }

    /**
     * Execute task with time out, if task execution make more than the
     * specified timeout, the task is cancel. This method block until the task
     * finish or timeout append
     * 
     * @param <PARAMETER> Task parameters type
     * @param timeout Timeout for the task in milliseconds
     * @param threadedTask Task to do
     * @param parameters Task parameters
     * @return {@code true} if task finish normally. {@code false} if timeout
     *         append
     */
    public <PARAMETER> boolean executeWithTimeOut(final int timeout,
            final ThreadedTask<PARAMETER> threadedTask,
            final PARAMETER... parameters) {
        int id = 0;

        this.lock();
        try {
            id = this.mNextTaskID;

            this.mNextTaskID++;
        } finally {
            this.unlock();
        }

        final TaskThreaded<PARAMETER> taskThreaded = new TaskThreaded<PARAMETER>(false, id,
                threadedTask,
                parameters);
        final Future<?> future = this.mScheduledThreadPoolExecutor
                .submit(taskThreaded);

        while (taskThreaded.started == false) {
            synchronized (taskThreaded) {
                try {
                    taskThreaded.wait(128);
                } catch (final InterruptedException exception) {
                }
            }
        }

        synchronized (taskThreaded) {
            try {
                taskThreaded.wait(timeout);
            } catch (final InterruptedException exception) {
            }
        }

        final boolean success = taskThreaded.finished;
        if (success == false) {
            future.cancel(true);
            threadedTask.cancel();
        }

        return success;
    }

    /**
     * Call internally when a task is finish
     * 
     * @param threadID Thread ID
     */
    void finish(final int threadID) {
        this.mTaskThreadedList.remove(threadID);
    }

    /**
     * Indicates if a task is finished
     * 
     * @param threadID Thread ID associated to task
     * @return {@code true} if task is done
     */
    public boolean isDone(final int threadID) {
        final Future<?> future = this.mTaskThreadedList.get(threadID);

        if (future == null) {
            return true;
        }

        return future.isDone();
    }

    /**
     * Do task several in repetitive way. Task task will be done, then wait an
     * amount of time, then done again, ...
     * 
     * @param <PARAMETER> Task parameters type
     * @param repeatTime Time for repetition
     * @param threadedTask Task to repeat
     * @param parameters Task parameters
     * @return The task ID to be able manipulate it later
     */
    public <PARAMETER> int repetitiveThread(final int repeatTime,
            final ThreadedTask<PARAMETER> threadedTask,
            final PARAMETER... parameters) {
        int id = 0;

        this.lock();
        try {
            id = this.mNextTaskID;

            this.mNextTaskID++;
        } finally {
            this.unlock();
        }

        final Future<?> future = this.mScheduledThreadPoolExecutor
                    .scheduleAtFixedRate(new TaskThreaded<PARAMETER>(true, id, threadedTask,
                            parameters),
                            repeatTime, repeatTime, TimeUnit.MILLISECONDS);
        this.mTaskThreadedList.append(id, future);

        return id;
    }
}
