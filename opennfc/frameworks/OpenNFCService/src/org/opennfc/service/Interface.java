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
 * Link with some native methods
 */
public class Interface {

    /**
     * Ask to start Open NFC server
     * 
     * @param config Configuration to use
     * @return Result status
     */
    static public native int OpenNFCServerStart(byte[] config);

    /**
     * Ask to stop Open NFC server
     * 
     * @return Result status
     */
    static public native int OpenNFCServerStop();
}
