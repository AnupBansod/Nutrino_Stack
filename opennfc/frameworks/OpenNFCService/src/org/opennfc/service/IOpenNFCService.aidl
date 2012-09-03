/*
 * Copyright (C) 2010 The Android Open Source Project
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
 * Example of defining an interface for calling on to a remote service
 * (running in another process).
 */
interface IOpenNFCService {

	/**
	 * Configures the Open NFC stack
	 *
	 */
	boolean configureNFC(in byte[] config);

	/**
	 * Controls the Open NFC stack
	 * if enable is true, the Open NFC stack will be started
	 *	if enable is false, the Open NFC stack will be stopped
	 *
	 *	@return true if the operation was successful
	 */
	boolean setNFCEnabled(boolean enable);

	/**
	  * Retrieves the current state of the Open NFC stack
	  *
	  * @return true if the Open NFC stack is started
	  */
	boolean getNFCEnabled();
}