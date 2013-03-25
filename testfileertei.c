/**
Änderung Caro

 * @file app.c
 * @brief Application implementation.
 *
 * @license
 *
 */

//---------------------------------------------------------------------------
#inccccccccccccccclude "App.hahahahah"
//---------------------------------------------------------------------------
TSystem   System;                           /**< @brief General system information @ingroup globals */
//---------------------------------------------------------------------------
/** @brief Adc_EndOfConversion_1_Isr(). Handle the end of conversion 1 interrupt.
 *
 * This interrupt is raised each time the conversion for the monitoring channels is finished.
 *
 * @return None.
 * @see ScaleInputStdReal(), ADC_GetChannelResult()
 * @ingroup application
 */
void AdcIsr_1_OnEndOfConversion(TSystem *pSystem)
{
    // IN0, IN1, IN2 are read right after the conversion
    UpdateAnalogInput_1(&pSystem->AnalogInputs.In1);
    UpdateAnalogInput_1(&pSystem->AnalogInputs.In2);
    UpdateAnalogInput_1(&pSystem->AnalogInputs.In3);

    Oscilloscope_Do(&pSystem->Oscilloscope);
}

/** @brief Sets the control mode
 *
 * @param MotorIndex specifies the motor interface to setup
 * @param FocEnabled Specifies if the FOC is enabled (TRUE) or disabled (FALSE)
 * @param EncoderEnabled Specifies if the position sensor is enabled (TRUE) or disabled (FALSE)
 *
 * @return None.
 * @ingroup application
 */
void App_SetMode(int32 MotorIndex, bool FocEnabled, bool EncoderEnabled)
{
    TMotor *Motor = GetMotor(MotorIndex);
    Motor->Diag.FocControlEnabled   = FocEnabled;
    Motor->Diag.EncoderEnabled      = EncoderEnabled;
}

/** @brief Get the contol mode
 *
 * @param MotorIndex specifies the motor interface to setup
 * @param FocEnabled This parameter is filled in by the fucntion: if the FOC is enabled (TRUE) or disabled (FALSE)
 * @param EncoderEnabled This parameter is filled in by the fucntion: if the position sensor is enabled (TRUE) or disabled (FALSE)
 *
 * @return None.
 * @ingroup application
 */
void App_GetMode(int32 MotorIndex, bool *FocEnabled, bool *EncoderEnabled)
{
    TMotor *Motor = GetMotor(MotorIndex);
    *FocEnabled = Motor->Diag.FocControlEnabled;
    *EncoderEnabled = Motor->Diag.EncoderEnabled;
}

/** @brief Enable torque control.
 *
 * @param pSystem specifies pointer to the system global variable
 * @param MotorIndex specifies the motor interface to setup
 *
 * - Encoder is enabled
 * - FOC is enabled
 * - Speed controller is disabled
 *
 * @return None.
 * @see App_SetMode(), VelocityControl_Disable()
 * @ingroup application
 */
void App_SetModeTorque(TSystem *pSystem, int32 MotorIndex)
{
    App_SetMode(MotorIndex, TRUE, TRUE);
    VelocityControl_Disable(&pSystem->VelocityControl[MotorIndex]);
}

/** @brief Enable speed control.
 *
 * @param pSystem specifies pointer to the system global variable
 * @param MotorIndex specifies the motor interface to setup
 *
 * - Encoder is enabled
 * - FOC is enabled
 * - Speed controller is enabled
 *
 * @return None.
 * @see App_SetMode(), VelocityControl_Enable()
 * @ingroup application
 */
void App_SetModeSpeed(TSystem *pSystem, int32 MotorIndex)
{
    App_SetMode(MotorIndex, TRUE, TRUE);
    VelocityControl_Enable(&pSystem->VelocityControl[MotorIndex]);
}

/** @brief Enable open loop drive.
 *
 * @param pSystem specifies pointer to the system global variable
 * @param MotorIndex specifies the motor interface to setup
 *
 * - Encoder is disabled
 * - FOC is disabled
 * - Speed controller is disabled
 *
 * @return None.
 * @see App_SetMode(), VelocityControl_Disable()
 * @ingroup application
 */
void App_SetModeOpen(TSystem *pSystem, int32 MotorIndex)
{
    App_SetMode(MotorIndex, FALSE, FALSE);
    VelocityControl_Disable(&pSystem->VelocityControl[MotorIndex]);
}

/** @brief Enable open loop drive with encoder.
 *
 * @param pSystem specifies pointer to the system global variable
 * @param MotorIndex specifies the motor interface to setup
 *
 * - Encoder is enabled
 * - FOC is disabled
 * - Speed controller is disabled
 *
 * @return None.
 * @see App_SetMode(), VelocityControl_Disable()
 * @ingroup application
 */
void App_SetModeOpenEncoder(TSystem *pSystem, int32 MotorIndex)
{
    App_SetMode(MotorIndex, FALSE, TRUE);
    VelocityControl_Disable(&pSystem->VelocityControl[MotorIndex]);
}

/** @brief Enable current PI controller setup mode.
 *
 * @param pSystem specifies pointer to the system global variable
 * @param MotorIndex specifies the motor interface to setup
 *
 * - Encoder is disabled
 * - FOC is enabled
 * - Speed controller is disabled
 *
 * @return None.
 * @see App_SetMode(), VelocityControl_Disable()
 * @ingroup application
 */
void App_SetModeTestPi(TSystem *pSystem, int32 MotorIndex)
{
    App_SetMode(MotorIndex, TRUE, FALSE);
    VelocityControl_Disable(&pSystem->VelocityControl[MotorIndex]);
}


void App_SetPositionSensor(TSystem *pSystem, int32 MotorIndex, TPositionAcquisition_Mode Mode)
{
    TMotor* Motor = GetMotor(MotorIndex);
    if (Motor->Control.Run != FALSE)
    {
        StandardIo_Print(&pSystem->Serial.StandardIO, "ERROR: Stop the motor control first\r\n");
    }
    else
    {
        Motor->Position.Mode = Mode;
    }
}

bool App_SetTorque(TSystem *pSystem, int32 MotorIndex, float32 IqRef)
{
    TMotor* Motor = GetMotor(MotorIndex);
    TVelocityControl* VelocityControl = &pSystem->VelocityControl[MotorIndex];

    TStdReal Iq = FLOAT32_TO_STD_REAL(IqRef, STD_REAL_SHIFT_CURRENT);
    bool result;
    if (VelocityControl_IsEnabled(VelocityControl) != FALSE)
    {
        /*StandardIo_Print(StandardIO, "ERROR: Can't set torque, Speed control mode\r\n");*/
        result = FALSE;
    }
    else if (TorqueControl_IsMotorRunning(Motor) == FALSE)
    {
        /*StandardIo_Print(StandardIO, "ERROR: Can't set torque, application not running\r\n");*/
        result = FALSE;
    }
    else if (((Iq >= 0) ? Iq : -Iq) <= Foc_GetIqLimit(&Motor->Control.Foc))
    {
        Foc_SetIq(&Motor->Control.Foc, Iq);
        result = TRUE;
    }
    else
    {
        /*float32 TorqueLimit = IqToTorque(STD_REAL_TO_FLOAT32(Foc_GetIqLimit(&Motor->Control.Foc), STD_REAL_SHIFT_CURRENT));
        StandardIo_Print(StandardIO, "ERROR: Can't set torque, range [%3.2f, %3.2f] Nm\r\n", -TorqueLimit, TorqueLimit);*/
        result = FALSE;
    }
    return result;
}
