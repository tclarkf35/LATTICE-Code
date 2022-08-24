#include "Shuttle.h"

#include "TakeupUtil.h"

using namespace lattice;

Shuttle::Shuttle() {
}
void Shuttle::Setup() {
    mLeftMotionMotor.Setup();
    mRightMotionMotor.Setup();

    mLeftTensionMotor.Setup();
    mRightTensionMotor.Setup();

    mLeftArmLimitSwitch.Setup();
    mRightArmLimitSwitch.Setup();

    mLeftLimitSwitch.Setup();
    mRightLimitSwitch.Setup();
    mCenterLimitSwitch.Setup();
}
void Shuttle::SetTakeup(double takeup) {
    mTargetTakeup = takeup;
    mOneArmTakeupAngle = SingleArmTakeupAngle(takeup, kLengthArm, kDistancePassivePulleyArmPulley, kDistancePassivePulleyDrivePulley);
    mTwoArmTakeupAngle = DoubleArmTakeupAngles(takeup, kLengthArm, kDistancePassivePulleyArmPulley, kDistancePassivePulleyDrivePulley);
}

void Shuttle::SetFrontLimitSwitch(FrontLimitSwitch front) {
    mFrontLimitSwitch = front;
}

bool Shuttle::ArmTransition(ArmPosition pos) {
    // TODO
    return true;
}
void Shuttle::UpdateSensors() {
    mLeftArmLimitSwitch.Update();
    mRightArmLimitSwitch.Update();

    mLeftLimitSwitch.Update();
    mRightLimitSwitch.Update();
    mCenterLimitSwitch.Update();
}

void Shuttle::EngageMotorBreak() {
    // TODO
}

void Shuttle::DisengageMotorBreak() {
    // TODO
}

void Shuttle::SetMotionMotors(double RPM) {
    mLeftMotionMotor.EnableMotor();
    mRightMotionMotor.EnableMotor();
    mLeftMotionMotor.SetVelocity(RPM);
    mRightMotionMotor.SetVelocity(RPM);
}
void Shuttle::SetTensionArms(double leftArmAngle, double rightArmAngle) {
    // TODO
}
void Shuttle::ResetTensionArms() {
    mTargetArmPos = kBothArmsLowered;
    if (!mLeftArmLimitSwitch.Get()) {
        mLeftTensionMotor.SetPercentOutput(-kPercentPowerReset);
    } else {
        mLeftTensionMotor.SetPercentOutput(0.0);
    }

    if (!mRightArmLimitSwitch.Get()) {
        mRightTensionMotor.SetPercentOutput(-kPercentPowerReset);
    } else {
        mRightTensionMotor.SetPercentOutput(0.0);
    }
}
void Shuttle::StopMotionMotors() {
    mLeftMotionMotor.DisableMotor();
    mRightMotionMotor.DisableMotor();
}

bool Shuttle::StakeTransition() {
    ArmPosition preTransition;
    ArmPosition postTransition;
    if (mFrontLimitSwitch == kLeft) {
        preTransition = kRightArmRaised;
        postTransition = kLeftArmRaised;
    } else {  // Right side is front
        preTransition = kLeftArmRaised;
        postTransition = kRightArmRaised;
    }
    if (mStakeTransitionState == kHitRail) {
        StopMotionMotors();
        if (ArmTransition(preTransition)) {
            mStakeTransitionState = kGoingOnRail;
        }
    } else if (mStakeTransitionState == kGoingOnRail) {
        if (!mCenterLimitSwitch.Get()) {
            SetMotionMotors(kRailRPM);
        } else {
            SetMotionMotors(0.0);
            mStakeTransitionState = kCenterOfRail;
        }

    } else if (mStakeTransitionState == kCenterOfRail) {
        SetMotionMotors(0.0);
        if (ArmTransition(postTransition)) {
            mStakeTransitionState = kGoingOffRail;
        }
    } else if (mStakeTransitionState == kGoingOffRail) {
        if ((mFrontLimitSwitch == kLeft && !mRightLimitSwitch.Get()) || (mFrontLimitSwitch == kRight && !mLeftLimitSwitch.Get())) {
            SetMotionMotors(kRailRPM);
        } else {
            StopMotionMotors();
            mStakeTransitionState = kResettingArms;
        }
    } else if (mStakeTransitionState == kResettingArms) {
        if (ArmTransition(kBothArmsRaised)) {
            mStakeTransitionState = kDone;
        }
    } else if (mStakeTransitionState == kDone) {
        return true;
    } else {
        // Error
    }
    return false;
}

bool Shuttle::ConstantTakeupDrive() {
    if (mLeftLimitSwitch.Get() || mRightLimitSwitch.Get() || mTargetArmPos != kBothArmsRaised) {
        StopMotionMotors();
        if (mTargetArmPos != kBothArmsRaised) {
            // TODO: Log somewhere that the arms aren't ready for a constant takeup drive
        } else if ((mLeftLimitSwitch.Get() && mFrontLimitSwitch == kLeft) || (mRightLimitSwitch.Get() && mFrontLimitSwitch == kRight)) {  // If front limit switch is hit start stake transition
            mStakeTransitionState = kHitRail;
            return true;
        } else {
            // TODO: Log that something isn't right most likely that the front of the robot wasn't configured properly
        }
    } else {
        SetMotionMotors(kDriveRPM);
    }
}
void Shuttle::EStop() {
    StopMotionMotors();

    mLeftTensionMotor.SetPercentOutput(0);
    mRightTensionMotor.SetPercentOutput(0);

    // Engage Motor break to prevent arms from moving
    EngageMotorBreak();
}