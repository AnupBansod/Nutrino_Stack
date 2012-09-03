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

import org.opennfc.HelperForNfc;
import org.opennfc.NfcException;
import org.opennfc.service.communication.ClientServerCache;
import org.opennfc.service.communication.ConnectionlessSocketLLCP;
import org.opennfc.service.communication.ILlcpServiceSocketImpl;
import org.opennfc.service.communication.ILlcpSocketImpl;

import android.app.PendingIntent;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.SharedPreferences;
import android.nfc.ErrorCodes;
import android.nfc.ILlcpConnectionlessSocket;
import android.nfc.ILlcpServiceSocket;
import android.nfc.ILlcpSocket;
import android.nfc.INfcAdapter;
import android.nfc.INfcSecureElement;
import android.nfc.INfcTag;
import android.nfc.IP2pInitiator;
import android.nfc.IP2pTarget;
import android.nfc.NfcAdapter;
import android.nfc.Tag;
import android.nfc.TechListParcel;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.provider.Settings;
import android.util.Log;
import android.widget.Toast;

import java.net.InetAddress;

/**
 * Represents a Open NFC service to communicate with background service
 * {@link OpenNFCBackgoundService}
 */
public class OpenNFCService extends Service {
    /**
     * Handler that we link to service thread to be able show toats or refresh
     * the package list in the good thread
     */
    final class HandlerOpenNFCService extends Handler {
        /**
         * Call when receive a message<br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param message Message received
         * @see Handler#handleMessage(Message)
         */
        @Override
        public void handleMessage(final Message message) {
            switch (message.arg1) {
                case R.id.OpenNfcMessageToast:
                    Toast.makeText(OpenNFCService.this, (String) message.obj, message.arg2).show();
                    break;
                case R.id.OpenNfcMessageRefreshPackageList:
                    if (OpenNFCService.DEBUG) {
                        Log.v(OpenNFCService.TAG, "Refresh package list");
                    }

                    OpenNFCService.this.mRegisteredPacakgeList.collectPackages();
                    break;
            }
        }
    }

    /** Action name of Open NFC service */
    public static final String ACTION_OPEN_NFC_SERVICE = "org.opennfc.service.OPEN_NFC_SERVICE";

    /** Enable/disable the debug */
    private static final boolean DEBUG = true;

    /** The NFC status by default */
    private static final boolean NFC_ON_DEFAULT = false;

    /** NFC permission name */
    private static final String NFC_PERM = android.Manifest.permission.NFC;
    /** NFC permission error text */
    private static final String NFC_PERM_ERROR = OpenNFCService.NFC_PERM + " permission required";
    /** Shared preference name */
    private static final String PREF = "OpenNFCServicePrefs";
    /** Preference for store if NFC is enable */
    private static final String PREF_NFC_ON = "nfc_on";

    /** Action name for refresh package list */
    public static final String REFRESH_PACKAGE_LIST = "org.opennfc.REFERESH_PACKAGE_LIST";
    /** Client/server URI name for P2P */
    public static final String SERVER_COM_ANDROID_NPP_NAME = "com.android.npp";
    /** RW to use for P2P */
    static final int SERVER_RW = 1;
    /** SAP to use for P2P */
  public  static final int SERVER_SAP = 16;

    /** Service name */
    public static final String SERVICE_NAME = "nfc";
    /** Open NFC service actual instance */
    public static OpenNFCService sOpenNFCService;
    /** Tag for debugging purpose */
    private static final String TAG = OpenNFCService.class.getSimpleName();
    /** Play at class first use */
    static {
        // Load Open NFC driver
        System.loadLibrary("open_nfc_server_jni");
    }

    /** Handler is service thread to able show toast */
    HandlerOpenNFCService handlerOpenNFCService;
    /** Link for communicate with background service */
    IOpenNFCBackgroundService iOpenNFCBackgroundService;

    /**
     * The IOpenNFCService Interface is defined through IDL
     */
    final IOpenNFCService.Stub mBinder = new IOpenNFCService.Stub() {
        /**
         * Configures the Open NFC stack<br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param config Configuration to use
         * @return {@code true} if configuration succeed. {@code false} if
         *         failed
         * @see IOpenNFCService#configureNFC(byte[])
         */
        @Override
        public boolean configureNFC(final byte[] config) {
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG, "IOpenNFCService:configureNFC(" + config + ")");
            }

            OpenNFCService.this.mConfig = config;
            return true;
        }

        /**
         * Prepare the connection to control card.<br>
         * It use the system preferences : {@code nfc_cc_ip} (IP address) and
         * {@code nfc_hal_mode} (Mode to use for HAL) to fill the 'mConfig'
         * field.<br>
         * If one of those preferences missing or invalid, the configuration
         * will failed and 'mConfig' not filled.
         * 
         * @return {@code true} if configuration succed. {@code false} if failed
         */
        private boolean connecttoCC() {
            // If already connected, the configuration succed ;)
            if (OpenNFCService.this.mIsServiceAlreadyConnected == true) {
                return true;
            }

            final ContentResolver contentResolver = OpenNFCService.this.getContentResolver();

            // Obtain IP address from system preferences
            final String addressIP = Settings.System.getString(contentResolver, "nfc_cc_ip");

            if (addressIP != null) {
                try {
                    final InetAddress inetAddress =
                            InetAddress.getByName(addressIP);

                    if (inetAddress == null || inetAddress.isReachable(1000) ==
                            false)
                    {
                        if (OpenNFCService.DEBUG) {
                            Log.d(OpenNFCService.TAG, "Invalid IP address");
                        }

                        return false;
                    }
                }
                catch (final Exception exception)
                {
                    if (OpenNFCService.DEBUG) {
                        Log.d(OpenNFCService.TAG, "Invalid IP address");
                    }
                    return false;
                }

                // Obtain HAL mode from system preferences
                final int halMode = Settings.System.getInt(contentResolver, "nfc_hal_mode", -1);

                if (halMode >= 0) {
                    // Create configuration string :
                    // config := <HAL_Mode><Address_IP>0
                    // HAL_MODE := Hardware: | Simulator:
                    // Address_IP := [0-255].[0-255].[0-255].[0-255]

                    // 26 = 10 (MAX_LENGTH{"Hardware:", "Simulator:"}) +
                    // 4*3([0,
                    // 255]) + 3*1(.) + 1(0)
                    final StringBuilder stringBuilder = new StringBuilder(26);

                    switch (halMode) {
                        case 0:
                        stringBuilder.append("Hardware:");
                        break;
                    case 1:
                        stringBuilder.append("Simulator:");
                        break;
                }

                stringBuilder.append(addressIP);
                stringBuilder.append('\u0000');

                final String config = stringBuilder.toString();

                if (OpenNFCService.DEBUG) {
                    Log.d(OpenNFCService.TAG, "ConnecttoCC : " + config);
                }

                // Fill the 'mConfig' field
                if (this.configureNFC(config.getBytes()) == false) {
                    if (OpenNFCService.DEBUG) {
                        Log.d(OpenNFCService.TAG, "connecttoCC : OpenNFC configuration failed !!!!");
                    }
                } else {

                    if (OpenNFCService.DEBUG) {
                        Log.d(OpenNFCService.TAG,
                                "connecttoCC : OpenNFC has been successfully started");
                    }

                    OpenNFCService.this.mIsServiceAlreadyConnected = true;
                }
            } else {
                if (OpenNFCService.DEBUG) {
                    Log.d(OpenNFCService.TAG, "connecttoCC : Please select a HAL mode !");
                }

                OpenNFCService.this.toast("Please select a HAL mode !", Toast.LENGTH_LONG);
            }
        } else {
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG, "connecttoCC : Please type a valid IP address !");
            }

            OpenNFCService.this.toast("Please type a valid IP address !", Toast.LENGTH_LONG);
        }

        return OpenNFCService.this.mIsServiceAlreadyConnected;
    }

        /**
         * Returns the current state of the Open NFC stack<br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @return {@code true} if NFC is enable
         * @see IOpenNFCService#getNFCEnabled()
         */
        @Override
        public boolean getNFCEnabled() {
            return OpenNFCService.this.mIsRunning;
        }

        /**
         * Enable / Disable the Open NFC stack<br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param enable New desired Open NFC stack status
         * @return {@code true} if operation to change status succeed and
         *         {@code false} if failed
         * @see IOpenNFCService#setNFCEnabled(boolean)
         */
        @Override
        public boolean setNFCEnabled(final boolean enable) {
            boolean ret;

            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG, "IOpenNFCService:setNFCEnabled(" + enable + ")");
            }
            if (enable == true) {
                // We want enable NFC service

                // First we test we can connect to card controller
                // The following method call fill the parameter 'mConfig'
                if (this.connecttoCC() == false) {
                    // Connection preparation failed (Usually, because miss some
                    // parameters)
                    Log.d("OpenNFCService", "can't create configuration !");

                    ret = false;
                    OpenNFCService.this.mIsRunning = false;

                    OpenNFCService.this.mPrefsEditor.putBoolean(OpenNFCService.PREF_NFC_ON, false);
                    OpenNFCService.this.mPrefsEditor.apply();
                }
                // connecttoCC succeed so 'mConfig' is correctly fill and
                // available to be use
                else if (Interface.OpenNFCServerStart(OpenNFCService.this.mConfig) == 0) {
                    // We manage to start the connection with card controller
                    Log.d("OpenNFCService", "OpenNFCServerStart succeeded");
                    OpenNFCService.this.mIsRunning = true;
                    ret = true;

                    if (OpenNFCService.this.mIsBound == false) {
                        // We start background service if need
                        if (OpenNFCService.DEBUG) {
                            Log.d(OpenNFCService.TAG,
                                    "Bind Backgroud service *******************************************************************");
                        }
                        OpenNFCService.this.bindService();
                    }

                    OpenNFCService.this.mPrefsEditor.putBoolean(OpenNFCService.PREF_NFC_ON, true);
                    OpenNFCService.this.mPrefsEditor.apply();
                } else {
                    // Connection with card controller failed
                    Log.e("OpenNFCService", "OpenNFCServerStart failed");
                    ret = false;
                    OpenNFCService.this.mIsRunning = false;
                    OpenNFCService.this.mPrefsEditor.putBoolean(OpenNFCService.PREF_NFC_ON, false);
                    OpenNFCService.this.mPrefsEditor.apply();
                }
            } else {
                // We want disable NFC service
                Log.d("OpenNFCService", "Will disable");
                OpenNFCService.this.registerReceiver(OpenNFCService.this.mReceiver,
                        new IntentFilter(NfcAdapter.ACTION_ADAPTER_STATE_CHANGE));

                // Disconnect the background service, if need
                if (OpenNFCService.this.mIsBound == true) {
                    Log.d("OpenNFCService", "Unbind");
                    OpenNFCService.this.unbindService();
                }

                Log.d("OpenNFCService", "Disabled");

                OpenNFCService.this.mPrefsEditor.putBoolean(OpenNFCService.PREF_NFC_ON, false);
                OpenNFCService.this.mPrefsEditor.apply();

                ret = true;
            }

            return ret;
        }
    };

    /**
     * Closing Open NFC communication
     */
    final ThreadedTask<Object> mCloseOpenNFCCommunication = new ThreadedTask<Object>() {
        /**
         * Cancel the task <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @see org.opennfc.service.ThreadedTask#cancel()
         */
        @Override
        public void cancel() {
            // Nothing to do here
        }

        /**
         * Closing Open NFC communication<br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param taskID Unused
         * @param parameters Unused
         * @see ThreadedTask#excuteTask(int, Object...)
         */
        @Override
        public void excuteTask(final int taskID, final Object... parameters) {
            Log.d(OpenNFCService.TAG, "I stop the server !!! ", new Throwable());

            try {
                Interface.OpenNFCServerStop();
            } catch (final Exception exception) {
                Log.e(OpenNFCService.TAG, "It is may be an issue", exception);
            } catch (final Error error) {
                Log.e(OpenNFCService.TAG, "It is may be an issue", error);
            }
        }
    };
    /** Actual configuration */
    byte[] mConfig = null;
    /** Flag indicating whether we have called bind on the service. */
    boolean mIsBound = false;
    /** Indicates if NFC is enable */
    volatile boolean mIsNfcEnabled = false;

    /** Indicates if service running */
    boolean mIsRunning = false;
    /** Indicates if the service is already connected */
    private boolean mIsServiceAlreadyConnected = false;

    /** NFC adapter that implements the AIDL */
    public final INfcAdapter.Stub mNfcAdapter = new INfcAdapter.Stub() {
        /**
         * TODO will be implements later <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param sap Unused
         * @return 0
         * @throws RemoteException On broken link
         * @see android.nfc.INfcAdapter#createLlcpConnectionlessSocket(int)
         */
        @Override
        public int createLlcpConnectionlessSocket(final int sap) throws RemoteException {
            try {
                return ConnectionlessSocketLLCP.CONNECTION.createConnection(null, sap);
            } catch (final NfcException exception) {
                if (OpenNFCService.DEBUG) {
                    Log.d(OpenNFCService.TAG, "createLlcpConnectionlessSocket", exception);
                }

                return HelperForNfc.obtainErrorCode(exception, ErrorCodes.ERROR_SOCKET_CREATION);
            }
        }

        /**
         * Create a server socket base on LLCP <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param sap SAP to connect
         * @param sn Service name or URI
         * @param miu MIU to use
         * @param rw RW to use
         * @param linearBufferLength Buffer size
         * @return Handle on the created server
         * @throws RemoteException On broken link
         * @see android.nfc.INfcAdapter#createLlcpServiceSocket(int,
         *      java.lang.String, int, int, int)
         */
        @Override
        public int createLlcpServiceSocket(final int sap, final String sn, final int miu,
                final int rw,
                final int linearBufferLength) throws RemoteException {
            return ClientServerCache.CACHE.createClientServer(sn, sap, false);
        }

        /**
         * Create a client socket base on LLCP <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param sap SAP to connect
         * @param miu MIU to use
         * @param rw RW to use
         * @param linearBufferLength Buffer size
         * @return Handle on created client
         * @throws RemoteException On broken link
         * @see android.nfc.INfcAdapter#createLlcpSocket(int, int, int, int)
         */
        @Override
        public int createLlcpSocket(final int sap, final int miu, final int rw,
                final int linearBufferLength)
                throws RemoteException {
            return ClientServerCache.CACHE.createClientServer(null, sap, false);
        }

        /**
         * TODO will be implements later <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @return 0
         * @throws RemoteException On broken link
         * @see android.nfc.INfcAdapter#deselectSecureElement()
         */
        @Override
        public int deselectSecureElement() throws RemoteException {
            // TODO org.opennfc.service.OpenNFCService.mNfcAdapter.new Stub()
            // {...}.deselectSecureElement ()
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG,
                        "TODO : org.opennfc.service.OpenNFCService.mNfcAdapter.new Stub() {...}.deselectSecureElement ()");
            }

            return 0;
        }

        /**
         * Disable NFC stack<br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @see INfcAdapter#disable()
         */
        @Override
        public boolean disable() throws RemoteException {
            boolean isSuccess = false;
            final boolean previouslyEnabled = this.isEnabled();

            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG, "Disabling NFC.  previous=" + previouslyEnabled);
            }

            // We only disable if NFC stack is enable
            if (previouslyEnabled == true) {
                /* tear down the my tag server */
                /**
                 * Deinitializes NFC stack.
                 */

                isSuccess = OpenNFCService.this._enable(false);
                if (OpenNFCService.DEBUG) {
                    Log.d(OpenNFCService.TAG, "NFC success of deinitialize = " + isSuccess);
                }

                if (isSuccess == true) {
                    OpenNFCService.this.mIsNfcEnabled = false;
                }
            }

            OpenNFCService.this.updateNfcOnSetting(previouslyEnabled);
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG, "$$$NFC$$$ Will disconnect $$$NFC$$$");
            }

            return isSuccess;
        }

        /**
         * Disable foreground dispaching <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param activity Activity parent
         * @throws RemoteException On broken link
         * @see android.nfc.INfcAdapter#disableForegroundDispatch(android.content.ComponentName)
         */
        @Override
        public void disableForegroundDispatch(final ComponentName activity) throws RemoteException {
            OpenNFCService.this.enforceCallingOrSelfPermission(OpenNFCService.NFC_PERM,
                    OpenNFCService.NFC_PERM_ERROR);

            synchronized (this) {
                if (OpenNFCBackgoundService.mDispatchOverrideIntent == null) {
                    Log.e(OpenNFCService.TAG, "No active foreground dispatching");
                }
                OpenNFCBackgoundService.mDispatchOverrideIntent = null;
                OpenNFCBackgoundService.mDispatchOverrideFilters = null;
            }
        }

        /**
         * Disable the P2P communication <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param activity Activity reference
         * @throws RemoteException On broken link
         * @see android.nfc.INfcAdapter#disableForegroundNdefPush(android.content.ComponentName)
         */
        @Override
        public void disableForegroundNdefPush(final ComponentName activity) throws RemoteException {
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG,
                        "disableForegroundNdefPush disableForegroundNdefPush disableForegroundNdefPush disableForegroundNdefPush disableForegroundNdefPush disableForegroundNdefPush disableForegroundNdefPush");
            }
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG, "REGISTER");
            }

            // final ClientServer clientServer = ClientServerCache.CACHE
            // .get(OpenNFCService.this.server_com_android_npp);
            // if (clientServer != null) {
            // clientServer.disconnect();
            // }
            //
            // OpenNFCService.this.iOpenNFCBackgroundService.register();
        }

        /**
         * Enable NFC stack<br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @see INfcAdapter#enable()
         */
        @Override
        public boolean enable() throws RemoteException {
            boolean isSuccess = false;
            final boolean previouslyEnabled = this.isEnabled();

            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG, "Enabling NFC.  previous =" + previouslyEnabled);
            }

            // We only enable if NFC stack is disable
            if (previouslyEnabled == false) {
                OpenNFCService.this.reset();

                if (OpenNFCService.DEBUG) {
                    Log.d(OpenNFCService.TAG, "mIsNfcEnabled NFC ="
                            + OpenNFCService.this.mIsNfcEnabled);
                }

                isSuccess = OpenNFCService.this._enable(true);

                if (OpenNFCService.DEBUG) {
                    Log.d(OpenNFCService.TAG, "NFC success of _enable" + !previouslyEnabled
                            + ") is " + isSuccess);
                }
            }
            // if (server_com_android_npp < 0) {
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG, "$$$NFC$$$ Will connect ! $$$NFC$$$");
            }

            return isSuccess;
        }

        /**
         * Enable foreground dispach <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param activity Activity parent
         * @param intent Intent to send if tag match
         * @param filters Filters to specify the activity
         * @param techListsParcel List of interested technologies
         * @throws RemoteException On broken link
         * @see android.nfc.INfcAdapter#enableForegroundDispatch(android.content.ComponentName,
         *      android.app.PendingIntent, android.content.IntentFilter[],
         *      android.nfc.TechListParcel)
         */
        @Override
        public void enableForegroundDispatch(final ComponentName activity,
                final PendingIntent intent,
                 IntentFilter[] filters, final TechListParcel techListsParcel)
                throws RemoteException {
            OpenNFCService.this.enforceCallingOrSelfPermission(OpenNFCService.NFC_PERM,
                    OpenNFCService.NFC_PERM_ERROR);

            if (activity == null)
            {
                throw new NullPointerException("activity musn't be null !");
            }

            if (intent == null)
            {
                throw new NullPointerException("intent musn't be null !");
            }

            if (filters != null)
            {
                if (filters.length == 0)
                {
                    filters = null;
                }
                else
                {
                    for (int i = filters.length - 1; i >= 0; i--)
                    {
                        if (filters[i] == null)
                        {
                            throw new NullPointerException("The filter at index " + i
                                    + " is null !");
                        }
                    }
                }
            }

            String[][] techLists = null;
            if (techListsParcel != null) {
                techLists = techListsParcel.getTechLists();
            }

            synchronized (this) {
                if (OpenNFCBackgoundService.mDispatchOverrideIntent != null) {
                    Log.e(OpenNFCService.TAG, "Replacing active dispatch overrides");
                }
                OpenNFCBackgoundService.mDispatchOverrideIntent = intent;
                OpenNFCBackgoundService.mDispatchOverrideFilters = filters;
                OpenNFCBackgoundService.mDispatchOverrideTechLists = techLists;
            }
        }

        /**
         * Enable (if need) the P2P connection and push a message to write <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param activity Activity linked
         * @param msg Message to send
         * @throws RemoteException On broken link
         * @see android.nfc.INfcAdapter#enableForegroundNdefPush(android.content.ComponentName,
         *      android.nfc.NdefMessage)
         */
        @Override
        public void enableForegroundNdefPush(final ComponentName activity,
                final android.nfc.NdefMessage msg)
                throws RemoteException {
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG,
                        "enableForegroundNdefPush enableForegroundNdefPush enableForegroundNdefPush enableForegroundNdefPush enableForegroundNdefPush enableForegroundNdefPush enableForegroundNdefPush");
            }
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG, "UNREGISTER");
            }
            // OpenNFCService.this.iOpenNFCBackgroundService.unregister();
            //
            // ClientServer clientServer = ClientServerCache.CACHE
            // .get(OpenNFCService.this.server_com_android_npp);
            // if (clientServer == null)
            // {
            // OpenNFCService.this.server_com_android_npp =
            // ClientServerCache.CACHE
            // .createClientServer(
            // OpenNFCService.SERVER_COM_ANDROID_NPP_NAME,
            // OpenNFCService.SERVER_SAP, true);
            // clientServer = ClientServerCache.CACHE
            // .get(OpenNFCService.this.server_com_android_npp);
            // }
            //
            // clientServer.connect();
            ClientServerCache.CACHE.post(OpenNFCService.SERVER_COM_ANDROID_NPP_NAME,
                    OpenNFCService.SERVER_SAP, msg);
        }

        /**
         * Obtain the connection less manager <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @return Connection less manager
         * @throws RemoteException On broken link
         * @see android.nfc.INfcAdapter#getLlcpConnectionlessInterface()
         */
        @Override
        public ILlcpConnectionlessSocket getLlcpConnectionlessInterface() throws RemoteException
        {
            return ConnectionlessSocketLLCP.CONNECTION;
        }

        /**
         * Obtain the client socket LLCP manager <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @return Client socket LLCP manager
         * @throws RemoteException On broken link
         * @see android.nfc.INfcAdapter#getLlcpInterface()
         */
        @Override
        public ILlcpSocket getLlcpInterface() throws RemoteException {
            return ILlcpSocketImpl.SOCKET;
        }

        /**
         * Obtain the server socket LLCP manager <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @return Server socket LLCP manager
         * @throws RemoteException On broken link
         * @see android.nfc.INfcAdapter#getLlcpServiceInterface()
         */
        @Override
        public ILlcpServiceSocket getLlcpServiceInterface() throws RemoteException {
            return ILlcpServiceSocketImpl.SOCKET;
        }

        /**
         * TODO will be implemented later <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @return {@code null}
         * @throws RemoteException On broken link
         * @see android.nfc.INfcAdapter#getNfcSecureElementInterface()
         */
        @Override
        public INfcSecureElement getNfcSecureElementInterface() throws RemoteException {
            // TODO org.opennfc.service.OpenNFCService.mNfcAdapter.new Stub()
            // {...}.getNfcSecureElementInterface ()
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG,
                        "TODO : org.opennfc.service.OpenNFCService.mNfcAdapter.new Stub() {...}.getNfcSecureElementInterface ()");
            }

            return null;
        }

        /**
         * TODO will be implemented later <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @return {@code null} for now
         * @see INfcAdapter#getNfcTagInterface()
         */
        @Override
        public INfcTag getNfcTagInterface() throws RemoteException {
            return null;
        }

        /**
         * TODO will be implemented later <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @return {@code null}
         * @throws RemoteException On broken link
         * @see android.nfc.INfcAdapter#getP2pInitiatorInterface()
         */
        @Override
        public IP2pInitiator getP2pInitiatorInterface() throws RemoteException {
            // TODO org.opennfc.service.OpenNFCService.mNfcAdapter.new Stub()
            // {...}.getP2pInitiatorInterface ()
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG,
                        "TODO : org.opennfc.service.OpenNFCService.mNfcAdapter.new Stub() {...}.getP2pInitiatorInterface ()");
            }

            return null;
        }

        /**
         * TODO will be implemented later <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @return {@code null}
         * @throws RemoteException On broken link
         * @see android.nfc.INfcAdapter#getP2pTargetInterface()
         */
        @Override
        public IP2pTarget getP2pTargetInterface() throws RemoteException {
            // TODO org.opennfc.service.OpenNFCService.mNfcAdapter.new Stub()
            // {...}.getP2pTargetInterface ()
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG,
                        "TODO : org.opennfc.service.OpenNFCService.mNfcAdapter.new Stub() {...}.getP2pTargetInterface ()");
            }

            return null;
        }

        /**
         * TODO will be implemented later <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param param Unused
         * @return {@code Empty string}
         * @throws RemoteException On broken link
         * @see android.nfc.INfcAdapter#getProperties(java.lang.String)
         */
        @Override
        public String getProperties(final String param) throws RemoteException {
            // TODO org.opennfc.service.OpenNFCService.mNfcAdapter.new Stub()
            // {...}.getProperties (String)
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG,
                        "TODO : org.opennfc.service.OpenNFCService.mNfcAdapter.new Stub() {...}.getProperties (String)");
            }

            return "";
        }

        /**
         * TODO will be implemented later <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @return {@code null}
         * @throws RemoteException On broken link
         * @see android.nfc.INfcAdapter#getSecureElementList()
         */
        @Override
        public int[] getSecureElementList() throws RemoteException {
            // TODO org.opennfc.service.OpenNFCService.mNfcAdapter.new Stub()
            // {...}.getSecureElementList ()
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG,
                        "TODO : org.opennfc.service.OpenNFCService.mNfcAdapter.new Stub() {...}.getSecureElementList ()");
            }

            return new int[0];
        }

        /**
         * TODO will be implemented later <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @return 0
         * @throws RemoteException On broken link
         * @see android.nfc.INfcAdapter#getSelectedSecureElement()
         */
        @Override
        public int getSelectedSecureElement() throws RemoteException {
            // TODO org.opennfc.service.OpenNFCService.mNfcAdapter.new Stub()
            // {...}.getSelectedSecureElement ()
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG,
                        "TODO : org.opennfc.service.OpenNFCService.mNfcAdapter.new Stub() {...}.getSelectedSecureElement ()");
            }

            return 0;
        }

        /**
         * Indicates if NFC stack is enable<br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @return {@code true} if NFC stack is enable
         * @throws RemoteException On broken link
         * @see INfcAdapter#isEnabled()
         */
        @Override
        public boolean isEnabled() throws RemoteException {
            return OpenNFCService.this.mIsNfcEnabled;
        }

        /**
         * TODO will be implemented later <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @return {@code null}
         * @throws RemoteException On broken link
         * @see android.nfc.INfcAdapter#localGet()
         */
        @Override
        public android.nfc.NdefMessage localGet() throws RemoteException {
            // TODO org.opennfc.service.OpenNFCService.mNfcAdapter.new Stub()
            // {...}.localGet ()
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG,
                        "TODO : org.opennfc.service.OpenNFCService.mNfcAdapter.new Stub() {...}.localGet ()");
            }

            return null;
        }

        /**
         * TODO will be implemented later <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param message Unused
         * @throws RemoteException On broken link
         * @see android.nfc.INfcAdapter#localSet(android.nfc.NdefMessage)
         */
        @Override
        public void localSet(final android.nfc.NdefMessage message) throws RemoteException {
            // TODO org.opennfc.service.OpenNFCService.localSet (NdefMessage)
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG,
                        "TODO : org.opennfc.service.OpenNFCService.localSet (NdefMessage)");
            }

            return;
        }

        /**
         * TODO will be implemented <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param tag Unused
         * @throws RemoteException On broken link
         * @see android.nfc.INfcAdapter#openTagConnection(android.nfc.Tag)
         */
        @Override
        public void openTagConnection(final Tag tag) throws RemoteException {
            // TODO org.opennfc.service.OpenNFCService.mNfcAdapter.new Stub()
            // {...}.openTagConnection (tag)
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG,
                        "TODO : org.opennfc.service.OpenNFCService.mNfcAdapter.new Stub() {...}.openTagConnection (tag)");
            }

            return;
        }

        /**
         * TODO will be implemented later <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param seId Unused
         * @return 0
         * @throws RemoteException On broken link
         * @see android.nfc.INfcAdapter#selectSecureElement(int)
         */
        @Override
        public int selectSecureElement(final int seId) throws RemoteException {
            // TODO org.opennfc.service.OpenNFCService.mNfcAdapter.new Stub()
            // {...}.selectSecureElement (seId)
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG,
                        "TODO : org.opennfc.service.OpenNFCService.mNfcAdapter.new Stub() {...}.selectSecureElement (seId)");
            }

            return 0;
        }

        /**
         * TODO will be implemented later <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param param Unused
         * @param value Unused
         * @return 0
         * @throws RemoteException On broken link
         * @see android.nfc.INfcAdapter#setProperties(java.lang.String,
         *      java.lang.String)
         */
        @Override
        public int setProperties(final String param, final String value) throws RemoteException {
            // TODO org.opennfc.service.OpenNFCService.mNfcAdapter.new Stub()
            // {...}.setProperties (param, value)
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG,
                        "TODO : org.opennfc.service.OpenNFCService.mNfcAdapter.new Stub() {...}.setProperties (param, value)");
            }

            return 0;
        }
    };

    /** NFC preference storage */
    private SharedPreferences mPrefs;

    /** Modify NFC preferences */
    private SharedPreferences.Editor mPrefsEditor;

    /**
     * Call when {@link NfcAdapter#ACTION_ADAPTER_STATE_CHANGE} append to
     * disconnect JNI part when we are sure that {@link OpenNFCBackgoundService}
     * is terminate
     */
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        /**
         * Call when {@link NfcAdapter#ACTION_ADAPTER_STATE_CHANGE} append to
         * disconnect JNI part when we are sure that
         * {@link OpenNFCBackgoundService} is terminate<br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param context Service context
         * @param intent Intent that launch the action
         * @see BroadcastReceiver#onReceive(Context, Intent)
         */
        @Override
        public void onReceive(final Context context, final Intent intent) {
            if (NfcAdapter.ACTION_ADAPTER_STATE_CHANGE.equals(intent.getAction())) {
                if (intent.getBooleanExtra(NfcAdapter.EXTRA_NEW_BOOLEAN_STATE, false) == false) {
                    // Stop the card controller, if need
                    if (OpenNFCService.this.mIsRunning == true) {
                        // Call to close open NFC communication in separate
                        // thread
                        ThreadedTaskManager.MANAGER
                                .doInSeparateThread(OpenNFCService.this.mCloseOpenNFCCommunication);
                    }

                    OpenNFCService.this.mIsRunning = false;
                }
            }
        }
    };

    /** List of registered packages */
    RegisteredPackageList mRegisteredPacakgeList;

    /**
     * Start open communication with Open NFC
     */
    final ThreadedTask<Object> mStartOpenNFCCommunication = new ThreadedTask<Object>() {
        /**
         * Cancel the task <br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @see org.opennfc.service.ThreadedTask#cancel()
         */
        @Override
        public void cancel() {
            // Nothing to do here
        }

        /**
         * Start open communication with Open NFC<br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param taskID Unused
         * @param parameters Unused
         * @see ThreadedTask#excuteTask(int, Object...)
         */
        @Override
        public void excuteTask(final int taskID, final Object... parameters) {
            try {
                // We wait 10 seconds before enable automaticaly the NFC
                // stack to be sure that all things are ready
                if (OpenNFCService.DEBUG) {
                    Log.d(OpenNFCService.TAG, "Wait 10 seconds before enable ....");
                }

                try {
                    Thread.sleep(10);
                } catch (final Exception e) {
                }

                if (OpenNFCService.DEBUG) {
                    Log.d(OpenNFCService.TAG, ".... Enable now !");
                }

                OpenNFCService.this.mBinder.setNFCEnabled(OpenNFCService.this.mPrefs
                        .getBoolean(OpenNFCService.PREF_NFC_ON,
                                OpenNFCService.NFC_ON_DEFAULT));
            } catch (final Exception exception) {
                if (OpenNFCService.DEBUG) {
                    Log.d(OpenNFCService.TAG,
                            "Exception in enable on thread inside onCreate : ", exception);
                }
            }

            final Message message = Message.obtain(OpenNFCService.this.handlerOpenNFCService);
            message.arg1 = R.id.OpenNfcMessageRefreshPackageList;

            OpenNFCService.this.handlerOpenNFCService.sendMessage(message);
        }
    };
    /** Listener of the connection to {@link OpenNFCBackgoundService} */
    private final ServiceConnection serviceConnection = new ServiceConnection() {
        /**
         * Call when {@link OpenNFCBackgoundService} connect<br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param name Component name description of the connected service
         * @param service Link with service
         * @see ServiceConnection#onServiceConnected(ComponentName, IBinder)
         */
        @Override
        final public void onServiceConnected(final ComponentName name, final IBinder service) {
            if (OpenNFCService.DEBUG) {
                Log.e(OpenNFCService.TAG, "onServiceConnected()");
            }

            OpenNFCService.this.iOpenNFCBackgroundService = IOpenNFCBackgroundService.Stub
                    .asInterface(service);
        }

        /**
         * Call when {@link OpenNFCBackgoundService} disconnect<br>
         * <br>
         * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
         * 
         * @param name Component name description of disconnected service
         * @see ServiceConnection#onServiceDisconnected(ComponentName)
         */
        @Override
        final public void onServiceDisconnected(final ComponentName name) {
            // This is called when the connection with the service has been
            // unexpectedly disconnected
            if (OpenNFCService.DEBUG) {
                Log.e(OpenNFCService.TAG, "onServiceDisconnected()");
            }
        }
    };

    /**
     * Enable / disable the NFC stack
     * 
     * @param enable Desired NFC stack state
     * @return {@code true} if operation succeed. {@code false} if operation
     *         failed
     */
    boolean _enable(final boolean enable) {
        boolean isSuccess = false;

        try {
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG, "_enable =" + enable);
            }

            // try to do the change
            isSuccess = OpenNFCService.this.mBinder.setNFCEnabled(enable);

            if (isSuccess == true) {
                this.mIsNfcEnabled = enable;
            }

        } catch (final android.os.RemoteException re) {
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG, "_enable to status:" + enable + " failed !!!", re);
            }

            this.mIsNfcEnabled = false;
        }

        this.updateNfcOnSetting(this.mIsNfcEnabled);

        return isSuccess;
    }

    /**
     * Connect to {@link OpenNFCBackgoundService}
     */
    final void bindService() {
        if (OpenNFCService.DEBUG) {
            Log.d(OpenNFCService.TAG, "NfcBackgoundService:bindService()");
        }

        this.bindService(new Intent(this, OpenNFCBackgoundService.class), this.serviceConnection,
                Context.BIND_AUTO_CREATE);
        this.mIsBound = true;
    }

    /**
     * Call when an other service bind to this service<br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param intent Intent used to bind
     * @see Service#onBind(Intent)
     */
    @Override
    public IBinder onBind(final Intent intent) {
        if (OpenNFCService.DEBUG) {
            Log.d(OpenNFCService.TAG, "onBind : Action=" + intent.getAction());
        }

        // Select the interface to return.

        if (INfcAdapter.class.getName().equals(intent.getAction())) {
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG, "onBind return INfcAdapter");
            }
            return this.mNfcAdapter;
        }

        if (OpenNFCService.ACTION_OPEN_NFC_SERVICE.equals(intent.getAction())) {
            if (OpenNFCService.DEBUG) {
                Log.d(OpenNFCService.TAG, "onBind return mBinder");
            }
            return this.mBinder;
        }

        return null;
    }

    /**
     * Call when service created.<br>
     * It initialized the service<br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @see Service#onCreate()
     */
    @Override
    public void onCreate() {
        OpenNFCService.sOpenNFCService = this;

        if (OpenNFCService.DEBUG) {
            Log.d(OpenNFCService.TAG, "Starting NFC service");
        }

        this.handlerOpenNFCService = new HandlerOpenNFCService();

        this.mRegisteredPacakgeList = new RegisteredPackageList(this);
        this.mRegisteredPacakgeList.registerFilter(NfcAdapter.ACTION_TECH_DISCOVERED,
                NfcAdapter.ACTION_TECH_DISCOVERED);

        this.mIsRunning = false;

        this.mPrefs = this.getSharedPreferences(OpenNFCService.PREF, Context.MODE_PRIVATE);
        this.mPrefsEditor = this.mPrefs.edit();

        this.mIsNfcEnabled = this.mPrefs.getBoolean(OpenNFCService.PREF_NFC_ON,
                OpenNFCService.NFC_ON_DEFAULT);

        if (OpenNFCService.DEBUG) {
            Log.d(OpenNFCService.TAG, "ServiceManager add " + OpenNFCService.SERVICE_NAME
                        + " service");
        }
        ServiceManager.addService(OpenNFCService.SERVICE_NAME, this.mNfcAdapter);

        // Call open NFC communication in separate thread
        ThreadedTaskManager.MANAGER.doInSeparateThread(this.mStartOpenNFCCommunication);
    }

    /**
     * Call when service destroy<br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @see Service#onDestroy()
     */
    @Override
    public void onDestroy() {
        if (OpenNFCService.DEBUG) {
            Log.d(OpenNFCService.TAG, "onDestroy");
        }
    }

    /**
     * Call when somone rebind <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param intent Intent used for rebind
     * @see android.app.Service#onRebind(android.content.Intent)
     */
    @Override
    public void onRebind(final Intent intent) {
        if (OpenNFCService.DEBUG) {
            Log.d(OpenNFCService.TAG, "onRebind : Action=" + intent.getAction());
        }
    }

    /**
     * Call when service start<br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param intent Intent that launch the service
     * @param flags Additional data about this start request.
     * @param startId A unique integer representing this specific request to
     *            start.
     * @see Service#onStartCommand(Intent, int, int)
     */
    @Override
    public int onStartCommand(final Intent intent, final int flags, final int startId) {
        if (OpenNFCService.DEBUG) {
            Log.d(OpenNFCService.TAG, "onStartCommand");
        }

        if (intent.getBooleanExtra(OpenNFCService.REFRESH_PACKAGE_LIST, false) == true) {
            final Message message = Message.obtain(this.handlerOpenNFCService);
            message.arg1 = R.id.OpenNfcMessageRefreshPackageList;

            this.handlerOpenNFCService.sendMessage(message);
        }

        return Service.START_STICKY;
    }

    /**
     * Call when a connection to the service is lost <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param intent Intent source
     * @return {@code true} for rebind mecanism
     * @see android.app.Service#onUnbind(android.content.Intent)
     */
    @Override
    public boolean onUnbind(final Intent intent) {
        if (OpenNFCService.DEBUG) {
            Log.d(OpenNFCService.TAG, "onUnbind" + intent.getAction());
        }

        return true;
    }

    /**
     * Reset
     */
    private synchronized void reset() {
        // Reset variables
        this.mIsNfcEnabled = false;
    }

    /**
     * Show a toast message in the good thread
     * 
     * @param msg Message to print
     * @param mode One of toast duration : {@link Toast#LENGTH_LONG} or
     *            {@link Toast#LENGTH_SHORT}
     */
    final void toast(final String msg, final int mode) {
        final Message message = Message.obtain(this.handlerOpenNFCService);
        message.arg1 = R.id.OpenNfcMessageToast;
        message.obj = msg;
        message.arg2 = mode;

        this.handlerOpenNFCService.sendMessage(message);
    }

    /**
     * Disconnect to {@link OpenNFCBackgoundService}
     */
    final void unbindService() {
        if (OpenNFCService.DEBUG) {
            Log.d(OpenNFCService.TAG, "NfcBackgoundService:unbindService() ", new Throwable());
        }

        this.unbindService(this.serviceConnection);
        this.mIsBound = false;
    };

    /**
     * Update the settings of NFC and broadcast a
     * {@link NfcAdapter#EXTRA_NEW_BOOLEAN_STATE} if the enable state just
     * changed
     * 
     * @param oldEnabledState Old enable state
     */
    void updateNfcOnSetting(final boolean oldEnabledState) {
        if (OpenNFCService.DEBUG) {
            Log.d(OpenNFCService.TAG, "updateNfcOnSetting: " + oldEnabledState);
        }

        synchronized (this) {
            if (oldEnabledState != this.mIsNfcEnabled) {
                if (OpenNFCService.DEBUG) {
                    Log.d(OpenNFCService.TAG, "Intent ACTION_ADAPTER_STATE_CHANGE: "
                            + this.mIsNfcEnabled);
                }
                final Intent intent = new Intent(NfcAdapter.ACTION_ADAPTER_STATE_CHANGE);
                // Indicates *if* the broadcast is attempted before the boot
                // process has completion, it should
                // only be sent to "registered" receivers. no attempt will be
                // made to launch new process
                intent.setFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
                intent.putExtra(NfcAdapter.EXTRA_NEW_BOOLEAN_STATE, this.mIsNfcEnabled);
                this.sendBroadcast(intent);
            }
        }
    }
}
