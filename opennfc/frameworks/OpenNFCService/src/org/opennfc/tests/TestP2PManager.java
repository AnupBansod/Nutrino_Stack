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

package org.opennfc.tests;

import org.opennfc.HelperForNfc;
import org.opennfc.NfcException;
import org.opennfc.p2p.P2PConnection;
import org.opennfc.p2p.P2PConnection.P2PConnectionParameter;
import org.opennfc.p2p.P2PLink;
import org.opennfc.p2p.P2PLinkEventHandler;
import org.opennfc.p2p.P2PManager;
import org.opennfc.service.ThreadedTask;
import org.opennfc.service.ThreadedTaskManager;

import android.util.Log;

import java.io.IOException;

/**
 * For testing the good way for implement P2P
 */
public class TestP2PManager extends TestScenario implements P2PLinkEventHandler,
        ThreadedTask<Integer> {
    /** Status : Connecting with link */
    private static final int CONNECT_WITH_LINK = 3;
    /** Status : Creating connection */
    private static final int CREATE_CONNECTION = 1;
    /** Enable/disable debug */
    private static final boolean DEBUG = true;
    /** Status : Disconnecting */
    private static final int DISCONNECT = 0;
    /** Status : Doing exchange */
    private static final int DO_EXCHANGE = 4;
    /** Status : Establishing link */
    private static final int ESTABLISH_LINK = 2;
    /** Messages for test */
    private static byte[][] messages = {
                HelperForNfc.stringToByteArray("TOC TOC !"),
                HelperForNfc.stringToByteArray("Who's there ?"),
                HelperForNfc.stringToByteArray("This is a tes !"),
                HelperForNfc.stringToByteArray("Ok, so goodbye !"),
            HelperForNfc.stringToByteArray("Bye!")
        };
    /** Tag use in debug */
    private static final String TAG = TestP2PManager.class.getSimpleName();
    /** Actual link */
    private P2PLink link;
    /** Actual connection */
    private P2PConnection mP2pConnection;

    /**
     * Create tes tester
     */
    public TestP2PManager() {
    }

    /**
     * Call when thread cancel <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @see org.opennfc.service.ThreadedTask#cancel()
     */
    @Override
    public void cancel() {
    }

    /**
     * Do the action with the current status <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param taskID Task ID
     * @param parameters First parameter : current status
     * @see org.opennfc.service.ThreadedTask#excuteTask(int, PARAMETER[])
     */
    @Override
    public void excuteTask(final int taskID, final Integer... parameters) {
        final int status = parameters[0];

        if (TestP2PManager.DEBUG) {
            Log.d(TestP2PManager.TAG, "excuteTask : " + status);
        }

        switch (status) {
            case CREATE_CONNECTION:
                try {
                    this.mP2pConnection = TestUtil.getP2PManager().createP2PConnection(
                            P2PManager.Mode.CLIENT_SERVER, "com.android.npp", (byte) 0x10);

                    if (TestP2PManager.DEBUG) {
                        Log.d(TestP2PManager.TAG, "Connection created : " + this.mP2pConnection);
                    }

                    ThreadedTaskManager.MANAGER.doInSeparateThread(this,
                            TestP2PManager.ESTABLISH_LINK);

                } catch (final NfcException exception) {
                    if (TestP2PManager.DEBUG) {
                        Log.d(TestP2PManager.TAG, "Create connection failed ! ", exception);
                    }

                    ThreadedTaskManager.MANAGER.doInSeparateThread(this, TestP2PManager.DISCONNECT);
                }
                break;

            case ESTABLISH_LINK:
                try {
                    TestUtil.getP2PManager().establishLink(this);

                    if (TestP2PManager.DEBUG) {
                        Log.d(TestP2PManager.TAG, "establishLink launched !");
                    }
                } catch (final NfcException exception) {
                    if (TestP2PManager.DEBUG) {
                        Log.d(TestP2PManager.TAG, "establishLink failed", exception);
                    }

                    switch (exception.getCode()) {
                    }

                    ThreadedTaskManager.MANAGER.doInSeparateThread(this, TestP2PManager.DISCONNECT);
                }

                break;

            case CONNECT_WITH_LINK:
                try {
                    this.mP2pConnection.connect(this.link);

                    if (TestP2PManager.DEBUG) {
                        Log.d(TestP2PManager.TAG, "Connect to link succed !");
                    }

                    ThreadedTaskManager.MANAGER
                            .doInSeparateThread(this, TestP2PManager.DO_EXCHANGE);
                } catch (final IOException exception) {
                    if (TestP2PManager.DEBUG) {
                        Log.d(TestP2PManager.TAG, "Connection link I/O try to create a new link",
                                exception);
                    }

                    ThreadedTaskManager.MANAGER.doInSeparateThread(this,
                            TestP2PManager.ESTABLISH_LINK);
                } catch (final NfcException exception) {
                    if (TestP2PManager.DEBUG) {
                        Log.d(TestP2PManager.TAG, "Connect to link failed !", exception);
                    }

                    switch (exception.getCode()) {
                    }

                    ThreadedTaskManager.MANAGER.doInSeparateThread(this, TestP2PManager.DISCONNECT);
                }
                break;

            case DO_EXCHANGE:
                try {
                    final byte[] temp = new byte[512];
                    int read;

                    boolean isServer = true;

                    try {
                        isServer = this.mP2pConnection
                                .getParameter(P2PConnectionParameter.IS_SERVER) == 1;
                    } catch (final NfcException exception) {
                        if (TestP2PManager.DEBUG) {
                            Log.d(TestP2PManager.TAG, "Is server failed ! ", exception);
                        }

                        ThreadedTaskManager.MANAGER.doInSeparateThread(this,
                                TestP2PManager.ESTABLISH_LINK);
                        return;
                    }

                    if (TestP2PManager.DEBUG) {
                        Log.d(TestP2PManager.TAG, "isServer=" + isServer);
                    }

                    if (isServer == true) {
                        for (final byte[] message : TestP2PManager.messages) {
                            this.mP2pConnection.write(message);

                            if (TestP2PManager.DEBUG) {
                                Log.d(TestP2PManager.TAG,
                                        ">WRITE> " + HelperForNfc.byteArrayToString(message));
                            }

                            read = this.mP2pConnection.read(temp);

                            if (TestP2PManager.DEBUG) {
                                Log.d(TestP2PManager.TAG,
                                            "Read = " + read + " : "
                                                    + HelperForNfc.byteArrayToString(temp, 0, read));
                            }
                        }
                    } else {
                        for (final byte[] message : TestP2PManager.messages) {
                            read = this.mP2pConnection.read(temp);

                            if (TestP2PManager.DEBUG) {
                                Log.d(TestP2PManager.TAG,
                                            "Read = " + read + " : "
                                                    + HelperForNfc.byteArrayToString(temp, 0, read));
                            }

                            this.mP2pConnection.write(message);

                            if (TestP2PManager.DEBUG) {
                                Log.d(TestP2PManager.TAG,
                                        ">WRITE> " + HelperForNfc.byteArrayToString(message));
                            }
                        }
                    }

                    if (TestP2PManager.DEBUG) {
                        Log.d(TestP2PManager.TAG, "***   END of test   ***");
                    }

                    this.stopTest();
                } catch (final IOException exception) {
                    if (TestP2PManager.DEBUG) {
                        Log.d(TestP2PManager.TAG, "Communictaion break, try again ...", exception);
                    }

                    ThreadedTaskManager.MANAGER.doInSeparateThread(this,
                            TestP2PManager.ESTABLISH_LINK);
                }
                break;

            case DISCONNECT:
                if (this.link != null) {
                    try {
                        this.link.close();
                    } catch (final IOException exception) {
                        if (TestP2PManager.DEBUG) {
                            Log.d(TestP2PManager.TAG, "link close failed", exception);
                        }
                    }
                }
                this.link = null;

                if (this.mP2pConnection != null) {
                    this.mP2pConnection.cancel();

                    try {
                        this.mP2pConnection.close();
                    } catch (final IOException exception) {
                        if (TestP2PManager.DEBUG) {
                            Log.d(TestP2PManager.TAG, "Connection close failed", exception);
                        }
                    }
                }
                this.mP2pConnection = null;

                if (TestP2PManager.DEBUG) {
                    Log.d(TestP2PManager.TAG, "Disocnected !");
                }

                if (TestP2PManager.DEBUG) {
                    Log.v(TestP2PManager.TAG, "Go for an other loop");
                }

                ThreadedTaskManager.MANAGER.doInSeparateThread(this,
                        TestP2PManager.CREATE_CONNECTION);

                break;
        }
    }

    /**
     * Cal when P2P link created <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @param link Created link
     * @see org.opennfc.p2p.P2PLinkEventHandler#onLinkDetected(org.opennfc.p2p.P2PLink)
     */
    @Override
    public void onLinkDetected(final P2PLink link) {
        if (TestP2PManager.DEBUG) {
            Log.d(TestP2PManager.TAG, "onLinkDetected");
        }

        this.link = link;

        ThreadedTaskManager.MANAGER.doInSeparateThread(this, TestP2PManager.CONNECT_WITH_LINK);
    }

    /**
     * Call when link error append <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @see org.opennfc.p2p.P2PLinkEventHandler#onLinkError()
     */
    @Override
    public void onLinkError() {
        if (TestP2PManager.DEBUG) {
            Log.e(TestP2PManager.TAG, "onLinkError");
        }

        ThreadedTaskManager.MANAGER.doInSeparateThread(this, TestP2PManager.ESTABLISH_LINK);
    }

    /**
     * Call when link connection released <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @see org.opennfc.p2p.P2PLinkEventHandler#onLinkReleased()
     */
    @Override
    public void onLinkReleased() {
        if (TestP2PManager.DEBUG) {
            Log.w(TestP2PManager.TAG, "onLinkReleased");
        }

        ThreadedTaskManager.MANAGER.doInSeparateThread(this, TestP2PManager.ESTABLISH_LINK);
    }

    /**
     * Launch the tester <br>
     * <br>
     * <u><b>Documentation from parent :</b></u><br> {@inheritDoc}
     * 
     * @see org.opennfc.tests.TestScenario#start()
     */
    @Override
    protected void start() {
        ThreadedTaskManager.MANAGER.doInSeparateThread(this, TestP2PManager.CREATE_CONNECTION);
    }
}
