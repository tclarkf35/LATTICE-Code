#include <Encoder.h>
#include <Servo.h>
namespace lattice {
/**
 * Class for interfacing with the Hytorc BLDC through the Hobby Star RC Motor Controller
 */
class HytorcRC {
   public:
    /**
     * Constructor for the Hytorc Class
     *
     * @param pwmPort The PWM port that the PWM signal to the motor controller is plugged into
     * @param forwardPort The Hall 1 port of the hall effect (acts as forward signal of a quadrature)
     * @param backwardPort The Hall 2 port of the hall effect (acts as backward signal of a quadrature)
     * @param gearing The gearing of the motor. (50:1 -> 50, 50:2 -> 25, etc)
     */
    HytorcRC(int pwmPort, int forwardPort, int backwardPort, double gearing = 5049);

    /**
     * Setup method for the Hytorc
     */
    void Setup();

    /**
     * Sets the motor controller to a percent output
     *
     * @param percentOut The percentage of the max voltage to set the controller to
     */
    void SetPercentOutput(double percentOut);

    /**
     * Sets the motor controller to a specific voltage based off the current battery voltage
     *
     * @param desiredVoltage The desired voltage to run the motor at
     * @param batteryVoltage The current battery voltage that is being supplied to the motor
     */
    void SetVoltage(double desiredVoltage, double batteryVoltage);

    /**
     * Gets the position of the hall effect sensor in rotations
     *
     * @return The position of the hall effect sensor in rotations
     */
    double GetPosition();

    /**
     * Sets the internal encoder position to zero
     */
    void ResetEncoderPosition();

   private:
    const int kPWMPort;
    const double kGearing;
    static constexpr double kCPR = 16.0;  // Counts Per Revolution
    Servo mMotor;
    Encoder mMotorEncoder;
};

}  // namespace lattice