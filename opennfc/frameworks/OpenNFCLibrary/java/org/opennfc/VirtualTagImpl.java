/*
 * Copyright (c) 2010 Inside Secure, All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.opennfc;

import org.opennfc.nfctag.VirtualTag;
import org.opennfc.nfctag.VirtualTagEventHandler;

/**
 * package private
 *
 * @hide
 **/
final class VirtualTagImpl extends ConnectionImpl implements VirtualTag {

     /* package private */VirtualTagImpl(ConnectionProperty tagType, byte[] identifier, int tagCapacity) throws NfcException{

        NfcSecurityManager.check(NfcSecurityManager.CARD_EMUL);

        if((tagType == null) || (identifier == null)) {
            throw new IllegalArgumentException("tagType or identifier = null");
        }

        if(tagCapacity <= 0) {
            throw new IllegalArgumentException("tagCapacity is invalid");
        }

        SafeHandle virtualTagHandle = new SafeHandle();

        checkError(MethodAutogen.WVirtualTagCreate(tagType.getValue(), identifier, tagCapacity, virtualTagHandle.getReference()));

        setHandle(virtualTagHandle);
    }

    public void start(VirtualTagEventHandler handler, boolean isReadOnly) throws NfcException {

      if(handler == null) {
         throw new IllegalArgumentException("handler == null");
      }

      GenericCallbackCompletion callbackFunction = new GenericCallbackCompletion();
      TagEventHandler eventHandler = new TagEventHandler(handler);

      MethodAutogen.WVirtualTagStart(
         getNonNullHandle(), callbackFunction.getId(),
         eventHandler.getId(), isReadOnly);

       checkError(callbackFunction.waitForResult());
    }

    public void stop() {
      GenericCallbackCompletion callbackFunction = new GenericCallbackCompletion();

      MethodAutogen.WVirtualTagStop(
         getNonNullHandle(), callbackFunction.getId());

      // Do not check the error, no exception is thrown
      callbackFunction.waitForCompletion();
    }

    private class GenericCallbackCompletion extends Callback_tWBasicGenericCallbackFunction {

      GenericCallbackCompletion() {
         super(false);
      }

      @Override
    protected void tWBasicGenericCallbackFunction(int result) {
         signalResult(result);
      }
    }

    private class TagEventHandler extends Callback_tWBasicGenericEventHandler {

      private VirtualTagEventHandler mHandler;

      TagEventHandler(VirtualTagEventHandler handler) {
         super(true);
         this.mHandler = handler;
      }

      @Override
    protected void tWBasicGenericEventHandler(int eventCode) {
         this.mHandler.onTagEventDetected(eventCode);
      }
    }

    private static void checkError(int error) throws NfcException {

        if(error != ConstantAutogen.W_SUCCESS) {
           switch(error) {
              case ConstantAutogen.W_ERROR_OUT_OF_RESOURCE:
                throw new OutOfMemoryError();
              case ConstantAutogen.W_ERROR_BAD_PARAMETER :
                throw new IllegalArgumentException("parameter inconsistency");
              case ConstantAutogen.W_ERROR_BAD_STATE :
                throw new IllegalStateException();
              default:
                throw new NfcException("Error creating the virtual tag", NfcErrorCode.getCode(error));
           }
        }
    }
}
