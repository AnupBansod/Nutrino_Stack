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

package org.opennfc.hardware;

import org.opennfc.NfcException;
import org.opennfc.NfcErrorCode;

/**
 * This class controls the hardware features of the NFC Controller. Only a
 * system-level application should use this class.
 *
 * @since Open NFC 4.0
 */
public interface NfcController {

    public enum Mode {
        /** A boot procedure is pending. */
        BOOT_PENDING(ConstantAutogen.W_NFCC_MODE_BOOT_PENDING),

        /** Maintenance mode, the firmware can be used. */
        MAINTENANCE(ConstantAutogen.W_NFCC_MODE_MAINTENANCE),

        /** Maintenance mode, no firmware is present. */
        NO_FIRMWARE(ConstantAutogen.W_NFCC_MODE_NO_FIRMWARE),

        /** Maintenance mode, the version of the firmware is not supported by the stack */
        FIRMWARE_NOT_SUPPORTED(ConstantAutogen.W_NFCC_MODE_FIRMWARE_NOT_SUPPORTED),

        /** The NFC Controller is not responding. */
        NOT_RESPONDING(ConstantAutogen.W_NFCC_MODE_NOT_RESPONDING),

        /** The version of the loader is not supported. */
        LOADER_NOT_SUPPORTED(ConstantAutogen.W_NFCC_MODE_LOADER_NOT_SUPPORTED),

        /** The NFC Controller is active. */
        ACTIVE(ConstantAutogen.W_NFCC_MODE_ACTIVE),

        /** The NFC Controller is switching from active mode to standby mode */
        SWITCH_TO_STANDBY(ConstantAutogen.W_NFCC_MODE_SWITCH_TO_STANDBY),

        /** The NFC Controller is in standby mode */
        STANDBY(ConstantAutogen.W_NFCC_MODE_STANDBY),

        /** The NFC Controller is switching from standby mode to active mode */
        SWITCH_TO_ACTIVE(ConstantAutogen.W_NFCC_MODE_SWITCH_TO_ACTIVE);

        /* the numeric identifier */
        private int identifier;

        /* private constructor */
        private Mode(int identifier) {
            this.identifier = identifier;
        }

        /**
         * Returns the mode value.
         *
         * @hide
         */
        public int getValue() {
            return this.identifier;
        }
    }

    /**
     * Resets the NFC controller.
     *
     * @param mode the NFC controller mode be entered after the reset operation:
     *             <ul>
     *             <li>{@link Mode#MAINTENANCE Mode.MAINTENANCE} for the maintenance mode, or</li>
     *             <li>{@link Mode#ACTIVE Mode.ACTIVE} for the active mode.</li>
     *             </ul>
     *
     * @throws IllegalArgumentException if <code>mode</code> is null or has a wrong value.
     * @throws IllegalStateException reset() cannot be called while a reset is already pending.
     * @throws NfcException an error occured with the NFC Controller.
     */
    void reset(Mode mode) throws NfcException;

    /**
     * Returns the name and the versio of a Firmware.
     *
     * @param firmware the firmware buffer.
     *
     * @return the property value.
     *
     * @throws IllegalArgumentException if <code>firmware</code> is null.
     * @throws NfcException if an error is detected in the firmware format.
     *
     * @see #firmwareUpdate firmwareUpdate()
     */
    String getFirmwareInfo(byte[] firmware) throws NfcException;

    /**
     * Updates the firmware and/or the configuration of the NFC controller.
     *
     * <p>The NFC Controller firmware may be updated with a new version of the firmware software.
     * The firmware file shall be provided by the NFC Controller manufacturer.</p>
     *
     * <p>After a successful update operation, the NFC Controller is rebooted.
     * The persistent information is reset to its default value.</p>
     *
     * <p>The update operation may fail if it is not completed (due to a power loss for example)
     * or if the new firmware is not validated by the NFC Controller (wrong format, wrong version, or wrong signature).
     * In case of failed update, the previous version of the firmware or its persistent data may be corrupted.
     * This may cause the NFC Controller invalidating the current firmware and the NFC Controller mode becomes
     * {@link Mode#NO_FIRMWARE Mode.NO_FIRMWARE}. If the previous firmware and its persistent data are not corrupted,
     * the NFC Controller is rebooted in the mode {@link Mode#MAINTENANCE Mode.MAINTENANCE}
     * or {@link Mode#ACTIVE Mode.ACTIVE}.</p>
     *
     * <p>The method {@link #getFirmwareInfo getFirmwareInfo()} retrieves the information of a firmware file.</p>
     *
     * <p>The firmware update operation may require several seconds to complete.
     * The method {@link #getFirmwareUpdateState()} may be used to follow the progression of the operation.</p>
     *
     * <p><b>Important Note</b></p>
     *
     * <p>If a UICC is connected by SWP to the NFC Controller, the UICC should be rebooted after a Firmware update.
     * Rebooting the UICC may require rebooting the NFC device. The explanation is the following:
     * When a firmware is downloaded in the NFC Controller, the UICC configuration stored in the NFC Controller is lost.
     * There is no message in the ETSI specification to inform the UICC of this configuration loss.
     * The UICC can rebuild the configuration only during UICC power up sequence.</p>
     *
     * <p>The firmware update is provided in <code>firmware</code>. The format of this buffer is proprietary.</p>
     *
     * <p>firmwareUpdate() should be called from one of the maintenance modes.</p>
     *
     * @param firmware the firmware binary value.
     * @param mode the NFC controller mode to be entered after the update operation:
     *             <ul>
     *             <li>{@link Mode#MAINTENANCE Mode.MAINTENANCE} for the maintenance mode, or</li>
     *             <li>{@link Mode#ACTIVE Mode.ACTIVE} for the active mode.</li>
     *             </ul>
     *
     * @throws IllegalArgumentException if <code>firmware</code> is null.
     * @throws IllegalArgumentException if <code>mode</code> is null or has a wrong value.
     * @throws IllegalStateException firmwareUpdate() is called while the NFC controller is not in one the maintenance modes.
     * @throws NfcException with one of the following error code:
     *         <ul>
     *         <li>{@link NfcErrorCode#TIMEOUT} A timeout occured during the communication with the NFC controller.</li>
     *         <li>{@link NfcErrorCode#BAD_FIRMWARE_FORMAT} An error is detected in the format of the firmware update buffer.</li>
     *         <li>{@link NfcErrorCode#BAD_FIRMWARE_VERSION} The firmware is not compliant with the NFC Controller type,
     *         with the NFC HAL Binding or with the current configuration of the NFC Controller.</li>
     *         <li>{@link NfcErrorCode#BAD_FIRMWARE_SIGNATURE} The signature of the firmware update buffer is not valid.</li>
     *         <li>{@link NfcErrorCode#DURING_FIRMWARE_BOOT} The new firmware does not boot correctly.</li>
     *         </ul>
     *
     * @see #getFirmwareUpdateState
     * @see #getFirmwareInfo getFirmwareInfo()
     */
    void firmwareUpdate(byte[] firmware, Mode mode) throws NfcException;

    /**
     * Returns the current progress status of a firmware update operation.
     *
     * @return The progression value is the current number of bytes
     *         of the firmware buffer downloaded in the NFC Controller.
     *         This value is zero if no update operation is pending.
     *
     * @see #firmwareUpdate firmwareUpdate()
     */
    int getFirmwareUpdateState();

    /**
     * Returns the current mode of the NFC controller.
     *
     * @return The current mode as described in <a href="{@docRoot}overview-summary.html#nfcc_modes">NFC Controller Modes</a>.
     */
    Mode getMode();

    /**
     * Starts monitoring the NFC controller exceptions.
     * <p/>
     * After a successful registration, the handler is called each time an
     * exception is detected. {@link #unregisterExceptionEventHandler
     * unregisterExceptionEventHandler()} should be called to unregister the
     * listener.
     * <p/>
     *
     * @param handler the {@link NfcControllerExceptionEventHandler} whose
     *            {@link NfcControllerExceptionEventHandler#onExceptionOccured}
     *            method will be called when an exception is detected.
     * @throws IllegalArgumentException if <code>handler</code> is null.
     * @throws IllegalArgumentException if <code>handler</code> is already
     *             registered.
     * @throws NfcException in case of a registration error occurs.
     * @see #unregisterExceptionEventHandler unregisterExceptionEventHandler()
     * @see NfcControllerExceptionEventHandler
     */
    void registerExceptionEventHandler(NfcControllerExceptionEventHandler handler)
            throws NfcException;

    /**
     * Stops monitoring NFC controller exceptions registered with
     * {@link #registerExceptionEventHandler}.
     *
     * @param handler the handler in case of exception
     * @throws IllegalArgumentException if <code>handler</code> is null.
     * @throws IllegalArgumentException if <code>handler</code> is not already
     *             registered.
     * @see #registerExceptionEventHandler registerExceptionEventHandler()
     */
    void unregisterExceptionEventHandler(NfcControllerExceptionEventHandler handler);

    /**
     * Performs self-test of the NFC controller.
     *
     * <p>The test is executed and the method returns without exception if the test succeeds.
     * The self-test must be executed when the NFC Controller is in maintenance mode.
     * The precise description of the self test is provided in the documentation of the NFC Controller.</p>
     *
     * @throws NfcException if the test fails or if an error occurs.
     */
    void selfTest() throws NfcException;

    /**
     * Performs a NFC controller production test.
     *
     * <p>The test parameters are given in a buffer.
     * A result buffer is provided to receive a result.
     * Production tests do not always return a test result.
     * The test result is obtained from the measurements performed by external test devices.</p>
     *
     * <p>Production tests shall be executed when the NFC Controller is in maintenance mode.
     * Any ongoing production test should be stopped before starting a new test.</p>
     *
     * <p>The precise description of the production tests and the associated
     * parameters is provided in the release notes of the NFC HAL implementation
     * for each NFC Controller.</p>
     *
     * @param testCommand the test command to be executed.
     * @return the test command answer.
     *
     * @throws NfcException if an error occurs
     */
    byte[] productionTest(byte[] testCommand) throws NfcException;

    /**
     * Switches the forced standby mode on or off.
     *
     * <p>Calling switchStandbyMode() initiates the switch to the active mode or to the standby mode.
     * The NFC Controller may need some time to switch to the requested mode after the return of the method.
     * See <a href="{@docRoot}overview-summary.html#nfcc_modes">NFC Controller Modes</a>.</p>
     *
     * <p>Should be called in one of the following modes:
     * <ul>
     *  <li>{@link Mode#ACTIVE Mode.ACTIVE},</li>
     *  <li>{@link Mode#SWITCH_TO_STANDBY Mode.SWITCH_TO_STANDBY},</li>
     *  <li>{@link Mode#STANDBY Mode.STANDBY}, or</li>
     *  <li>{@link Mode#SWITCH_TO_ACTIVE Mode.SWITCH_TO_ACTIVE}.</li>
     * </ul></p>
     *
     * @param standbyOn if forced standby mode is enabled or not.
     *
     * @throws IllegalStateException if the NFC Controller is not in a valid mode to call this method.
     * @throws NfcException  if the standby mode is not supported by the NFC Controller.
     */
    void switchStandbyMode(boolean standbyOn) throws NfcException;

    /**
     * Registers a new card collision handler.
     *
     * <p>A collision is detected when the following conditions are met:
     *  <ul>
     *    <li>the NFC Device is listening on the NFC RF,</li>
     *    <li>a collision is detected, and</li>
     *    <li>collision-resolution is not supported or does not apply.</li>
     *  </ul>
     * Then, an event is sent to the listener.
     * The NFC Device firmware should handle this event and should display
     * a message to the user such as
     * &quot;Multiple cards or tags detected, please present only one&quot;.</p>
     *
     * <p>
     * After a successful registration, the handler is called each time a new
     * card collision is detected. {@link #unregisterCardCollisionHandler
     * unregisterCardCollisionHandler()} should be called to unregister the
     * listener.
     * </p>
     *
     * @param handler the {@link CollisionEventHandler} whose
     *            {@link CollisionEventHandler#onCollisionDetected} method will
     *            be called when a collision has been detected.
     *
     * @throws IllegalArgumentException if <code>handler</code> is null.
     * @throws IllegalArgumentException if <code>handler</code> is already registered.
     * @throws NfcException in case of a registration error occurs.
     *
     * @see #unregisterCardCollisionHandler unregisterCardCollisionHandler()
     * @see CollisionEventHandler
     */
    void registerCardCollisionHandler(CollisionEventHandler handler) throws NfcException;

    /**
     * Unregisters a card collision handler registered with
     * {@link #registerCardCollisionHandler}.
     *
     * @param handler the card detection handler.
     * @throws IllegalArgumentException if <code>handler</code> is null.
     * @throws IllegalArgumentException if <code>handler</code> is not already registered.
     * @see #registerCardCollisionHandler registerCardCollisionHandler()
     */
    void unregisterCardCollisionHandler(CollisionEventHandler handler);

    /**
     * Registers a new unknown card handler.
     * <p/>
     * After a successful registration, the handler is called each time a new
     * new unknown card is detected. {@link #unregisterUnknownCardHandler
     * unregisterUnknownCardHandler()} should be called to unregister the
     * listener.
     * <p/>
     *
     * @param startDetection forces a permanent detection.
     * @param handler the {@link UnknownCardEventHandler} whose
     *            {@link UnknownCardEventHandler#onUnknownCardDetected} method
     *            will be called when an unknown card has been presented.
     * @throws IllegalArgumentException if <code>handler</code> is null.
     * @throws IllegalArgumentException if <code>handler</code> is already
     *             registered.
     * @throws NfcException in case of a registration error occurs.
     * @see #unregisterUnknownCardHandler unregisterUnknownCardHandler()
     * @see UnknownCardEventHandler
     */
    void registerUnknownCardHandler(boolean startDetection, UnknownCardEventHandler handler)
            throws NfcException;

    /**
     * Unregisters a unknown card handler registered with
     * {@link #registerUnknownCardHandler}.
     *
     * @param handler the unknown card handler.
     * @throws IllegalArgumentException if <code>handler</code> is null.
     * @throws IllegalArgumentException if <code>handler</code> is not already
     *             registered.
     * @see #registerUnknownCardHandler registerUnknownCardHandler()
     */
    void unregisterUnknownCardHandler(UnknownCardEventHandler handler);
}
