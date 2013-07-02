#define LIMIT_CIRCLE_DETECTION      5
#define BALL_MAX_AREA   222
#define BALL_MIN_AREA   23
#define BALL_MAX_RADIUS 10
#define BALL_MIN_RADIUS 2

#define KICK_PAN                    10
#define KICK_TILT                   180
#define KICK_FRAME_CENTER_X         10
#define KICK_FRAME_CENTER_Y         10
#define KICK_FRAME_LEFTLEG_X_MAX    5
#define KICK_FRAME_LEFTLEG_X_MIN    2
#define KICK_FRAME_RIGHTLEG_X_MAX   30
#define KICK_FRAME_RIGHTLEG_X_MIN   20

#define RALAT_COMPASS   10

#include <iostream>

#include "std/threading.h"
#include "std/speed_test.h"
#include "std/custom_math.h"
#include "imgproc/momentdetection.h"
#include "imgproc/circledetection.h"
#include "motion/head.h"
#include "motion/action.h"

#include <opencv2/highgui/highgui.hpp>

#define forever         1
#define Loop(x)         while(x)
#define Do_Forever_Move while (arg == NULL)

#define DEFAULT_ACTION_PARAM    1, 5000
#define MOMENT_DETECTION        true
#define CIRCLE_DETECTION        false

using namespace std;
using namespace cv;

pthread_t gerakan_t;
struct MinMax {
    int min,max;
}frameDepth;

struct BallState {
    bool not_found, is_far, is_near, in_foot_range, ready_to_kick;
    int8_t circle_counter; bool fail_to_detect_circle = false;
}s_ball;
struct GoalState {
    bool not_found, in_search, probably_in_left, probably_in_right;
}s_goal;
struct Position {
    int x,y,z;
}frame;
uint16_t compassHeading, enemyGoal;

#define kepala  Head::GetInstance()
#define lakukan Action::GetInstance()
MomentDetection m_ball, m_goal;
CircleDetection m_circle;

void setPos(const uint32_t posX, const uint32_t posY, const uint32_t posZ, bool type);
void setBallState();
void setGoalState();

void cam_init(VideoCapture &cam);

void *actionStateMachine(void *arg)
{
    kepala->setTiltingPID(1/7, 1/14);
    kepala->setPanningPID(1/8, 1/16);

    Do_Forever_Move {
        if (s_ball.not_found) {
            kepala->sinusoidalSearch(1, 500);
            lakukan->action(ACTION_ROT_KA_PA);
        }
        else {
            kepala->targetLock(frame.x, frame.y);
            if (s_ball.is_far) {
                kepala->setTiltingPID(1/7, 1/14);
                kepala->setPanningPID(1/8, 1/16);
                lakukan->action(ACTION_MAJU_CEPAT);
            }
            if (s_ball.is_near) {
                kepala->setTiltingPID(1/6, 1/12);
                kepala->setPanningPID(1/7, 1/14);
                lakukan->action(ACTION_MAJU_LAMBAT);
            }
            if (s_ball.in_foot_range) {
                // cek kompas -> revolusi
                while (!(constraintErr(compassHeading, enemyGoal, RALAT_COMPASS))) {
                    if (compassHeading > enemyGoal)
                        lakukan->action(ACTION_REV_KA);
                    else if (compassHeading < enemyGoal)
                        lakukan->action(ACTION_REV_KI);
                    kepala->targetLock(frame.x, frame.y);
                }

                // cek gawang -> revolusi | geser
                s_goal.in_search = true;
                kepala->sweepRight(180, 10, 5000);
                kepala->sweepLeft(360, 20, 5000);
                s_goal.in_search = false;
                if (s_goal.probably_in_left)
                    lakukan->action(ACTION_ROT_KI);
                if (s_goal.probably_in_right)
                    lakukan->action(ACTION_ROT_KA);

                if (s_ball.ready_to_kick) {
                    kepala->moveAtAngle(KICK_TILT, KICK_PAN);
                    // compare frame of leg with frame camera for kick
                    if (between(KICK_FRAME_LEFTLEG_X_MIN, frame.x, KICK_FRAME_LEFTLEG_X_MAX)) {
                        lakukan->action(ACTION_TENDANG_KI);
                        kepala->panning(40);
                    }
                    if (between(KICK_FRAME_RIGHTLEG_X_MIN, frame.x, KICK_FRAME_RIGHTLEG_X_MAX)) {
                        lakukan->action(ACTION_TENDANG_KA);
                        kepala->panning(40);
                    }
                    if (!(s_ball.not_found)) {
                        if (frame.x < KICK_FRAME_CENTER_X)
                            lakukan->action(ACTION_GES_KA);
                        if (frame.x > KICK_FRAME_CENTER_Y)
                            lakukan->action(ACTION_GES_KI);
                    }
                }
            }
        }
    }

    return NULL;
}

int main()
{
    VideoCapture cam;
    Mat src, dst;

    cam_init(cam);
    port_init();

    threadInitialize(gerakan_t, actionStateMachine, 30);

    Loop(forever) {
        cam >> dst;
        m_ball.threshold(&dst);
        m_ball.moment(&dst);
        cout << "Moment Ball : "
             << " x=" << m_ball.getX()
             << " y=" << m_ball.getY()
             << " area=" << m_ball.getArea() << endl;
        setPos(m_ball.getX(), m_ball.getY(), m_ball.getArea(), MOMENT_DETECTION);
        setBallState();

        while (s_ball.is_near || s_ball.in_foot_range && !(s_ball.fail_to_detect_circle)) {
            m_circle.threshold(&dst);
            if (m_circle.houghTransform(&dst) != 0)
                s_ball.fail_to_detect_circle = false;
            else {
                s_ball.circle_counter++;
                if (s_ball.circle_counter > LIMIT_CIRCLE_DETECTION)
                    s_ball.fail_to_detect_circle = true;
                break;
            }
            cout << "Circle Ball : "
                 << " x=" << m_circle.getX()
                 << " y=" << m_circle.getY()
                 << " radius=" << m_circle.getRadius() << endl;
            setPos(m_circle.getX(), m_circle.getY(), m_circle.getRadius(), CIRCLE_DETECTION);
            setBallState();
            cam >> dst;
        }
        s_ball.fail_to_detect_circle = false;

        while (s_goal.in_search) {
            m_goal.threshold(&dst);
            m_goal.moment(&dst);
            cout << "Moment Ball : "
                 << " x=" << m_goal.getX()
                 << " y=" << m_goal.getY()
                 << " area=" << m_goal.getArea() << endl;
            setPos(m_goal.getX(), m_goal.getY(), m_goal.getArea(), MOMENT_DETECTION);
            setGoalState();
            cam >> dst;
        }
    }

    return 0;
}

void setPos(const uint32_t posX, const uint32_t posY, const uint32_t posZ, bool type)
{
    frame.x = posX; frame.y = posY; frame.z = posZ;
    if (type) {
        frameDepth.min = BALL_MIN_AREA;
        frameDepth.max = BALL_MAX_AREA;
    } else {
        frameDepth.min = BALL_MIN_RADIUS;
        frameDepth.max = BALL_MAX_RADIUS;
    }
}

void setBallState()
{
    if ((frame.x == 0 && frame.y == 0) || frame.z == 0)
        s_ball.not_found = true;
    else {
        if (frame.z >= frameDepth.max) {
            s_ball.is_far = true;
            s_ball.is_near = false;
            s_ball.in_foot_range = false;
        }
        else if (frame.z >= frameDepth.min) {
            s_ball.is_near = true;
            s_ball.is_far = false;
            s_ball.in_foot_range = false;
        }
        else if (frame.z <= frameDepth.min) {
            s_ball.in_foot_range = true;
            s_ball.is_near = false;
            s_ball.is_far = false;
        }
    }
}

void setGoalState()
{
}


void cam_init(VideoCapture &cam)
{
    cam.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cam.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

    if (!cam.open(-1)) {
        cerr << "Error : Couldn't open camera !\n";
        exit(1);
    }
}
