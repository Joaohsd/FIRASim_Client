#ifndef CORE_H
#define CORE_H

#include <clients/vision/visionclient.h>
#include <clients/referee/refereeclient.h>
#include <clients/actuator/actuatorclient.h>
#include <clients/replacer/replacerclient.h>

#include <src/arguments/arguments.h>
#include <src/defines.h>

#include <src/staticdata.h>
#include <src/robot.h>

class Core : public QObject
{
    Q_OBJECT
public:
    Core();
    void updateArguments(int argc, char** argv);

    void start();
    void initializeTeam(bool team, int formation);
    void sendRef();

    void setTestCondition(bool _test);
    bool getTestCondition();

    Robot *player[NUMBER_OF_TEAMS][NUMBER_OF_ROBOTS];
    StaticData *data;

public slots:
    void update();
    void process();
    void verifyStatus();
    void reposition();
    void stop();

signals:
    //Events
    void readyToProcess();
    void repositioningAlert();

private:
    //Network clients
    VisionClient *visionClient;
    RefereeClient *refereeClient;
    ReplacerClient *replacerClient;
    ActuatorClient *actuatorClient;

    //Code arguments
    Arguments *codeArguments;

    //IDs of each robot
    int att_id = ID_2;
    int def_id = ID_1;
    int goal_id = ID_0;

    //Counter to penalty action
    int max_cont_penalty = 60;
    int max_cont2_penalty = 60;
    int max_cont_defend_penalty = 75;

    //Constant to predict ball position
    double predict_fact = 0.2;   //Para caĺculo da posição futura da bola (TESTAR)
                                 //1.0

    //Flags
    bool defende_penalty=false;
    bool testCondition = true;

    void ballInfo();

    //Inline functions
    /**
     * @brief get_angle
     * @param vec_a Vector A
     * @param vec_b Vector B
     * @return Returns the angle between two vectors
     */

    inline double get_angle(QPoint vec_a, QPoint vec_b){
        double ans = atan2(vec_b.y(),vec_b.x()) - atan2(vec_a.y(),vec_a.x());
        if(ans < -PI)
            ans += 2*PI;
        else if(ans > PI)
            ans -= 2*PI;
        return ans;
    }

    /**
     * @brief is_inside
     * @param point Point A
     * @param field Field A
     * @return Returns true if the point a is inside the field A, and false
     * otherwise
     */
    inline bool is_inside(QPointF point, FieldSpace field){
        double x = point.x();
        double y = point.y();
        if(x <= field.getBottomRight().x() && x >= field.getBottomLeft().x())
        if(x <= field.getTopRight().x() && x >= field.getTopLeft().x())
        if(y <= field.getTopRight().y() && y >= field.getBottomRight().y())
        if(y <= field.getTopLeft().y() && y >= field.getBottomLeft().y())
            return 1;
        return 0;
    }

    /**
     * @brief distance
     * @param point_a Point A
     * @param point_b Point B
     * @return Returns the euclidian distance between two points
     */
    inline double distance(QPoint point_a, QPoint point_b){
        double xx = point_a.x() - point_b.x();
        double yy = point_a.y() - point_b.y();
        return sqrt(xx*xx + yy*yy);
    }
};

#endif // CORE_H
