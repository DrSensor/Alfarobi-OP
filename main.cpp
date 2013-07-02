#define LIMIT_CIRCLE_DETECTION      5
#define BALL_MAX_AREA   222
#define BALL_MIN_AREA   23

#include <iostream>

#include "std/threading.h"
#include "std/speed_test.h"
#include "imgproc/momentdetection.h"
#include "imgproc/circledetection.h"
#include "motion/head.h"
#include "motion/action.h"

#include <opencv2/highgui/highgui.hpp>

#define forever         1
#define Loop(x)         while(x)
#define Do_Forever_Move while (arg == NULL)

using namespace std;
using namespace cv;

pthread_t gerakan_t;
struct BallState {
    bool not_found, is_far, is_near, in_foot_range, ready_to_kick;
    int8_t circle_counter; bool fail_to_detect_circle = false;
}s_ball;
struct GoalState {
    bool not_found, in_search, probably_in_left, probably_in_right;
}s_goal;


#define kepala  Head::GetInstance()
#define lakukan Action::GetInstance()
MomentDetection ball, goal;
CircleDetection circle;

void setPos(const uint32_t posX, const uint32_t posY, const uint32_t posZ);
void setBallState();
void setGoalState();

void *actionStateMachine(void *arg)
{
    Do_Forever_Move {

    }

    return NULL;
}

int main()
{
    VideoCapture cam;
    Mat src, dst;

    if (!cam.open(-1)) {
        cerr << "Error : Couldn't open camera !\n";
        exit(1);
    }

    threadInitialize(gerakan_t, actionStateMachine, 30);

    Loop(forever) {
        cam >> dst;
        ball.threshold(&dst);
        ball.moment(&dst);
        cout << "Moment Ball : "
             << " x=" << ball.getX()
             << " y=" << ball.getY()
             << " area=" << ball.getArea() << endl;
        setPos(ball.getX(), ball.getY(), ball.getArea());
        setBallState();

        while (s_ball.not_found || !s_ball.fail_to_detect_circle) {
            circle.threshold(&dst);
            if (circle.houghTransform(&dst) != 0)
                s_ball.fail_to_detect_circle = false;
            else {
                s_ball.circle_counter++;
                if (s_ball.circle_counter > LIMIT_CIRCLE_DETECTION)
                    s_ball.fail_to_detect_circle = true;
                break;
            }
            cout << "Circle Ball : "
                 << " x=" << circle.getX()
                 << " y=" << circle.getY()
                 << " radius=" << circle.getRadius() << endl;
            setPos(circle.getX(), circle.getY(), circle.getRadius());
            setBallState();
            cam >> dst;
        }

        while (s_goal.in_search) {
            goal.threshold(&dst);
            goal.moment(&dst);
            cout << "Moment Ball : "
                 << " x=" << goal.getX()
                 << " y=" << goal.getY()
                 << " area=" << goal.getArea() << endl;
            setPos(goal.getX(), goal.getY(), goal.getArea());
            setGoalState();
            cam >> dst;
        }

    } NeverEndHere;

    return 0;
}

void setPos(const uint32_t posX, const uint32_t posY, const uint32_t posZ)
{
}


void setBallState()
{
}


void setGoalState()
{
}
