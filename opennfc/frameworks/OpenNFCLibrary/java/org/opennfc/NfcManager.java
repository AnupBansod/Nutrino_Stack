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

import org.opennfc.cardemulation.CardEmulationRegistry;
import org.opennfc.cardlistener.CardListenerRegistry;
import org.opennfc.hardware.NfcController;
import org.opennfc.nfctag.NfcTagManager;
import org.opennfc.p2p.P2PManager;

/**
 * The NFC Manager class is the single-instance class used to access the Open NFC library.
 * 
 * @since Open NFC 4.0
 **/
public final class NfcManager {

    private static NfcManager sInstance;

    private NfcManager() {

        NfcSecurityManager.check(NfcSecurityManager.NFC);

        System.loadLibrary("open_nfc_client_jni");
    }

    /**
     * Returns the single instance of the NFC manager.
     * 
     * @return the NFC manager instance.
     */
    public static NfcManager getInstance() {

        NfcManager instance = NfcManager.sInstance;
        if (instance == null) {
            instance = new NfcManager();
            NfcManager.sInstance = instance;
        }
        return instance;
    }

    /**
     * Returns the single instance of the NFC manager.
     * 
     * @param object a blind object provided to the Security Manager
     *
     * @return the NFC manager instance.
     */
    public static NfcManager getInstance(Object object) {

        NfcManager instance = NfcManager.sInstance;
        if (instance == null) {

            NfcSecurityManager.setContext(object);

            instance = new NfcManager();
            NfcManager.sInstance = instance;
        }
        return instance;
    }

    /**
     * Executes the event loop in the event thread.
     * 
     * @throws NfcException in case of error.
     */
    private static void executeEventLoop() throws NfcException {

        for (;;) {

            int error = MethodAutogen.WBasicPumpEvent(true);
            if (error == ConstantAutogen.W_SUCCESS) {
                AsynchronousCompletion.dispatchEvent();
            } else if (error == ConstantAutogen.W_ERROR_WAIT_CANCELLED) {
                /* WBasicStopEventLoop has been called, exit the loop */
                break;
            } else {
                throw new NfcException("Error while pumping an event", NfcErrorCode.getCode(error));
            }
        }
    }

    private Thread mEventThread;

    private NfcException mEventException;

    private Error mEventError;

    private RuntimeException mEventRuntimeException;

    private NfcTagManagerImpl mInstanceNfcTagManager;

    private CardListenerRegistryImpl mInstanceCardListenerRegistry;

    private CardEmulationRegistryImpl mInstanceCardEmulationRegistry;

    private NfcControllerImpl mInstanceNfcController;

    private P2PManager mInstanceP2PManager;

    /**
     * Inner class for the event thread
     */
    private class InnerThread extends Thread {

        @Override
        public void run() {
            try {
                NfcManager.executeEventLoop();
            } catch (NfcException e) {
                mEventException = e;
            } catch (Error e) {
                mEventError = e;
            } catch (RuntimeException e) {
                mEventRuntimeException = e;
            }
        }
    }

    /**
     * Starts the NFC Manager. This method shall be called once before any usage of the other NFC objects. Use {@link #stop()}
     * when the NFC objects are no longer used.
     * 
     * @see #stop()
     * @throws IllegalStateException if the NFC Manager is already started.
     * @throws NfcException if an error occurred in the initialization of the NFC Manager.
     * @since Open NFC 4.0
     **/
    public void start() throws IllegalStateException, NfcException {

        synchronized (this) {
            if (this.mEventThread != null) {
                throw new IllegalStateException();
            }

            int error = MethodAutogen.WBasicInit("4.2");
            if (error != ConstantAutogen.W_SUCCESS) {
                throw new NfcException("Error while starting the NFC Manager", NfcErrorCode.getCode(error));
            }

            this.mEventThread = new InnerThread();
            this.mEventException = null;
            this.mEventError = null;
            this.mEventRuntimeException = null;

            this.mEventThread.start();
        }
    }

    /**
     * Stops the NFC Manager. This method shall be called once when the NFC Manager is no longer used. The NFC manager is
     * stopped even if an exception is thrown.
     * 
     * @see #start()
     * @throws IllegalStateException if the NFC Manager is not started.
     * @throws NfcException if an error occurred in the event thread.
     * @since Open NFC 4.0
     **/
    public void stop() throws NfcException, IllegalStateException {

        synchronized (this) {
            if (this.mEventThread == null) {
                throw new IllegalStateException();
            }

            MethodAutogen.WBasicStopEventLoop();

            AsynchronousCompletion.abortAll();

            try {
                this.mEventThread.join();
            } catch (InterruptedException e) {
                throw new IllegalStateException("Event thread was interrupted", e);
            } finally {
                this.mEventThread = null;
                MethodAutogen.WBasicTerminate();
            }

            if (this.mEventException != null) {
                throw this.mEventException;
            }
            if (this.mEventError != null) {
                throw this.mEventError;
            }
            if (this.mEventRuntimeException != null) {
                throw this.mEventRuntimeException;
            }
        }
    }

    private final static String OPEN_NFC_VERSION = "open_nfc.version";

    private final static String OPEN_NFC_IMPL = "open_nfc.implementation";

    private final static String NFC_HAL_VERSION = "nfcc.nfc_hal_version";

    private final static String P2P_SUPPORTED = "p2p.supported";

    private final static String CARD_LISTEN_ISO_14443_A_CID = "card_listen.iso_14443_A_cid";

    private final static String CARD_LISTEN_ISO_14443_A_INPUT_SIZE = "card_listen.iso_14443_A_input_size";

    private final static String CARD_LISTEN_ISO_14443_A_MAX_RATE = "card_listen.iso_14443_A_max_rate";

    private final static String CARD_LISTEN_ISO_14443_A_NAD = "card_listen.iso_14443_A_nad";

    private final static String CARD_LISTEN_ISO_14443_B_CID = "card_listen.iso_14443_B_cid";

    private final static String CARD_LISTEN_ISO_14443_B_INPUT_SIZE = "card_listen.iso_14443_B_input_size";

    private final static String CARD_LISTEN_ISO_14443_B_MAX_RATE = "card_listen.iso_14443_B_max_rate";

    private final static String CARD_LISTEN_ISO_14443_B_NAD = "card_listen.iso_14443_B_nad";

    private final static String CARD_EMUL_ISO_14443_A_CID = "card_emul.iso_14443_A_cid";

    private final static String CARD_EMUL_ISO_14443_A_MAX_RATE = "card_emul.iso_14443_A_max_rate";

    private final static String CARD_EMUL_ISO_14443_A_NAD = "card_emul.iso_14443_A_nad";

    private final static String CARD_EMUL_ISO_14443_B_CID = "card_emul.iso_14443_B_cid";

    private final static String CARD_EMUL_ISO_14443_B_MAX_RATE = "card_emul.iso_14443_B_max_rate";

    private final static String CARD_EMUL_ISO_14443_B_NAD = "card_emul.iso_14443_B_nad";

    private final static String BATTERY_LOW_SUPPORTED = "nfcc.battery_low_supported";

    private final static String BATTERY_OFF_SUPPORTED = "nfcc.battery_off_supported";

    private final static String STANDBY_SUPPORTED = "nfcc.standby_supported";

    private final static String STANDBY_TIMEOUT = "nfcc.standby_timeout";

    private final static String LOADER_VERSION = "nfcc.loader_version";

    private final static String FIRMWARE_VERSION = "nfcc.firmware_version";

    private final static String HARDWARE_VERSION = "nfcc.harware_version";

    private final static String HARDWARE_SN = "nfcc.hardware_serial_number";

    private final static String SPECIAL_PULSE_PERIOD = "nfcc.pulse_period";

    private final static int PULSE_PERIOD = 0x1000;

    private final static String SPECIAL_PERSIST_LOCK_CARD = "nfcc.lock.persistent.card_emulation";

    private final static int PERSIST_LOCK_CARD = 0x1001;

    private final static String SPECIAL_PERSIST_LOCK_READER = "nfcc.lock.persistent.card_listener";

    private final static int PERSIST_LOCK_READER = 0x1002;

    private final static String SPECIAL_VOLATILE_LOCK_CARD = "nfcc.lock.volatile.card_emulation";

    private final static int VOLATILE_LOCK_CARD = 0x1004;

    private final static String SPECIAL_VOLATILE_LOCK_READER = "nfcc.lock.volatile.card_listener";

    private final static int VOLATILE_LOCK_READER = 0x1005;

    private final static String SPECIAL_ACTIVITY_CARD = "nfcc.activity.card_emulation";

    private final static int ACTIVITY_CARD = 0x1007;

    private final static String SPECIAL_ACTIVITY_READER = "nfcc.activity.card_listener";

    private final static int ACTIVITY_READER = 0x1008;

    private final static String SPECIAL_ACTIVITY_P2P = "nfcc.activity.p2p";

    private final static int ACTIVITY_P2P = 0x1009;

    private final static String ACTIVITY_ACTIVE = "active";

    private final static String ACTIVITY_DETECTION = "detection";

    private final static String ACTIVITY_INACTIVE = "inactive";

    /**
     * Finds the specified property identifier.
     * 
     * @param property The property identifier.
     *
     * @return The property identifier or -1 if the property is not found.
     * @throws SecurityException if the calling application is not allowed to access this property.
     **/
    private static int findProperty(String property) {
        int identifier = -1;
        String securityToken = null;

        if (property.equals(OPEN_NFC_VERSION)) {
            identifier = ConstantAutogen.W_NFCC_PROP_LIBRARY_VERSION;
            securityToken = NfcSecurityManager.NFC;
        } else if (property.equals(OPEN_NFC_IMPL)) {
            identifier = ConstantAutogen.W_NFCC_PROP_LIBRARY_IMPL;
            securityToken = NfcSecurityManager.NFC;
        } else if (property.equals(NFC_HAL_VERSION)) {
            identifier = ConstantAutogen.W_NFCC_PROP_NFC_HAL_VERSION;
            securityToken = NfcSecurityManager.HARDWARE;
        } else if (property.equals(P2P_SUPPORTED)) {
            identifier = ConstantAutogen.W_NFCC_PROP_P2P;
            securityToken = NfcSecurityManager.P2P;
        } else if (property.equals(CARD_LISTEN_ISO_14443_A_CID)) {
            identifier = ConstantAutogen.W_NFCC_PROP_READER_ISO_14443_A_CID;
            securityToken = NfcSecurityManager.CARD_LISTEN;
        } else if (property.equals(CARD_LISTEN_ISO_14443_A_INPUT_SIZE)) {
            identifier = ConstantAutogen.W_NFCC_PROP_READER_ISO_14443_A_INPUT_SIZE;
            securityToken = NfcSecurityManager.CARD_LISTEN;
        } else if (property.equals(CARD_LISTEN_ISO_14443_A_MAX_RATE)) {
            identifier = ConstantAutogen.W_NFCC_PROP_READER_ISO_14443_A_MAX_RATE;
            securityToken = NfcSecurityManager.CARD_LISTEN;
        } else if (property.equals(CARD_LISTEN_ISO_14443_A_NAD)) {
            identifier = ConstantAutogen.W_NFCC_PROP_READER_ISO_14443_A_NAD;
            securityToken = NfcSecurityManager.CARD_LISTEN;
        } else if (property.equals(CARD_LISTEN_ISO_14443_B_CID)) {
            identifier = ConstantAutogen.W_NFCC_PROP_READER_ISO_14443_B_CID;
            securityToken = NfcSecurityManager.CARD_LISTEN;
        } else if (property.equals(CARD_LISTEN_ISO_14443_B_INPUT_SIZE)) {
            identifier = ConstantAutogen.W_NFCC_PROP_READER_ISO_14443_B_INPUT_SIZE;
            securityToken = NfcSecurityManager.CARD_LISTEN;
        } else if (property.equals(CARD_LISTEN_ISO_14443_B_MAX_RATE)) {
            identifier = ConstantAutogen.W_NFCC_PROP_READER_ISO_14443_B_MAX_RATE;
            securityToken = NfcSecurityManager.CARD_LISTEN;
        } else if (property.equals(CARD_LISTEN_ISO_14443_B_NAD)) {
            identifier = ConstantAutogen.W_NFCC_PROP_READER_ISO_14443_B_NAD;
            securityToken = NfcSecurityManager.CARD_LISTEN;
        } else if (property.equals(BATTERY_LOW_SUPPORTED)) {
            identifier = ConstantAutogen.W_NFCC_PROP_BATTERY_LOW_SUPPORTED;
            securityToken = NfcSecurityManager.HARDWARE;
        } else if (property.equals(BATTERY_OFF_SUPPORTED)) {
            identifier = ConstantAutogen.W_NFCC_PROP_BATTERY_OFF_SUPPORTED;
            securityToken = NfcSecurityManager.HARDWARE;
        } else if (property.equals(STANDBY_TIMEOUT)) {
            identifier = ConstantAutogen.W_NFCC_PROP_STANDBY_TIMEOUT;
            securityToken = NfcSecurityManager.HARDWARE;
        } else if (property.equals(STANDBY_SUPPORTED)) {
            identifier = ConstantAutogen.W_NFCC_PROP_STANDBY_SUPPORTED;
            securityToken = NfcSecurityManager.HARDWARE;
        } else if (property.equals(LOADER_VERSION)) {
            identifier = ConstantAutogen.W_NFCC_PROP_LOADER_VERSION;
            securityToken = NfcSecurityManager.HARDWARE;
        } else if (property.equals(FIRMWARE_VERSION)) {
            identifier = ConstantAutogen.W_NFCC_PROP_FIRMWARE_VERSION;
            securityToken = NfcSecurityManager.HARDWARE;
        } else if (property.equals(HARDWARE_VERSION)) {
            identifier = ConstantAutogen.W_NFCC_PROP_HARDWARE_VERSION;
            securityToken = NfcSecurityManager.HARDWARE;
        } else if (property.equals(HARDWARE_SN)) {
            identifier = ConstantAutogen.W_NFCC_PROP_HARDWARE_SN;
            securityToken = NfcSecurityManager.HARDWARE;
        } else if (property.equals(CARD_EMUL_ISO_14443_A_CID)) {
            identifier = ConstantAutogen.W_NFCC_PROP_CARD_ISO_14443_A_CID;
            securityToken = NfcSecurityManager.CARD_EMUL;
        } else if (property.equals(CARD_EMUL_ISO_14443_A_MAX_RATE)) {
            identifier = ConstantAutogen.W_NFCC_PROP_CARD_ISO_14443_A_MAX_RATE;
            securityToken = NfcSecurityManager.CARD_EMUL;
        } else if (property.equals(CARD_EMUL_ISO_14443_A_NAD)) {
            identifier = ConstantAutogen.W_NFCC_PROP_CARD_ISO_14443_A_NAD;
            securityToken = NfcSecurityManager.CARD_EMUL;
        } else if (property.equals(CARD_EMUL_ISO_14443_B_CID)) {
            identifier = ConstantAutogen.W_NFCC_PROP_CARD_ISO_14443_B_CID;
            securityToken = NfcSecurityManager.CARD_EMUL;
        } else if (property.equals(CARD_EMUL_ISO_14443_B_MAX_RATE)) {
            identifier = ConstantAutogen.W_NFCC_PROP_CARD_ISO_14443_B_MAX_RATE;
            securityToken = NfcSecurityManager.CARD_EMUL;
        } else if (property.equals(CARD_EMUL_ISO_14443_B_NAD)) {
            identifier = ConstantAutogen.W_NFCC_PROP_CARD_ISO_14443_B_NAD;
            securityToken = NfcSecurityManager.CARD_EMUL;
        }

        if (identifier != -1) {
            NfcSecurityManager.check(securityToken);
        }

        return identifier;
    }

    /**
     * Finds the specified property identifier.
     * 
     * @param property The property identifier.
     *
     * @return The property identifier or -1 if the property is not found.
     * @throws SecurityException if the calling application is not allowed to access this property.
     **/
    private static int findSpecialProperty(String property) {
        int identifier = -1;
        String securityToken = null;

        if (property.equals(SPECIAL_PULSE_PERIOD)) {
            identifier = PULSE_PERIOD;
            securityToken = NfcSecurityManager.HARDWARE;
        } else if (property.equals(SPECIAL_PERSIST_LOCK_CARD)) {
            identifier = PERSIST_LOCK_CARD;
            securityToken = NfcSecurityManager.HARDWARE;
        } else if (property.equals(SPECIAL_PERSIST_LOCK_READER)) {
            identifier = PERSIST_LOCK_READER;
            securityToken = NfcSecurityManager.HARDWARE;
        } else if (property.equals(SPECIAL_VOLATILE_LOCK_CARD)) {
            identifier = VOLATILE_LOCK_CARD;
            securityToken = NfcSecurityManager.HARDWARE;
        } else if (property.equals(SPECIAL_VOLATILE_LOCK_READER)) {
            identifier = VOLATILE_LOCK_READER;
            securityToken = NfcSecurityManager.HARDWARE;
        } else if (property.equals(SPECIAL_ACTIVITY_CARD)) {
            identifier = ACTIVITY_CARD;
            securityToken = NfcSecurityManager.HARDWARE;
        } else if (property.equals(SPECIAL_ACTIVITY_READER)) {
            identifier = ACTIVITY_READER;
            securityToken = NfcSecurityManager.HARDWARE;
        } else if (property.equals(SPECIAL_ACTIVITY_P2P)) {
            identifier = ACTIVITY_P2P;
            securityToken = NfcSecurityManager.HARDWARE;
        }

        if (identifier != -1) {
            NfcSecurityManager.check(securityToken);
        }

        return identifier;
    }

    private static int getPulsePeriod() {
        int[] pTimeout = new int[1];
        int error = MethodAutogen.WReaderGetPulsePeriod(pTimeout);
        if (error != ConstantAutogen.W_SUCCESS)
            throw new IllegalArgumentException("Get Pulse Period error - " + NfcErrorCode.getCode(error));
        return pTimeout[0];
    }

    private static void setPulsePeriod(int value) throws NfcException {
        CallbackGenericCompletion callback = new CallbackGenericCompletion();
        MethodAutogen.WReaderSetPulsePeriod(callback.getId(), value);
        callback.waitForCompletionOrException();
    }

    /**
     * Returns the string value of a property. For the definition of the
     * properties, see <a href="{@docRoot}overview-summary.html#nfc_controller_properties">NFC Properties</a>.
     *
     * @param property the identifier of the property.
     *
     * @return the property value.
     *
     * @throws IllegalArgumentException if the property is null.
     * @throws IllegalArgumentException if the property is not a string property.
     * @throws SecurityException if the calling application is not allowed to access this property.
     * @since Open NFC 4.0
     */
    public String getProperty(String property) {

        int identifier = -1;

        if (property == null) {
            throw new IllegalArgumentException("property = null");
        }

        identifier = findProperty(property);

        if (identifier != -1) {
            int[] actualLength = new int[1];
            char[] buffer = new char[64];
            int error = MethodAutogen.WNFCControllerGetProperty(identifier, buffer, actualLength);

            if (error != ConstantAutogen.W_SUCCESS) {
                throw new IllegalArgumentException("Invalid property identifier");
            }

            return new String(buffer, 0, actualLength[0]);
        }

        identifier = findSpecialProperty(property);

        int index = -1;
        switch (identifier) {
            case ACTIVITY_READER:
                index = 0;
                break;
            case ACTIVITY_CARD:
                index = 1;
                break;
            case ACTIVITY_P2P:
                index = 2;
                break;
        }

        if (index >= 0) {
            int[][] array = new int[3][1];
            MethodAutogen.WNFCControllerGetRFActivity(array[0], array[1], array[2]);
            switch (array[index][0]) {
                case ConstantAutogen.W_NFCC_RF_ACTIVITY_ACTIVE:
                    return ACTIVITY_ACTIVE;
                case ConstantAutogen.W_NFCC_RF_ACTIVITY_DETECTION:
                    return ACTIVITY_DETECTION;
                case ConstantAutogen.W_NFCC_RF_ACTIVITY_INACTIVE:
                    return ACTIVITY_INACTIVE;
            }
        }
        throw new IllegalArgumentException("Invalid property identifier");
    }

    /**
     * Returns the value of a boolean property. For the definition of the
     * properties, see <a href="{@docRoot}overview-summary.html#nfc_controller_properties">NFC Properties</a>.
     *
     * @param property the identifier of the property.
     *
     * @return the property value. false if the property is not a boolean property.
     *
     * @throws IllegalArgumentException if the property is null.
     * @throws SecurityException if the calling application is not allowed to access this property.
     * @since Open NFC 4.0
     */
    public boolean getBooleanProperty(String property) {

        int index = -1;
        int store = 0;

        if (property == null) {
            throw new IllegalArgumentException("property = null");
        }

        int identifier = findProperty(property);

        if (identifier != -1) {
            return MethodAutogen.WJavaNFCControllerGetBooleanProperty(identifier);
        }

        identifier = findSpecialProperty(property);

        switch (identifier) {
            case PERSIST_LOCK_READER:
                index = 0;
                store = ConstantAutogen.W_NFCC_STORAGE_PERSISTENT;
                break;
            case PERSIST_LOCK_CARD:
                index = 1;
                store = ConstantAutogen.W_NFCC_STORAGE_PERSISTENT;
                break;
            case VOLATILE_LOCK_READER:
                index = 0;
                store = ConstantAutogen.W_NFCC_STORAGE_VOLATILE;
                break;
            case VOLATILE_LOCK_CARD:
                index = 1;
                store = ConstantAutogen.W_NFCC_STORAGE_VOLATILE;
                break;
        }

        if (index >= 0) {
            int[][] array = new int[2][1];
            MethodAutogen.WNFCControllerGetRFLock(store, array[0], array[1]);
            return (array[index][0] != 0);
        }

        throw new IllegalArgumentException("Invalid property identifier");
    }

    /**
     * Returns the value of an integer property. For the definition of the
     * properties, see <a href="{@docRoot}overview-summary.html#nfc_controller_properties">NFC Properties</a>.
     *
     * @param property the identifier of the property.
     *
     * @return the property value.
     *
     * @throws IllegalArgumentException if the property is null.
     * @throws NumberFormatException if the property is not an integer property.
     * @throws SecurityException if the calling application is not allowed to access this property.
     * @since Open NFC 4.0
     */
    public int getIntegerProperty(String property) {

        int[] actualLength = new int[1];
        char[] buffer = new char[64];

        if (property == null) {
            throw new IllegalArgumentException("property = null");
        }

        int identifier = findProperty(property);

        if (identifier != -1) {
            int error = MethodAutogen.WNFCControllerGetProperty(identifier, buffer, actualLength);
            if (error != ConstantAutogen.W_SUCCESS) {
                throw new IllegalArgumentException("Error retreiving the value");
            }
            return Integer.parseInt(new String(buffer, 0, actualLength[0]));
        }

        identifier = findSpecialProperty(property);

        if (identifier == PULSE_PERIOD) {
            return getPulsePeriod();
        }

        throw new IllegalArgumentException("Invalid property identifier");
    }

    /**
     * Sets the value of a boolean property. For the definition of the
     * properties, see <a href="{@docRoot}overview-summary.html#nfc_controller_properties">NFC Properties</a>.
     *
     * @param property the identifier of the property.
     *
     * @param value the property value.
     *
     * @throws IllegalArgumentException if the property is null.
     * @throws IllegalArgumentException if the property is not a boolean property.
     * @throws IllegalArgumentException if the property is read-only.
     * @throws SecurityException if the calling application is not allowed to access this property.
     * @since Open NFC 4.0
     */
    public void setProperty(String property, boolean value) {

        int index = -1;
        int store = 0;

        if (property == null) {
            throw new IllegalArgumentException("property = null");
        }

        int identifier = findSpecialProperty(property);

        switch (identifier) {
            case PERSIST_LOCK_READER:
                index = 0;
                store = ConstantAutogen.W_NFCC_STORAGE_PERSISTENT;
                break;
            case PERSIST_LOCK_CARD:
                index = 1;
                store = ConstantAutogen.W_NFCC_STORAGE_PERSISTENT;
                break;
            case VOLATILE_LOCK_READER:
                index = 0;
                store = ConstantAutogen.W_NFCC_STORAGE_VOLATILE;
                break;
            case VOLATILE_LOCK_CARD:
                index = 1;
                store = ConstantAutogen.W_NFCC_STORAGE_VOLATILE;
                break;
        }

        if (index >= 0) {
            int[][] array = new int[2][1];
            MethodAutogen.WNFCControllerGetRFLock(store, array[0], array[1]);
            if (value != (array[index][0] != 0)) {
                array[index][0] = value
                        ? 1
                        : 0;

                CallbackGenericCompletion callback = new CallbackGenericCompletion();

                MethodAutogen.WNFCControllerSetRFLock(store, (array[0][0] != 0), (array[1][0] != 0), callback.getId());
                callback.waitForCompletion();
            }
        }

        throw new IllegalArgumentException("Invalid property identifier");
    }

    /**
     * Sets the value of an integer property. For the definition of the
     * properties, see <a href="{@docRoot}overview-summary.html#nfc_controller_properties">NFC Properties</a>.
     *
     * @param property the identifier of the property.
     *
     * @param value the property value.
     *
     * @throws IllegalArgumentException if the property is null.
     * @throws IllegalArgumentException if the property is not an integer property.
     * @throws IllegalArgumentException if the property is read-only.
     * @throws SecurityException if the calling application is not allowed to access this property.
     * @since Open NFC 4.0
     */
    public void setProperty(String property, int value) {

        if (property == null) {
            throw new IllegalArgumentException("property = null");
        }

        int identifier = findSpecialProperty(property);

        if (identifier == PULSE_PERIOD) {
            try {
                setPulsePeriod(value);
            } catch (Exception e) {
                throw new IllegalArgumentException("Set Pulse Priod error - " + e.getMessage());
            }
        }

        throw new IllegalArgumentException("Invalid property identifier");
    }

    /**
     * Returns the single instance of NFC Tag Manager.
     * 
     * @return the NFC Tag Manager.
     */
    public NfcTagManager getNfcTagManager() {
        synchronized (this) {
            if (this.mInstanceNfcTagManager == null) {
                this.mInstanceNfcTagManager = new NfcTagManagerImpl();
            }
            return this.mInstanceNfcTagManager;
        }
    }

    /**
     * Returns the single instance of P2P Manager.
     * 
     * @return the P2P Manager.
     *
     * @throws NfcException if the platform doesn't support P2P feature.
     * @throws SecurityException if the calling application is not allowed to use the Peer to Peer protocol.
     * @since Open NFC 4.1
     */
    public P2PManager getP2PManager() throws NfcException {

        NfcSecurityManager.check(NfcSecurityManager.P2P);

        if (getBooleanProperty(P2P_SUPPORTED)) {
            if (this.mInstanceP2PManager == null) {
                this.mInstanceP2PManager = new P2PManagerImpl();
            }
            return this.mInstanceP2PManager;
        }
        throw new NfcException("The P2P feature is not supported.");
    }

    /**
     * Returns the single instance of the card listener registry.
     * 
     * @return the card listener registry.
     *
     * @throws SecurityException if the calling application is not allowed to listen to cards.
     */
    public CardListenerRegistry getCardListenerRegistry() {

        NfcSecurityManager.check(NfcSecurityManager.CARD_LISTEN);

        synchronized (this) {
            if (this.mInstanceCardListenerRegistry == null) {
                this.mInstanceCardListenerRegistry = new CardListenerRegistryImpl();
            }
            return this.mInstanceCardListenerRegistry;
        }
    }

    /**
     * Returns the single instance of the card emulation registry.
     * 
     * @return the card emulation registry.
     *
     * @throws SecurityException if the calling application is not allowed to emulate cards.
     *
     * @since Open NFC 4.1
     */
    public CardEmulationRegistry getCardEmulationRegistry() {

        NfcSecurityManager.check(NfcSecurityManager.CARD_EMUL);

        synchronized (this) {
            if (this.mInstanceCardEmulationRegistry == null) {
                this.mInstanceCardEmulationRegistry = new CardEmulationRegistryImpl();
            }
            return this.mInstanceCardEmulationRegistry;
        }
    }

    /**
     * Returns the single instance of NFC Controller.
     * 
     * @return the NFC Controller instance.
     *
     * @throws SecurityException if the calling application is not allowed to access hardware.
     */
    public NfcController getNfcController() {

        NfcSecurityManager.check(NfcSecurityManager.HARDWARE);

        synchronized (this) {
            if (this.mInstanceNfcController == null) {
                this.mInstanceNfcController = new NfcControllerImpl();
            }
            return this.mInstanceNfcController;
        }
    }

    /**
     * package private
     * 
     * @hide
     **/
    static void checkErrorAndException(int error) throws NfcException {

        if (error == ConstantAutogen.W_SUCCESS) {
            return;
        } else if (error == ConstantAutogen.W_ERROR_BAD_NFCC_MODE) {
            throw new IllegalStateException("The requested operation is not valid with the current mode of the NFC Controller");
        } else if (error == ConstantAutogen.W_ERROR_BAD_STATE) {
            throw new IllegalStateException("Bad state for this operation");
        } else if ((error == ConstantAutogen.W_ERROR_BAD_HANDLE) || (error == ConstantAutogen.W_ERROR_CANCEL)) {
            throw new IllegalStateException("The connection is closed");
        } else if (error == ConstantAutogen.W_ERROR_OUT_OF_RESOURCE) {
            throw new OutOfMemoryError("Error allocating resources");
        } else if (error == ConstantAutogen.W_ERROR_BAD_PARAMETER) {
            throw new IllegalArgumentException("Error in the argument value");
        } else {
            throw new NfcException(NfcErrorCode.getCode(error));
        }
    }

    /**
     * package private
     * 
     * @hide
     **/
    static void checkError(int error) {

        try {
            checkErrorAndException(error);
        } catch (NfcException e) {
            throw new InternalError("NFC: " + e.getMessage());
        }
    }

    /**
     * package private
     * 
     * @hide
     **/
    static byte[] copyArray(byte[] array) {
        byte[] copy = null;
        if (array != null) {
            int length = array.length;
            copy = new byte[length];
            if (length != 0) {
                System.arraycopy(array, 0, copy, 0, length);
            }
        }
        return copy;
    }
}

/**
 * package private
 * 
 * @hide
 **/
final class CallbackGenericCompletion extends Callback_tWBasicGenericCallbackFunction {

    /* package private */CallbackGenericCompletion() {
        super(false);
    }

    @Override
    protected void tWBasicGenericCallbackFunction(int result) {
        if (result == ConstantAutogen.W_SUCCESS) {
            signalCompletion();
        } else if (result == ConstantAutogen.W_ERROR_BAD_NFCC_MODE) {
            signalException(new IllegalStateException(
                    "The requested operation is not valid with the current mode of the NFC Controller"));
        } else if (result == ConstantAutogen.W_ERROR_BAD_STATE) {
            signalException(new IllegalStateException("Bad state for this operation"));
        } else if ((result == ConstantAutogen.W_ERROR_BAD_HANDLE) || (result == ConstantAutogen.W_ERROR_CANCEL)) {
            signalException(new IllegalStateException("The connection is closed"));
        } else if (result == ConstantAutogen.W_ERROR_OUT_OF_RESOURCE) {
            signalException(new OutOfMemoryError("Error allocating resources"));
        } else if (result == ConstantAutogen.W_ERROR_BAD_PARAMETER) {
            signalException(new IllegalArgumentException("Error in the argument value"));
        } else {
            signalException(new NfcException(NfcErrorCode.getCode(result)));
        }
    }
}

/**
 * @hide
 */
final class CallbackMyDMoveGetConfigurationCompleted extends Callback_tWMyDMoveGetConfigurationCompleted {
    int nStatusByte;
    int nPasswordRetryCounter;

    CallbackMyDMoveGetConfigurationCompleted() {
        super(false);
    }

    @Override
    protected void tWMyDMoveGetConfigurationCompleted(int nError, int nStatusByte, int nPasswordRetryCounter) {
        switch (nError) {
            case ConstantAutogen.W_SUCCESS:
                this.nStatusByte = nStatusByte;
                this.nPasswordRetryCounter = nPasswordRetryCounter;
                signalCompletion();
                break;

            case ConstantAutogen.W_ERROR_BAD_NFCC_MODE:
                signalException(new IllegalStateException(
                        "The requested operation is not valid with the current mode of the NFC Controller"));
                break;

            case ConstantAutogen.W_ERROR_BAD_STATE:
                signalException(new IllegalStateException("Bad state for this operation"));
                break;

            case ConstantAutogen.W_ERROR_BAD_HANDLE:
            case ConstantAutogen.W_ERROR_CANCEL:
                signalException(new IllegalStateException("The connection is closed"));
                break;

            case ConstantAutogen.W_ERROR_OUT_OF_RESOURCE:
                signalException(new OutOfMemoryError("Error allocating resources"));
                break;

            case ConstantAutogen.W_ERROR_BAD_PARAMETER:
                signalException(new IllegalArgumentException("Error in the argument value"));
                break;

            default:
                signalException(new NfcException(NfcErrorCode.getCode(nError)));
                break;
        }
    }
}

/**
 * package private
 * 
 * @hide
 **/
final class CallbackGenericDataCompletion extends Callback_tWBasicGenericDataCallbackFunction {

    public CallbackGenericDataCompletion() {
        super(false);
    }

    @Override
  protected void tWBasicGenericDataCallbackFunction(int dataLength, int error) {

      if(error == ConstantAutogen.W_SUCCESS) {
          signalResult(dataLength);
      } else if (error == ConstantAutogen.W_ERROR_SECURITY ) {
          signalException(new SecurityException("A security error is detected"));
      } else if (error == ConstantAutogen.W_ERROR_BAD_NFCC_MODE ) {
          signalException(new IllegalStateException("The requested operation is not valid with the current mode of the NFC Controller"));
      } else if (error == ConstantAutogen.W_ERROR_BAD_STATE) {
          signalException(new IllegalStateException("Bad state for this operation"));
      } else if ((error == ConstantAutogen.W_ERROR_BAD_HANDLE) || (error == ConstantAutogen.W_ERROR_CANCEL)) {
          signalException(new IllegalStateException("The connection is closed"));
      } else if (error == ConstantAutogen.W_ERROR_OUT_OF_RESOURCE) {
          signalException(new OutOfMemoryError("Error allocating resources"));
      } else if (error == ConstantAutogen.W_ERROR_BAD_PARAMETER) {
          signalException(new IllegalArgumentException("Error in the argument value"));
      } else {
          signalException(new NfcException(NfcErrorCode.getCode(error)));
      }
  }
}
